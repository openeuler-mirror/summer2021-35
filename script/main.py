import subprocess


def get_output(cmd: str):
    """
    Return the result of executing a command.
    """
    output = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    return output.stdout[:-1]


def main():
    cmd = 'git log --pretty=format:"%H" ./mm/kfence'
    commits = get_output(cmd).splitlines()
    # merge gene.py and apply.py here!


if __name__ == '__main__':
    main()
