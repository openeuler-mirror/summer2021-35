void efi_crash_gracefully_on_page_fault(unsigned long phys_addr)
{
	if (!IS_ENABLED(CONFIG_X86_64))
		return;

	/*
	 * If we get an interrupt/NMI while processing an EFI runtime service
	 * then this is a regular OOPS, not an EFI failure.
	 */
	if (in_interrupt())
		return;

	/*
	 * Make sure that an efi runtime service caused the page fault.
	 * READ_ONCE() because we might be OOPSing in a different thread,
	 * and we don't want to trip KTSAN while trying to OOPS.
	 */
	if (READ_ONCE(efi_rts_work.efi_rts_id) == EFI_NONE ||
	    current_work() != &efi_rts_work.work)
		return;

	/*
	 * Address range 0x0000 - 0x0fff is always mapped in the efi_pgd, so
	 * page faulting on these addresses isn't expected.
	 */
	if (phys_addr <= 0x0fff)
		return;

	/*
	 * Print stack trace as it might be useful to know which EFI Runtime
	 * Service is buggy.
	 */
	WARN(1, FW_BUG "Page fault caused by firmware at PA: 0x%lx\n",
	     phys_addr);

	/*
	 * Buggy efi_reset_system() is handled differently from other EFI
	 * Runtime Services as it doesn't use efi_rts_wq. Although,
	 * native_machine_emergency_restart() says that machine_real_restart()
	 * could fail, it's better not to complicate this fault handler
	 * because this case occurs *very* rarely and hence could be improved
	 * on a need by basis.
	 */
	if (efi_rts_work.efi_rts_id == EFI_RESET_SYSTEM) {
		pr_info("efi_reset_system() buggy! Reboot through BIOS\n");
		machine_real_restart(MRR_BIOS);
		return;
	}

	/*
	 * Before calling EFI Runtime Service, the kernel has switched the
	 * calling process to efi_mm. Hence, switch back to task_mm.
	 */
	arch_efi_call_virt_teardown();

	/* Signal error status to the efi caller process */
	efi_rts_work.status = EFI_ABORTED;
	complete(&efi_rts_work.efi_rts_comp);

	clear_bit(EFI_RUNTIME_SERVICES, &efi.flags);
	pr_info("Froze efi_rts_wq and disabled EFI Runtime Services\n");

	/*
	 * Call schedule() in an infinite loop, so that any spurious wake ups
	 * will never run efi_rts_wq again.
	 */
	for (;;) {
		set_current_state(TASK_IDLE);
		schedule();
	}
}
