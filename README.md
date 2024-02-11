# bulk-rename-plus
Tiny but powerful C++ cmd tool to rename recursively paths on Linux.

Useful for batch renaming.

Default and recommended installation path is `~/.local/bin/bulk_rename++`, no need to run it as `ROOT` unless of course you know what you are doing.

## How to use:

Options: `-c` option stands for case set, `-ce` option stands for case set for file extensions, `-cp` option stands for case ste including parent dir(s) and `-v` or `--verbose` option stands for verbose.

Current suported case modes: 

*`lower` for lower case, `upper` for upper case, 
*`fupper` for converting every first character to upper case and the rest to lower, 
*`reverse` for reversing existing case status, 
*`rspace` and `runderscore` replace spaces with underscores and vice versa.

e.g. with -c lower: Test => test, upper: Test=> TEST, reverse: Test => tEST fupper: test => Test.

e.g. with -ce lower: Test.txt => Test.txt upper: Test.txt => Test.TXT reverse: Test => Test.TXT fupper: test.txt => test.Txt

for help:

bulk_rename++ -h or bulk_rename++ --help.

For improving the performance and compatibility of windows apps in a case sensitive linux file system with wine, use `lower` case mode exclusively.

### Ways to install:

1. Download binary from latest release.
2. Download from source and compile with `make`.
3. If on arch, or on an arch based distro `yay -S bulk-rename-plus`




