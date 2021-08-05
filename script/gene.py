import subprocess


def get_output(cmd: str) -> str:
    """
    Return the result of executing a command.
    """
    output = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    return output.stdout[:-1]


def digit_len(num: int) -> int:
    return len(str(num))


def main():
    cmd = 'git log --pretty=format:"%H" ./mm/kfence'
    commits = get_output(cmd).splitlines()[::-1]

    prefix_len = 4
    patches = []
    for serial, commit in enumerate(commits[::-1]):
        patch = get_output(f'git format-patch -1 {commit}')
        zero_width = prefix_len - digit_len(serial)
        prefix = '0'*zero_width+str(serial)
        commit_title = '-'.join(patch.split('-')[1:])
        ordered_patch = f'{prefix}-{commit_title}'
        get_output(f'mv {patch} {ordered_patch}')


if __name__ == '__main__':
    main()
