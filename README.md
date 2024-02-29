# bulk-rename-plus
Tiny and Powerful C++ CMD tool to rename recursively paths on Linux, useful for batch renaming.

Multithreaded at the top level with each input path being assigned to a separate thread and at the mid-level where each subdirectory is processed by a separate thread.

Default and recommended installation path is `~/.local/bin/bulk_rename++`, no need to run as `ROOT` unless of course you know what you are doing.

## How to use:

Usage: bulk_rename++ [OPTIONS] [MODE] [PATHS]

Options: 
- `-d` stands for recursion depth level (optional).
- `-fi` stands for exclusive file renaming (optional).
- `-fo` stands for exclusive folder renaming (optional).
- `-sym` stands for include also the renaming of symlinks, can be combined with `-fi` or `-fo` (optional).
- `-c` option stands for case set.
- `-ce` option stands for case set for file extensions.
- `-cp` option stands for case set including the lowest parent dir(s).
- `-v` or `--verbose` option stands for enabling verbose output (optional).
- `--version` print version.

### Current suported case modes: 

#### Regular CASE Modes:
- `title`      Convert names to titleCase (e.g., test => Test)
- `upper`      Convert names to upperCase (e.g., Test => TEST)
- `lower`      Convert names to lowerCase (e.g., Test => test)
- `reverse`    Reverse current case in names (e.g., Test => tEST)
#### Special CASE Modes:
- `snake`      Convert names to snakeCase (e.g., Te st => Te_st)
- `rsnake`     Reverse snakeCase in names (e.g., Te_st => Te st)
- `kebab`      Convert names to kebabCase (e.g., Te st => Te-st)
- `rkebab`     Reverse kebabCase in names (e.g., Te-st => Te st)
- `camel`      Convert names to camelCase (e.g., Te st => teSt)
- `rcamel`     Reverse camelCase in names (e.g., TeSt => te st)
- `pascal`     Convert names to pascalCase (e.g., Te st => TeSt)
- `rpascal`    Reverse pascalCase in names (e.g., TeSt => Te St)
- `sentence`   Convert names to sentenceCase (e.g., Te st => Te St)
#### Extension CASE Modes:
- `bak`        Add .bak on file extension names (e.g., Test.txt => Test.txt.bak)
- `rbak`       Remove .bak from file extension names (e.g., Test.txt.bak => Test.txt)
- `noext`      Remove file extensions (e.g., Test.txt => Test)
#### Numerical CASE Modes:
- `nsequence`  Append sequential numbering to names if no numbering pre-exists (e.g., Test => 001_Test)
- `rnsequence` Remove sequential numbering from names (e.g., 001_Test => Test)
- `date`       Append current date to names if no date pre-exists (e.g., Test => Test_20240215)
- `rdate`      Remove date from names (e.g., Test_20240215 => Test)
- `rnumeric`   Remove numeric characters from names (e.g., 1Te0st2 => Test)
#### Custom CASE Modes:
- `rbra`       Remove [ ] { } ( ) from names (e.g., [{Test}] => Test)
- `roperand`   Remove - + > < = * from names (e.g., =T-e+s<t> => Test)
- `rspecial`   Remove special characters from names (e.g., Tes\t!@#$%^|&~`'\"\";? => Test)
- `swap`       Swap upper-lower case for names (e.g., Test => TeSt)
- `swapr`      Swap lower-upper case for names (e.g., Test => tEsT)

for more help and usage examples:

bulk_rename++ -h or bulk_rename++ --help.
##### Tips:
To improve the performance and compatibility of windows apps on a case sensitive linux file system with wine, use `lower` case mode exclusively.
More info here: https://wiki.winehq.org/Case_Insensitive_Filenames

### Ways to install:

1. Download binary from latest release.
2. Download from source and compile with `make`.
3. If on arch, or on an arch based distro `yay -S bulk-rename-plus`




