import yaml
import subprocess
import os
import colorama


# Linux command
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


# File operate
def read_config():
    """read and return config in yaml format"""
    try:
        with open('config.yaml', 'r') as file:
            config = yaml.safe_load(file)
    except FileNotFoundError:
        print('file not found!')
    else:
        return config


def read_file(file_path):
    with open(file_path, 'r') as file:
        content = file.read()
    return content


class Repo:
    """Git repo class"""

    def __init__(self, repo: str, branch: str = None,
                 file: str = None, target: str = None) -> None:
        self.repo = repo
        self.branch = branch
        self.branches = None
        self.file = file
        self.file_history = None
        self.target = target

    @property
    def repo(self):
        return self._path

    @repo.setter
    def repo(self, path: str):
        self._path = path
        os.chdir(path)

    @property
    def branch(self):
        return self._branch

    @branch.setter
    def branch(self, branch):
        if branch is None:
            self._branch = get_output('git branch --show-current')
        else:
            self._branch = branch

    def switch(self, target: str):
        run_cmd(f'git switch {target}')
        self.branch = get_output('git branch --show-current')

    def checkout(self, target: str):
        run_cmd(f'git checkout {target}')
        self.branch = get_output('git branch --show-current')

    @property
    def file(self):
        return self._file

    @file.setter
    def file(self, file_path):
        self._file = file_path

    @property
    def file_history(self):
        return self._file_history

    @file_history.setter
    def file_history(self, commits: list):
        self._file_history = commits

    def get_file_history(self, file: str = None) -> list:
        if file is not None:
            commits = get_output(f'''git log --pretty=format:'%H' {file}''')
        else:
            commits = get_output(
                f'''git log --pretty=format:'%H' {self.file}''')
        self.file_history = commits.splitlines()

    def track(self):
        """
        Track a string in specific file.
        """
        # Iterate every commit in history by order
        for commit in self.file_history[::-1]:
            # 'git checkout' to specific commit for searching
            self.checkout(commit)
            # Check if taret exist
            if self.target in read_file(self.file):
                colorama.init()
                print()
                print(colorama.Fore.YELLOW + '*'*6 + 'got it!' + '*'*6)
                print(colorama.Fore.GREEN + commit)
                return


def main():
    # Init
    config = read_config()
    repo = Repo(repo=config['repo'],
                file=config['repo']+'/'+config['file'],
                target=config['target'])

    # Get all commit history of target file
    repo.get_file_history()

    # Track string
    repo.track()


if __name__ == '__main__':
    main()
