/************************************************
 * Author: Malik Mohamedali
 * Assignment Name: Program 6
 * Course: CSI 4337
 ************************************************/

#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client_simple.hpp>
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <string>
#include <fcntl.h>
#include <utime.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <cstdlib>
#include <cstring>

#include "include/xmlrpc-c/config.h"  /* information about this build environment */

#ifndef P6_P8_HOOFSRPCCLIENT_H
#define P6_P8_HOOFSRPCCLIENT_H

using namespace std;
using namespace xmlrpc_c;

class rpcClient {
    clientSimple myClient;

public:
    string endpoint;
    rpcClient               (string serverUrl, string port);
    ~rpcClient();
    int rpc_readdir         (const char *path, void *buf);
    int rpc_open            (const char *path, int flags);
    int rpc_create          (const char *path, int mode);
    int rpc_unlink          (const char *path);
    int rpc_release         (int fd);
    int rpc_chmod           (const char *path, mode_t mode);
    int rpc_chown           (const char *path, uid_t uid, gid_t gid);
    int rpc_utime           (const char *path, struct utimbuf *ubuf);
    int rpc_truncate        (const char *path, off_t newsize);
    int rpc_getAttr(const char *path, struct stat *stbuf);
    int rpc_setxattr        (const char *path, const char *name, const char *_value, size_t size, int flags);
    int rpc_rename(const char *path, const char *newpath);
    int rpc_rmdir           (const char *path);
    int rpc_read            (char *buf, int fd, int size, int offset);
    int rpc_write           (int fd, const char *buf, int size, int offset);
};

#endif //P6_P8_HOOFSRPCCLIENT_H
