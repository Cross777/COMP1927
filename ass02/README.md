# Fury-of-Dracula
### Installing Git
To set it up, download the appropriate Git command-line tool:

1. [Mac OS X](http://mac.github.com/)
2. [Windows](https://git-scm.com/)

For Linux, if you do not have a `git` command, use your package manager to get the package (usually called git).

### Downloading the files
1. Navigate to your desired folder (where files will be downloaded to).
2. Run the command `git init`. This creates the local git repository.
3. Run the command `git remote add <remote_name> git@github.com:nickrobson/Fury-of-Dracula.git`. This adds the remote (used later).
4. Run the command `git pull <remote_name> master`. This fetches the code from the repository.

### Seeing what you've changed
###### So you know what to say in your commit message
1. Navigate to the folder where the git repo is.
2. Run the command `git diff [file_name]`. The optional parameter is the file to be diff'd.

### Committing changes
1. Navigate to the folder where the git repo is.
2. Run the command `git add <file_name...>`, where the parameters are file names.
3. Run the command `git commit -m "Commit message goes here"`, double quotes needed.
4. Run the command `git push <remote_name> master`. This sends the code to the [online repo](https://github.com/nickrobson/Fury-of-Dracula/).
