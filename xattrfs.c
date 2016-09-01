#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <fuse.h>

#include "callbacks.h"

enum {
    KEY_HELP
};

static struct fuse_opt xattrfs_opts[] = {
    FUSE_OPT_KEY("-h",          KEY_HELP),
    FUSE_OPT_KEY("--help",      KEY_HELP),
    FUSE_OPT_END
};

struct fuse_operations callback_operations = {
    .getattr = getattr_cb,
    .readlink = readlink_cb,
    // .getdir is depricated,
    .mknod = mknod_cb,
    .mkdir = mkdir_cb,
    .unlink = unlink_cb,
    .rmdir = rmdir_cb,
    .symlink = symlink_cb,
    .rename = rename_cb,
    .link = link_cb,
    .chmod = chmod_cb,
    .chown = chown_cb,
    .truncate = truncate_cb,
    .utime = utime_cb,
    .open = open_cb,
    .read = read_cb,
    .write = write_cb,
    .statfs = statfs_cb,
    .flush = flush_cb,
    .release = release_cb,
    .fsync = fsync_cb,
    .setxattr = setxattr_cb,
    .getxattr = getxattr_cb,
    .listxattr = listxattr_cb,
    .removexattr = removexattr_cb,
    .opendir = opendir_cb,
    .readdir = readdir_cb,
    .releasedir = releasedir_cb,
    .access = access_cb
};

static int xattrfs_parse_opt(void *data, const char *arg, int key,
        struct fuse_args *outargs)
{
    switch(key) {
        case FUSE_OPT_KEY_OPT:
            return 1;
        case FUSE_OPT_KEY_NONOPT:
            if(source_dir == NULL) {
                source_dir = strdup(arg);
                return 0;
            } else {
                return 1;
            }
        case KEY_HELP:
            fprintf(stdout,
                    "Utility for adding extended file attributes(xattr) support for any file system\n"
                    "without this feature. Utility bind any directory to a new mountpoint. xattr data\n"
                    "stores in hidden file\n"
                    "Usage:\nxattrfs source_dir mountpoint [-o FUSE arguments]\n"
                    "See list of FUSE arguments in general mount options section of 'man mount.fuse'\n"
                    "Example:\n"
                    "xattrfs /src /dest -o allow_other\n");
            exit(0);
        default:
            fprintf(stderr, "Unknown argument %s\n", outargs->argv[0]);
            exit(1);
    }
    return 1;
}

int main(int argc, char *argv[]) {
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    int res;

    res = fuse_opt_parse(&args, &source_dir, xattrfs_opts, xattrfs_parse_opt);
    if(res != 0) {
        fprintf(stderr, "Invalid arguments\n");
        exit(1);
    }
    if(source_dir == NULL) {
        fprintf(stderr, "Missing source_dir\n");
        exit(1);
    }
    if(source_dir[strlen(source_dir) - 1] == '/') {
        source_dir[strlen(source_dir) - 1] = 0;
    }
    fuse_main(args.argc, args.argv, &callback_operations, NULL);
    return 0;
}
