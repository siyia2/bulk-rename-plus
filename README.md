# bulk-rename-plus
Tiny and Powerful C++ CMD tool to rename recursively paths on Linux, useful for batch renaming.

Multithreaded at top level with each input path being assigned to a separate thread and at mid-level where each subdirectory is processed by a separate trhead.

Default and recommended installation path is `~/.local/bin/bulk_rename++`, no need to run it as `ROOT` unless of course you know what you are doing.

## How to use:

Usage: bulk_rename++ [OPTIONS] [MODE] [PATHS]

Options: 
- `-d` stands for recursion depth level (optional).
- `-fi` stands for exclusive file renaming (optional).
- `-fo` stands for exclusive folder renaming (optional).
- `-c` option stands for case set.
- `-ce` option stands for case set for file extensions.
- `-cp` option stands for case set including the lowest parent dir(s).
- `-v` or `--verbose` option stands for enabling verbose output (optional).
- `--version` print version.

### Current suported case modes: 

#### Regular CASE Modes:
- `title`      Convert names to Title Case (e.g., test => Test)
- `upper`      Convert names to UPPERCASE (e.g., Test => TEST)
- `lower`      Convert names to lowercase (e.g., Test => test)
- `reverse`    Reverse current case in names (e.g., Test => tEST)
#### Special CASE Modes:
- `snake`      Convert spaces to underscores in names (e.g., Te st => Te_st)
- `rsnake`     Convert underscores to spaces in names (e.g., Te_st => Te st)
- `kebab`      Convert spaces to hyphens in names (e.g., Te st => Te-st)
- `rkebab`     Convert hyphens to spaces in names (e.g., Te-st => Te st)
- `camel`     Convert names to camelCase (e.g., Te st => teSt)
- `rcamel`     Reverse camelCase in names (e.g., TeSt => te st)
- `pascal`     Convert names to pascalCase (e.g., Te st => TeSt)
- `rpascal`    Reverse pascalCase in names (e.g., TeSt => te st)
- `sentence`    Convert names to sentenceCase (e.g., Te st => Te St)
#### Extension CASE Modes:
- `bak`        Add .bak on file extension names (e.g., Test.txt => Test.txt.bak)
- `rbak`       Remove .bak from file extension names (e.g., Test.txt.bak => Test.txt)
- `noext`      Remove file extensions (e.g., Test.txt => Test)
#### Numerical CASE Modes:
- `nsequence`  Apply sequential numbering in names (e.g., Test => 001_Test)
- `rnsequence` Remove sequential numbering from names (e.g., 001_Test => Test)
- `date`       Append current date if no date preexists in names (e.g., Test => Test_20240215)
-	`rdate`      Remove date from names (e.g., Test_20240215 => Test)
- `rnumeric`   Remove numeric characters from names (e.g., 1Te0st2 => Test)
#### Custom CASE Modes:
- `rbra`       Remove [ ] { } ( ) from names (e.g., [{Test}] => Test)
- `roperand`   Remove - + > < = * from names (e.g., =T-e+s<t> => Test)
- `rspecial`   Remove special characters from names (e.g., Tes\t!@#$%^|&~`'\"\";? => Test)
- `swap`       Swap upper-lower case for names (e.g., Test => TeSt)
- `swap`       Swap upper-lower case for names (e.g., Test => tEsT)

for more help and usage examples:

bulk_rename++ -h or bulk_rename++ --help.
##### Tips:
To improve the performance and compatibility of windows apps in a case sensitive linux file system with wine, use `lower` case mode exclusively.

### Ways to install:

1. Download binary from latest release.
2. Download from source and compile with `make`.
3. If on arch, or on an arch based distro `yay -S bulk-rename-plus`




