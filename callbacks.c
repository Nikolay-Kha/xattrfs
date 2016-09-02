#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/statvfs.h>

#include "callbacks.h"
#include "xattrdb.h"

#define ERROR 1
#define MAX_PATH 4096
#define XATTRDB_FILE_NAME ".xattrdb"
char *source_dir = NULL;

static void destination_path(char* dpath, const char *path) {
    snprintf(dpath, MAX_PATH, "%s/%s", source_dir, path);
}

static int check_res(const int res) {
    if(res == -1) {
        return -errno;
    }
    return 0;
}

int getattr_cb(const char *path, struct stat *st_data) {
    char dpath[MAX_PATH];
    destination_path(dpath, path);
    const int res = lstat(dpath, st_data);
    return check_res(res);
}

int readlink_cb(const char *path, char *buf, size_t size) {
    char dpath[MAX_PATH];
    destination_path(dpath, path);
    const int res = readlink(dpath, buf, size - 1);
    if(res != - 1) {
        buf[res] = 0;
    }
    return check_res(res);
}

int mknod_cb(const char *path, mode_t mode, dev_t dev) {
    char dpath[MAX_PATH];
    destination_path(dpath, path);
    const int res = mknod(dpath, mode, dev);
    return check_res(res);
}

int mkdir_cb(const char *path, mode_t mode) {
    char dpath[MAX_PATH];
    destination_path(dpath, path);
    const int res = mkdir(dpath, mode);
    return check_res(res);
}

int unlink_cb(const char *path) {
    char dpath[MAX_PATH];
    destination_path(dpath, path);
    const int res = unlink(dpath);
    if(res != - 1) {
        xattrdb_removepath(path);
    }
    return check_res(res);
}

int rmdir_cb(const char *path) {
    char dpath[MAX_PATH];
    destination_path(dpath, path);
    const int res = rmdir(dpath);
    // TODO cleanup database
    return check_res(res);
}

int symlink_cb(const char *target, const char *linkpath) {
    char dtarget[MAX_PATH];
    destination_path(dtarget, target);
    char dlinkpath[MAX_PATH];
    destination_path(dlinkpath, linkpath);
    const int res = symlink(dtarget, dlinkpath);
    return check_res(res);
}

int rename_cb(const char *oldpath, const char *newpath) {
    char doldpath[MAX_PATH];
    destination_path(doldpath, oldpath);
    char dnewpath[MAX_PATH];
    destination_path(dnewpath, newpath);
    const int res = rename(doldpath, dnewpath);
    if(res != -1) {
        xattrdb_renamepath(oldpath, newpath);
    }
    return check_res(res);
}

int link_cb(const char *oldpath, const char *newpath) {
    char doldtarget[MAX_PATH];
    destination_path(doldtarget, oldpath);
    char dnewpath[MAX_PATH];
    destination_path(dnewpath, newpath);
    const int res = link(doldtarget, dnewpath);
    return check_res(res);
}

int chmod_cb(const char *path, mode_t mode) {
    char dpath[MAX_PATH];
    destination_path(dpath, path);
    const int res = chmod(dpath, mode);
    return check_res(res);
}

int chown_cb(const char *path, uid_t owner, gid_t group) {
    char dpath[MAX_PATH];
    destination_path(dpath, path);
    const int res = chown(dpath, owner, group);
    return check_res(res);
}

int truncate_cb(const char *path, off_t size) {
    char dpath[MAX_PATH];
    destination_path(dpath, path);
    const int res = truncate(dpath, size);
    return check_res(res);
}

int utime_cb(const char *path, struct utimbuf *buf) {
    char dpath[MAX_PATH];
    destination_path(dpath, path);
    const int res = utime(dpath, buf);
    return check_res(res);
}

int open_cb(const char *path, struct fuse_file_info *fi) {
    char dpath[MAX_PATH];
    destination_path(dpath, path);
    const int res = open(dpath, fi->flags);
    if(res == -1) {
        return -errno;
    }
    fi->fh = res;
    return 0;
}

int read_cb(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    const int res = pread(fi->fh, buf, size, offset);
    if(res == -1) {
        return -errno;
    }
    return res;
}

int write_cb(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    const int res = pwrite(fi->fh, buf, size, offset);
    if(res == -1) {
        return -errno;
    }
    return res;
}

int statfs_cb(const char *path, struct statvfs *st_buf) {
    char dpath[MAX_PATH];
    destination_path(dpath, path);
    const int res = statvfs(dpath, st_buf);
    return check_res(res);
}

int flush_cb(const char *path, struct fuse_file_info *fi) {
    const int res = fsync(fi->fh);
    return check_res(res);
}

int release_cb(const char *path, struct fuse_file_info *fi) {
    const int res = close(fi->fh);
    return check_res(res);
}

int fsync_cb(const char *path, int isdatasync, struct fuse_file_info *fi) {
    const int res = fsync(fi->fh);
    return check_res(res);
}

int setxattr_cb(const char *path, const char *name, const char *value, size_t size, int flags) {
    return xattrdb_set(path, name, value, size) ? 0 : -ERROR;
}

int getxattr_cb(const char *path, const char *name, char *value, size_t size) {
    unsigned int c;
    bool res = xattrdb_get(path, name, &c, value, size);
    return res ? c : -ERROR;
}

int listxattr_cb(const char *path, char *list, size_t size) {
    unsigned int c;
    bool res = xattrdb_list(path, &c, list, size);
    return res ? c : -ERROR;
}

int removexattr_cb(const char *path, const char *name) {
    return xattrdb_removename(path, name) ? 0 : -ERROR;
}

int opendir_cb (const char *path, struct fuse_file_info *fi) {
    char dpath[MAX_PATH];
    destination_path(dpath, path);
    DIR *dp = opendir(dpath);
    if(dp == NULL) {
        return -errno;
    }
    fi->fh = (uint64_t)dp;
    return 0;
}

int readdir_cb(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi) {
    struct dirent *de;
    while((de = readdir((DIR *)fi->fh)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if(filler(buf, de->d_name, &st, 0)) {
            break;
        }
    }
    return 0;
}

int releasedir_cb(const char *path, struct fuse_file_info *fi) {
    const int res = closedir((DIR *)fi->fh);
    return check_res(res);
}

int access_cb(const char *path, int mode) {
    char dpath[MAX_PATH];
    destination_path(dpath, path);
    const int res = access(dpath, mode);
    return check_res(res);
}

void *init_cb(struct fuse_conn_info *conn) {
    char dpath[MAX_PATH];
    destination_path(dpath, XATTRDB_FILE_NAME);
    xattrdb_open(dpath);
    return NULL;
}

void destroy_cb(void *data) {
    xattrdb_close();
}

