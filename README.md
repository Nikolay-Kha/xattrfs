# xattrfs
The purpose of this project is to emulate extended file attributes (aka xattr) for filesystems which don't support it. Project uses FUSE to create a bind of directory in some different mountpoint. New mountpoint allows to work with xattr regardless of capability of directory file system. Attributes will be stored in SQLite database in file `.xattrbd`.

# Building
```bash
sudo apt-get install libfuse-dev libsqlite3-dev
make
```

# Usage
```bash
xattrfs source_dir mountpoint [-o FUSE arguments]
```
See list of FUSE arguments in general mount options section of `man mount.fuse`. Example:  
```bash
xattrfs /src /dest -o allow_other
```

# Demo
This demo shows output on attemp to set up xattr on file system without xattr support - aufs. Then mount to another directory with xattrfs and it's possible to set up xattr. Last lines show where xattr actually stored. 
```bash
 ~ $ cd /tmp
 /tmp $ mkdir aufs-doesnt-support-xattr
 /tmp $ sudo mount -t aufs -o br=/dev/shm/fordemo none /tmp/aufs-doesnt-support-xattr
[sudo] password for nikolay: 
 /tmp $ touch aufs-doesnt-support-xattr/testfile
 /tmp $ setfattr -n "user.demo.xattr" -v "myvalue" aufs-doesnt-support-xattr/testfile 
setfattr: aufs-doesnt-support-xattr/testfile: Operation not supported
 /tmp $ 
 /tmp $ mkdir xattrfs
 /tmp $ ~/xattrfs/xattrfs /tmp/aufs-doesnt-support-xattr /tmp/xattrfs
 /tmp $ setfattr -n "user.demo.xattr" -v "this value will be stored in sqlite" xattrfs/testfile 
 /tmp $ getfattr -d -m- xattrfs/testfile 
# file: xattrfs/testfile
user.demo.xattr="this value will be stored in sqlite"

 /tmp $ sqlite3 /tmp/xattrfs/.xattrdb "SELECT * from '/testfile';"
user.demo.xattr|this value will be stored in sqlite
 /tmp $
```

# License
The MIT License. See LICENSE file.

# Authors
Nikolay Khabarov
