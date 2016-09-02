/*
 * callbacks.h
 *
 *  Created on: Sep 1, 2016
 *      Author: Nikolay Khabarov
 */

#ifndef CALLBACKS_H_
#define CALLBACKS_H_

#include <fuse.h>

extern char *source_dir;

int getattr_cb(const char *, struct stat *);
int readlink_cb(const char *, char *, size_t);
int mknod_cb(const char *, mode_t, dev_t);
int mkdir_cb(const char *, mode_t);
int unlink_cb(const char *);
int rmdir_cb(const char *);
int symlink_cb(const char *, const char *);
int rename_cb(const char *, const char *);
int link_cb(const char *, const char *);
int chmod_cb(const char *, mode_t);
int chown_cb(const char *, uid_t, gid_t);
int truncate_cb(const char *, off_t);
int utime_cb(const char *, struct utimbuf *);
int open_cb(const char *, struct fuse_file_info *);
int read_cb(const char *, char *, size_t, off_t, struct fuse_file_info *);
int write_cb(const char *, const char *, size_t, off_t, struct fuse_file_info *);
int statfs_cb(const char *, struct statvfs *);
int flush_cb(const char *, struct fuse_file_info *);
int release_cb(const char *, struct fuse_file_info *);
int fsync_cb(const char *, int, struct fuse_file_info *);
int setxattr_cb(const char *, const char *, const char *, size_t, int);
int getxattr_cb(const char *, const char *, char *, size_t);
int listxattr_cb(const char *, char *, size_t);
int removexattr_cb(const char *, const char *);
int opendir_cb(const char *, struct fuse_file_info *);
int readdir_cb(const char *, void *, fuse_fill_dir_t ,off_t, struct fuse_file_info *);
int releasedir_cb(const char *, struct fuse_file_info *);
int access_cb(const char *, int);
void *init_cb(struct fuse_conn_info *);
void destroy_cb(void *);

#endif /* CALLBACKS_H_ */
