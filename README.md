# bulk-rename-plus
Tiny but powerful C++ program to rename recursively paths into upper/lower/reverse case from the terminal.

Useful for improving the performance of installed wine windows apps in a case sensitive linux file system and/or for improving compatibility with some wine windows game mods.

## How to use:
Case conversion modes include `lower` for converting everything to lower case `upper` for converting everything to upper case and `reverse` for reversing existing case status.

e.g. lower: Test => test, upper: Test=> TEST, reverse: Test => tEST.

usage:

bulk_rename [path1] [path2]... -c lower.

if you wish to also rename the parent directories of input paths:

bulk_rename [path1] [path2]... -cp lower.


if you wish to also enable verbose mode:

bulk_rename [path1] [path2]... -c lower -v.
or
bulk_rename [path1] [path2]... -v -c lower.

`--verbose` instead of `-v` can also be used.

for help:
bulk_rename++ -h or bulk_rename++ --help.

For improving the performance and compatibility of windows apps in a case sensitive linux file system with wine, use `lower` case mode exclusively.

### Ways to install:

1. Download binary from latest release.
2. Download from source and compile with `make`.
3. If on arch, or on an arch based distro `yay -S bulk-rename-plus`




