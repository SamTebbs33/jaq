# Contributing

## Ideas and bug reports
If you'd like to contribute with an idea or bug report, please search the [list of issues](https://github.com/SamTebbs33/jaq/issues) here on GitHub to make sure it hasn't been submitted already. If it has, then feel free to comment on the issue with your own ideas and reports. If it hasn't then please start creating an issue and follow the guidelines below.

1. Give it a descriptive title so that it's easy to know what your issue is about and identify it among others. An example of a good title is "Build fails with 'src/arch/x86_64/Makefile: No such file or directory'", and an example of a bad title is "Can't build code".
2. Make sure you've done as much troubleshooting and investigation as you can. Doing as much investigation as possible means you'll be able to provide more detailed information (you might even find a solution during this process!), which means it can be addressed more effectively.
3. If your reporting a bug or similar problem, then please provide as much background information as you can, such as extracts from log files, architecture info (what kind of machine you're building/running on and what architecture you're building/running for).
4. Leave the issue unassigned, unlabeled, not attached to a milestone or a project. This helps filter out new issues from those already addressed. Labels and such will be assigned by someone with the authority to do so.

Submit it! Someone from the community will give you feedback on your bug report/suggestion soon. If it's a feature suggestion then you'd be very welcome to implement it yourself after some discussion has taken place.

## Code contributions and other changes

### Issue
If you'd like to fix a bug, implement a feature etc., then make sure an issue has been opened for it (see above). All changes to the project require an issue for tracking and discussion purposes. If the issue is unassigned then comment on it asking if you can take it and wait for an answer.

### Forking and branching
The next step is to fork the repo, to which all of your commits and pushes should be made.

The last step before you can get to the fun part is to create a branch off the `dev` branch in your forked repo. The name of this branch should be as follows:
* Start with an identifier: "f" if it's a feature ("feature" label), "b" if it's a bug fix ("bug" label) or "q" if it's a code quality change ("code quality" label).
* Followed by a slash and a very brief description of the change with hyphen-separated words.

E.g.: "f/round-robin-scheduler", "b/missing-build-command" or "q/driver-interface-cleanup".

### The fun part
Start work! Do your best to follow the code style guidelines detailed below so we can keep the project in a consistent style. It is also a good idea to make your git commits well organised and atomic, see [this great article](https://www.freshconsulting.com/atomic-commits/) on what that means.

### Pull request
Once you've finished your work and tested it, make sure you rebase on the latest changes to `dev` in the main repo, then push your branch to your fork then open up a pull request to the main repo. Leave the pull request title as the branch name but add "closes #x" if your pull request is addressing ticket _x_, and enter a description of what your changes do and any decisions you made while working on it.

You should assign the people responsible for reviewing your pull request. These details can be found in the [CODEOWNERS](CODEOWNERS) file. Don't be scared to mis-assign someone, they'll notice it and let you know for future reference.

Don't be worried if your changes fail some of the status checks or get a lot of requested changes, that's the reason why we have pull request reviews!

## Code style
The project follows the [BSD Kernel Normal Form](https://en.wikipedia.org/wiki/Indentation_style#Variant:_BSD_KNF) style but with unlimited line length and braces on the same line as function signatures. The most important thing is to be consistent with the existing style in whatever file you're editing.
