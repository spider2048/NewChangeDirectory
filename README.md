# NewChangeDirectory
Program to change directory of the parent process.

Read about the project [here](https://sp1d3r.vercel.app/posts/cmd-cd/).

# Usage
```bash
ChangeDir.exe <Valid Folder Path>
```

Unlike `cmd.exe`'s cd, no `/d` flag is needed to change folders across drives.

# Building
This project now uses the (meson)[meson.build] build system.

To start off, install `meson`:
```py
pip install meson
```
More details [here](https://mesonbuild.com/Quick-guide.html).

Next, clone the repository and build the project.
```bash
git clone https://github.com/spider2048/NewChangeDirectory
meson setup build --buildtype release # or debugoptimized
ninja -C build
```
The output file `ChangeDir.exe` will be in the `build` folder.