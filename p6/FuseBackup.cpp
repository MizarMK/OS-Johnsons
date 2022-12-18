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

#include "hoofs_interface.h"
// #include <xmlrpc-c/base.h>
// #include <xmlrpc-c/client.h>
#include "include/xmlrpc-c/config.h"  /* information about this build environment */

#define NAME "HooFS Fuse Temp Client"
#define VERSION "1.1"

static char *fileSystemRoot = "/tmp/user_files";
static char *server;
static char *port;

static void dieIfFaultOccurred (xmlrpc_env * const envP) {
    if (envP->fault_occurred) {
        fprintf(stderr, "ERROR: %s (%d)\n",
                envP->fault_string, envP->fault_code);
        exit(1);
    }
}

/*
 * Get file attributes
 */
static int hoofs_getattr(const char *path, struct stat *stbuf) {
    CHandle c = create_client(server, port);
    return rpc_getAttr(c, path, stbuf);
}

/*
 * set file attributes
 */
static int hoofs_setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
    CHandle c = create_client(server, port);
    return rpc_setxattr(c, path, name, value, size, flags);
}

/*
 * change file mode/perms
 */
static int hoofs_chmod(const char *path, mode_t mode) {
    const char * const methodName = "rpc.chmod";

    xmlrpc_env env;
    xmlrpc_value * resultP;
    xmlrpc_env_init(&env);
    xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);

    dieIfFaultOccurred(&env);

    resultP = xmlrpc_client_call(&env, endpoint, methodName, "(si)", path,
                                 (xmlrpc_int32)mode); // run temp client for service
    dieIfFaultOccurred(&env);

    /* Dispose of unused result value. */
    xmlrpc_DECREF(resultP);

    /* Clean up our error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Shutdown our XML-RPC client library. */
    xmlrpc_client_cleanup();
    return 0;
}

/*
 * change file owner
 */
static int hoofs_chown(const char *path, uid_t uid, gid_t gid) {
    const char * const methodName = "rpc.chown";

    xmlrpc_env env;
    xmlrpc_value * resultP;
    xmlrpc_env_init(&env);
    xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);

    dieIfFaultOccurred(&env);

    resultP = xmlrpc_client_call(&env, endpoint, methodName, "(sii)", path,
                                 (xmlrpc_int32)uid,
                                 (xmlrpc_int32)gid); // run temp client for service
    dieIfFaultOccurred(&env);

    /* Dispose of unused result value. */
    xmlrpc_DECREF(resultP);

    /* Clean up our error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Shutdown our XML-RPC client library. */
    xmlrpc_client_cleanup();
    return 0;
}

/*
 * get utime obj
 */
static int hoofs_utime(const char *path, struct utimbuf *ubuf) {
    const char * const methodName = "rpc.utime";

    xmlrpc_env env;
    xmlrpc_value * resultP;
    xmlrpc_env_init(&env);
    xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);

    dieIfFaultOccurred(&env);

    resultP = xmlrpc_client_call(&env, endpoint, methodName, "(sS)", path,
                                 ubuf); // run temp client for service
    dieIfFaultOccurred(&env);
    ubuf = (struct utimbuf *) resultP;

    /* Dispose of unused result value. */
    xmlrpc_DECREF(resultP);

    /* Clean up our error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Shutdown our XML-RPC client library. */
    xmlrpc_client_cleanup();
    return 0;
}

/*
 * truncate server file
 */
static int hoofs_truncate(const char *path, off_t newsize) {
    const char * const methodName = "rpc.truncate";

    xmlrpc_env env;
    xmlrpc_value * resultP;
    xmlrpc_env_init(&env);
    xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);

    dieIfFaultOccurred(&env);

    resultP = xmlrpc_client_call(&env, endpoint, methodName, "(si)", path,
                                 (xmlrpc_int32)newsize); // run temp client for service
    dieIfFaultOccurred(&env);

    /* Dispose of unused result value. */
    xmlrpc_DECREF(resultP);

    /* Clean up our error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Shutdown our XML-RPC client library. */
    xmlrpc_client_cleanup();
    return 0;
}

/*
 * read server directory listing from a given path
 */
static int hoofs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    const char * const methodName = "rpc.readdir";

    xmlrpc_env env;
    xmlrpc_value * resultP;
    xmlrpc_env_init(&env);
    xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);

    dieIfFaultOccurred(&env);

    resultP = xmlrpc_client_call(&env, endpoint, methodName, "(s)", path); // run temp client for service
    dieIfFaultOccurred(&env);
    xmlrpc_read_string(&env, resultP, &buf);

    /* Dispose of unused result value. */
    xmlrpc_DECREF(resultP);

    /* Clean up our error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Shutdown our XML-RPC client library. */
    xmlrpc_client_cleanup();
    return 0;
}

/*
 * open file on server
 */
static int hoofs_open(const char *path, struct fuse_file_info *fi) {
    const char * const methodName = "rpc.open";

    xmlrpc_env env;
    xmlrpc_value * resultP;
    xmlrpc_env_init(&env);
    xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);

    dieIfFaultOccurred(&env);

    resultP = xmlrpc_client_call(&env, endpoint, methodName, "(s)", path); // run temp client for service
    dieIfFaultOccurred(&env);
    int fd = -1;
    xmlrpc_read_int(&env, resultP, &fd);
    fi->fh = (uint64_t) fd;


    /* Dispose of unused result value. */
    xmlrpc_DECREF(resultP);

    /* Clean up our error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Shutdown our XML-RPC client library. */
    xmlrpc_client_cleanup();
    return 0;
}

