import subprocess


def run_cmd(cmd: str):
    """
    Executing a command.
    """
    subprocess.run(cmd, shell=True)


def get_output(cmd: str):
    """
    Return the result of executing a command.
    """
    output = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    return output.stdout[:-1]


def main():
    with open('kfence.patches', 'r') as file:
        patches = file.readlines()

    for patch in patches:
        run_cmd(f'git am kfence-patches/{patch}')


if __name__ == '__main__':
    main()
