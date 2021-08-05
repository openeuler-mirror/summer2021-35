import subprocess


def get_output(cmd: str):
    """
    Return the result of executing a command.
    """
    output = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    return output.stdout[:-1]


def main():
    result = get_output('git log --oneline ./mm/kfence')
    print(result)


if __name__ == '__main__':
    main()
