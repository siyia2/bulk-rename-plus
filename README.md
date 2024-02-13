# bulk-rename-plus
Tiny and powerful C++ CMD tool to rename recursively paths on Linux, useful for batch renaming.

Default and recommended installation path is `~/.local/bin/bulk_rename++`, no need to run it as `ROOT` unless of course you know what you are doing.

## How to use:

Options: 
- `-d` stands for recursion depth level (optional).
- `-fi` stands for exclusive file renaming (optional).
- `-fo` stands for exclusive folder renaming (optional).
- `-c` option stands for case set.
- `-ce` option stands for case set for file extensions.
- `-cp` option stands for case set including parent dir(s).
- `-v` or `--verbose` option stands for enabling verbose output (optional).

### Current suported case modes: 

#### Common Regular CASE Modes:
- title      Convert names to Title Case (e.g., test => Test)
- upper      Convert names to UPPERCASE (e.g., Test => TEST)
- lower      Convert names to lowercase (e.g., Test => test)
- reverse    Reverse current case in names (e.g., Test => tEST)
#### Common Special CASE Modes:
- snake      Convert spaces to underscores in names (e.g., Te st => Te_st)
- rsnake     Convert underscores to spaces in names (e.g., Te_st => Te st)
- kebab      Convert spaces to hyphens in names (e.g., Te st => Te-st)
- rkebab     Convert hyphens to spaces in names (e.g., Te-st => Te st)
- camel      Convert names to camelCase (e.g., Te st => TeSt)
- rcamel     Reverse camelCase in names (e.g., TeSt => Te st)
#### Extension exclusive CASE Modes:
- bak        Add .bak at file extension names (e.g., Test.txt => Test.txt.bak)
- rbak       Remove .bak from file extension names (e.g., Test.txt.bak => Test.txt)
- noext      Remove extensions (e.g., Test.txt => Test)\n"
#### Numbering CASE Modes:
- sequence   Apply sequential numbering to files only (e.g. Test => 001_Test)
- rsequence  Remove sequential numbering of files (e.g. 001_Test => Test)
- rnumeric   Remove numeric characters from names (e.g., 1Te0st2 => Test)
#### Custom CASE Modes:
- rbra       Remove [ ] { } ( ) from names (e.g., [{Test}] => Test)
- roperand   Remove - + > < = * from names (e.g., =T-e+s<t> => Test)
- rspecial   Remove special characters from names (e.g., Tes\t!@#$%^|&~`'\"\";? => Test)

for more help and usage examples:

bulk_rename++ -h or bulk_rename++ --help.

For improving the performance and compatibility of windows apps in a case sensitive linux file system with wine, use `lower` case mode exclusively.

### Ways to install:

1. Download binary from latest release.
2. Download from source and compile with `make`.
3. If on arch, or on an arch based distro `yay -S bulk-rename-plus`




