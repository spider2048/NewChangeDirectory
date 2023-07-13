# NewChangeDirectory
Program to change directory (from within `cmd.exe`) without using `cd`. [Windows]

# Usage
```bash
ChangeDir.exe <valid_folder_path>
```

Unlike cmd.exe's cd, no `/d` flag is needed to change folders across drives.

# Build commands
Build it as a normal C++ file. The requirement of additional runtime depends on the compiler you are using.
```
g++ .\ChangeDir.cpp -o ChangeDir.exe -O2 -static
strip -s ChangeDir.exe
```