/*
 * remove file from server
 */
static int hoofs_release(const char *path, struct fuse_file_info *fi) {
    const char * const methodName = "rpc.release";

    xmlrpc_env env;
    xmlrpc_value * resultP;
    xmlrpc_env_init(&env);
    xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);

    dieIfFaultOccurred(&env);

    int fd = fi->fh;
    resultP = xmlrpc_client_call(&env, endpoint, methodName, "(i)", (xmlrpc_int32)fd); // run temp client for service
    dieIfFaultOccurred(&env);

    /* Dispose of unused result value. */
    xmlrpc_DECREF(resultP);

    /* Clean up our error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Shutdown our XML-RPC client library. */
    xmlrpc_client_cleanup();
    return 0;
}

/*
 * create file on server
 */
static int hoofs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    const char * const methodName = "rpc.create";

    xmlrpc_env env;
    xmlrpc_value * resultP;
    xmlrpc_env_init(&env);
    xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);

    dieIfFaultOccurred(&env);

    resultP = xmlrpc_client_call(&env, endpoint, methodName, "(si)", path,
                                 (xmlrpc_int32) mode); // run temp client for service
    dieIfFaultOccurred(&env);
    int fd = -1;
    xmlrpc_read_int(&env, resultP, &fd);
    fi->fh = (uint64_t) fd;

    /* Dispose of unused result value. */
    xmlrpc_DECREF(resultP);

    /* Clean up our error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Shutdown our XML-RPC client library. */
    xmlrpc_client_cleanup();
    return 0;
}

/*
 * unlink file on server
 */
static int hoofs_unlink(const char *path) {
    const char * const methodName = "rpc.unlink";

    xmlrpc_env env;
    xmlrpc_value * resultP;
    xmlrpc_env_init(&env);
    xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);

    dieIfFaultOccurred(&env);

    resultP = xmlrpc_client_call(&env, endpoint, methodName, "(s)", path); // run temp client for service
    dieIfFaultOccurred(&env);


    /* Dispose of unused result value. */
    xmlrpc_DECREF(resultP);

    /* Clean up our error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Shutdown our XML-RPC client library. */
    xmlrpc_client_cleanup();
    return 0;
}

/*
 * remove directory from server
 */
static int hoofs_rmdir(const char *path) {
    return hoofs_unlink(path);
}

/*
 * read file from server
 */
static int hoofs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    const char * const methodName = "rpc.read";

    xmlrpc_env env;
    xmlrpc_value * resultP;
    xmlrpc_env_init(&env);
    xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);

    dieIfFaultOccurred(&env);

    int fd = fi->fh;
    resultP = xmlrpc_client_call(&env, endpoint, methodName, "(iii)", (xmlrpc_int32)fd,
                                 (xmlrpc_int32) size,
                                 (xmlrpc_int32) offset); // run temp client for service
    dieIfFaultOccurred(&env);
    xmlrpc_read_string(&env, resultP, &buf);

    /* Dispose of unused result value. */
    xmlrpc_DECREF(resultP);

    /* Clean up our error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Shutdown our XML-RPC client library. */
    xmlrpc_client_cleanup();

    return sizeof(buf);
}

/*
 * write to file on server
 */
static int hoofs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    const char * const methodName = "rpc.write";

    xmlrpc_env env;
    xmlrpc_value * resultP;
    xmlrpc_env_init(&env);
    xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);

    dieIfFaultOccurred(&env);

    int fd = fi->fh;
    resultP = xmlrpc_client_call(&env, endpoint, methodName, "(iii)", (xmlrpc_int32)fd,
                                 (xmlrpc_int32) size,
                                 (xmlrpc_int32) offset); // run temp client for service
    dieIfFaultOccurred(&env);
    int written = -1;
    xmlrpc_read_int(&env, resultP, &written);

    /* Dispose of unused result value. */
    xmlrpc_DECREF(resultP);

    /* Clean up our error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Shutdown our XML-RPC client library. */
    xmlrpc_client_cleanup();

    return written;
}

static struct fuse_operations hoofs_oper = {
        .getattr = hoofs_getattr,
};

static int myfs_opt_proc(void *data, const char *arg, int key, struct fuse_args *outargs) {
    if (key == FUSE_OPT_KEY_NONOPT && fileSystemRoot == NULL) {
        fileSystemRoot = strdup(arg);
        return 0;
    }
    return 1;
}

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <server name/address> <port> <fuse params>...\n", argv[0]);
        exit(1);
    }
    printf("%s", "Starting Fuse Driver...\n");
    char ep[150];
    char po[6];
    char *addr;
    port = argv[2];
    int i;

    strcpy(ep, "http://");
    strcat(ep, argv[1]);
    server = ep;

    /* Remove Server Port Args from argv used in fuse args */
    for(i=2-1; i<argc-1; i++) {
        argv[i] = argv[i + 1];
    }
    argc--;
    for(i=2-1; i<argc-1; i++) {
        argv[i] = argv[i + 1];
    }
    argc--;

    /* Print array after deletion
    printf("argc: %d\n", argc);
    for (i = 0; i < argc; i++) {
        printf("%s ", *(argv + i));
    }
    printf("\n");

    printf("endpoint: %s\n", endpoint); */
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    fuse_opt_parse(&args, NULL, NULL, myfs_opt_proc);

    printf("Beginning Fuse Operations...\n");

    return fuse_main(args.argc, args.argv, &hoofs_oper, NULL);
}
