**xattrfs**
The purpose of this project is to emulate extended file attributes (aka xattr) for filesystems which don't support it. Project uses FUSE to create a bind of directory in some different mountpoint. New mountpoint allows to work with xattr regardless of capability of directory file system.

**Building**
```bash
sudo apt-get install libfuse-dev
make
```

**Usage**
```bash
xattrfs source_dir mountpoint [-o FUSE arguments]
```
See list of FUSE arguments in general mount options section of `man mount.fuse`. Example:  
```bash
xattrfs /src /dest -o allow_other
```
