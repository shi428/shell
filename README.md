# shell

A Unix Shell written in C++ that combines the behaviors found in modern shells (bash, zsh, csh, etc.) for better workflow and productivity.

## How to use this repository

1. Clone this repository by typing in the command ```git clone https://github.com/shi428/shell.git```
2. Run ```make shell```

## Features

* Execution of basic commands
* I/O redirection
* Pipes
* Background processes
* Ignoring Ctrl-c
* Zombie cleanup
* Quote support
* Escape support
* Built-in functions (can be listed by running the help command)
* Default configuration file ($HOME/.shellrc)
* Subshells
* Environment variable expansion
* Tilde expansion
* Wild card expansion
* Line editing
* History
* Tab completion
* Variable color prompt (set using the PROMPT environment variable)

## How to make this your default shell

1. In a terminal window type sudo ```echo $PWD/shell >> /etc/shells```. This will add the shell to your list of shells.
2. Run ```chsh -s $PWD/shell```.
3. Run su $USER. Your default shell should now switch.

Node: Replace PWD if you moved the shell location to a different directory.
