# bulk-rename-plus
Tiny but powerful C++ program to rename files into upper/lower/reverse case from the terminal.

Useful for improving performance for wine windows apps in a case sensitive linux system and for improving compatibility with some windows wine game mods.

# How to use:
modes include `lower` for converting everything to lower case `upper` for converting everything to upper case and `reverse` for reversing existing case status.

e.g. lower: Test => test, upper: Test=> TEST, reverse: Test => tEST.

usage:

bulk_rename [path1] [path2]... -c lower.

if you wish to also rename the parent directories of input paths:

bulk_rename [path1] [path2]... -cp lower.


if you wish to also enable verbose mode:

bulk_rename [path1] [path2]... -c lower -v.
or
bulk_rename [path1] [path2]... -v -c lower.

`--verbose` instead of `-v` can also be used

for help:
bulk_rename++ -h or bulk_rename++ --help




