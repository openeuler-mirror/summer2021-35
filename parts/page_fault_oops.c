static noinline void
page_fault_oops(struct pt_regs *regs, unsigned long error_code,
		unsigned long address)
{
	unsigned long flags;
	int sig;

	if (user_mode(regs)) {
		/*
		 * Implicit kernel access from user mode?  Skip the stack
		 * overflow and EFI special cases.
		 */
		goto oops;
	}

#ifdef CONFIG_VMAP_STACK
	/*
	 * Stack overflow?  During boot, we can fault near the initial
	 * stack in the direct map, but that's not an overflow -- check
	 * that we're in vmalloc space to avoid this.
	 */
	if (is_vmalloc_addr((void *)address) &&
	    (((unsigned long)current->stack - 1 - address < PAGE_SIZE) ||
	     address - ((unsigned long)current->stack + THREAD_SIZE) < PAGE_SIZE)) {
		unsigned long stack = __this_cpu_ist_top_va(DF) - sizeof(void *);
		/*
		 * We're likely to be running with very little stack space
		 * left.  It's plausible that we'd hit this condition but
		 * double-fault even before we get this far, in which case
		 * we're fine: the double-fault handler will deal with it.
		 *
		 * We don't want to make it all the way into the oops code
		 * and then double-fault, though, because we're likely to
		 * break the console driver and lose most of the stack dump.
		 */
		asm volatile ("movq %[stack], %%rsp\n\t"
			      "call handle_stack_overflow\n\t"
			      "1: jmp 1b"
			      : ASM_CALL_CONSTRAINT
			      : "D" ("kernel stack overflow (page fault)"),
				"S" (regs), "d" (address),
				[stack] "rm" (stack));
		unreachable();
	}
#endif

	/*
	 * Buggy firmware could access regions which might page fault.  If
	 * this happens, EFI has a special OOPS path that will try to
	 * avoid hanging the system.
	 */
	if (IS_ENABLED(CONFIG_EFI))
		efi_crash_gracefully_on_page_fault(address);

	/* Only not-present faults should be handled by KFENCE. */
	if (!(error_code & X86_PF_PROT) && kfence_handle_page_fault(address))
		return;

oops:
	/*
	 * Oops. The kernel tried to access some bad page. We'll have to
	 * terminate things with extreme prejudice:
	 */
	flags = oops_begin();

	show_fault_oops(regs, error_code, address);

	if (task_stack_end_corrupted(current))
		printk(KERN_EMERG "Thread overran stack, or stack corrupted\n");

	sig = SIGKILL;
	if (__die("Oops", regs, error_code))
		sig = 0;

	/* Executive summary in case the body of the oops scrolled away */
	printk(KERN_DEFAULT "CR2: %016lx\n", address);

	oops_end(flags, regs, sig);
}


