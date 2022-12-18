/************************************************
 * Author: Malik Mohamedali
 * Assignment Name: Program 6
 * Course: CSI 4337
 ************************************************/

#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <cstdlib>
#include <cstring>

#ifndef P6_P8_HOOFSRPCCLIENT_H
#define P6_P8_HOOFSRPCCLIENT_H

using namespace std;
using namespace xmlrpc_c;

class rpcClient {
    string endpoint;
    clientSimple myClient;

public:
    rpcClient               (string serverUrl, string port);
    ~rpcClient();
    string rpc_readdir      (const char *path);
    int rpc_open            (const char *path, int flags);
    int rpc_create          (const char *path, int mode);
    int rpc_unlink          (const char *path);
    int rpc_release         (int fd);
    struct stat *rpc_getAttr(const char *path, struct stat *stbuf);
    int rpc_rmdir           (const char *path);
    string rpc_read         (int fd, int size, int offset);
    int rpc_write           (int fd, const char *buf, int size, int offset);
};

#endif //P6_P8_HOOFSRPCCLIENT_H
