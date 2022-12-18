/************************************************
 * Author: Malik Mohamedali
 * Assignment Name: Program 6
 * Course: CSI 4337
 ************************************************/

#define FUSE_USE_VERSION 26
#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>

#include "HooFSRPCClient.h"

#define NAME "HooFS Fuse Temp Client"
#define VERSION "1.1"

static char *server;
static char *port;

rpcClient *rpcclient;

/*
 * Get file attributes
 */
static int hoofs_getattr(const char *path, struct stat *stbuf) {
    rpcclient->rpc_getAttr(path, stbuf);

    return 0;
}

/*
 * set file attributes
 */
static int hoofs_setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
    rpcclient->rpc_setxattr(path, name, value, size, flags);

    return 0;
}

/*
 * change file mode/perms
 */
static int hoofs_chmod(const char *path, mode_t mode) {
    rpcclient->rpc_chmod(path, mode);

    return 0;
}

/*
 * change file owner
 */
static int hoofs_chown(const char *path, uid_t uid, gid_t gid) {
    rpcclient->rpc_chown(path, uid, gid);

    return 0;
}

/*
 * get utime obj
 */
static int hoofs_utime(const char *path, struct utimbuf *ubuf) {
    rpcclient->rpc_utime(path, ubuf);

    return 0;
}

/*
 * truncate server file
 */
static int hoofs_truncate(const char *path, off_t newsize) {
    rpcclient->rpc_truncate(path, newsize);

    return 0;
}

/*
 * read server directory listing from a given path
 */
static int hoofs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    rpcclient->rpc_readdir(path, buf);

    return 0;
}

/*
 * open file on server
 */
static int hoofs_open(const char *path, struct fuse_file_info *fi) {
    fi->fh = (int) rpcclient->rpc_open(path, fi->flags);

    return 0;
}

/*
 * remove file from server
 */
static int hoofs_release(const char *path, struct fuse_file_info *fi) {
    rpcclient->rpc_release(fi->fh);

    return 0;
}

/*
 * create file on server
 */
static int hoofs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    rpcclient->rpc_create(path, mode);

    return 0;
}

/*
 * unlink file on server
 */
static int hoofs_unlink(const char *path) {
    rpcclient->rpc_unlink(path);

    return 0;
}

/*
 * remove directory from server
 */
static int hoofs_rmdir(const char *path) {
    return hoofs_unlink(path);
}

static int hoofs_rename(const char *path, const char *newpath) {
    rpcclient->rpc_rename(path, newpath);

    return 0;
}

/*
 * read file from server
 */
static int hoofs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    rpcclient->rpc_read(buf, fi->fh, size, offset);

    return 0;
}

/*
 * write to file on server
 */
static int hoofs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    int ret = (int) rpcclient->rpc_write(fi->fh, buf, size, offset);

    return ret;
}

static struct fuse_operations hoofs_oper;

static int myfs_opt_proc(void *data, const char *arg, int key, struct fuse_args *outargs) {
if (key == FUSE_OPT_KEY_NONOPT && (server == NULL || port == NULL)) {
    if (server == NULL) {
        server = strdup(arg);
    }
    else if (port == NULL) {
        port = strdup(arg);
    }
    return 0;
}
return 1;
}

void printargs(int argc, char** argv) {
     int i;
    printf("argc: %d\n", argc);
    for (i = 0; i < argc; i++) {
        printf("%s ", *(argv + i));
    }
    printf("\n");
 }

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <server name/address> <port> <fuse params>...\n", argv[0]);
        exit(1);

    }
    hoofs_oper.getattr = hoofs_getattr;
    hoofs_oper.setxattr = hoofs_setxattr;
    hoofs_oper.chmod = hoofs_chmod;
    hoofs_oper.chown = hoofs_chown;
    hoofs_oper.utime = hoofs_utime;
    hoofs_oper.truncate = hoofs_truncate;
    hoofs_oper.readdir = hoofs_readdir;
    hoofs_oper.rename = hoofs_rename;
    hoofs_oper.open = hoofs_open;
    hoofs_oper.release = hoofs_release;
    hoofs_oper.create = hoofs_create;
    hoofs_oper.unlink = hoofs_unlink;
    hoofs_oper.rmdir = hoofs_rmdir;
    hoofs_oper.read = hoofs_read;
    hoofs_oper.write = hoofs_write;

    printf("%s", "Starting Fuse Driver...\n");

    // printf("endpoint: %s\n", endpoint); */
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    fuse_opt_parse(&args, NULL, NULL, myfs_opt_proc);
    fuse_opt_parse(&args, NULL, NULL, myfs_opt_proc);
    printf("new args: "); printargs(args.argc, args.argv);

    rpcclient = new rpcClient(server, port);

    printf("Beginning Fuse Operations...\n");

    return fuse_main(args.argc, args.argv, &hoofs_oper, NULL);
}
