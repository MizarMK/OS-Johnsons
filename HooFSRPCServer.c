/************************************************
 * Author: Malik Mohamedali
 * Assignment Name: Program 6
 * Course: CSI 4337
 ************************************************/

#define WIN32_LEAN_AND_MEAN  /* required by xmlrpc-c/server_abyss.h */
#ifdef _WIN32
#  include <windows.h>
#  include <winsock2.h>
#else
#  include <unistd.h>
#endif

#include <xmlrpc-c/base.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_abyss.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
// #include "include/xmlrpc-c/config.h"  /* information about this build environment */

static char *fileSystemRoot;

#ifdef _WIN32
#define SLEEP(seconds) SleepEx(seconds * 1000, 1);
#else
#define SLEEP(seconds) sleep(seconds);
#endif

/*
 * Write given log message to console
 */
static void logMessage(const char *format, ...) {
    va_list argList;
    va_start(argList, format);
    vfprintf(stdout, format, argList);
}

/*
 * Compute the byte length of the full path string
 * from the root (fileSystemRoot + path)
 */
static size_t getFullPathLength(const char *path) {
    return strlen(fileSystemRoot) + strlen(path) + 1;
}

/*
 * Compute the full path from the root (fileSystemRoot + path).
 * We assume the user only wants up to n-1 bytes of the full path.
 */
static char *getFullPath(const char *path, char *fullPath, size_t n) {
    strncpy(fullPath, fileSystemRoot, n);
    strncat(fullPath, path, n);

    return fullPath;
}
static xmlrpc_value* rpc_readdir(xmlrpc_env *   const envP,
                              xmlrpc_value * const paramArrayP,
                              void *  const serverInfo,
                              void *  const channelInfo) {
    xmlrpc_value *_path;

    /* Parse our argument array. */
    xmlrpc_decompose_value(envP, paramArrayP, "(s)", &_path);
    const char *path = (char *) _path;
    if (envP->fault_occurred)
        return NULL;

    // Compute the full path name
    size_t pathLen = getFullPathLength(path);
    char fullPath[pathLen];
    getFullPath(path, fullPath, pathLen);
    logMessage("Reading directory for %s\n", fullPath);

    // Fetch the directory information
    DIR *dirPtr = opendir(fullPath);
    if (dirPtr == NULL) {
        logMessage("opendir() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "s", NULL);    }

    const char * arg;
    char *dirs;
    struct dirent *dirEntry;
    while ((dirEntry = readdir(dirPtr)) != NULL) {
        if (strcmp(dirEntry->d_name, arg) != 0)
            continue;

        strcat(dirs, arg);
    }

    closedir(dirPtr);
    /* Return our result. */
    return xmlrpc_build_value(envP, "s", dirs);
}
static xmlrpc_value* rpc_open(xmlrpc_env *   const envP,
                              xmlrpc_value * const paramArrayP,
                              void *  const serverInfo,
                              void *  const channelInfo) {
    xmlrpc_int flags;
    xmlrpc_value *_path;

    /* Parse our argument array. */
    xmlrpc_decompose_value(envP, paramArrayP, "(si)", &_path, &flags);
    const char *path = (char *) _path;
    /* Parse our argument array. */

    if (envP->fault_occurred) {
        printf("ERROR READING ARGS");
        return NULL;
    }

    // Compute the full path name
    size_t pathLen = getFullPathLength(path);
    char fullPath[pathLen];
    getFullPath(path, fullPath, pathLen);
    printf("Opening file %s\n", fullPath);

    int fd = open(fullPath, flags);
    if (fd <= 0) {
        printf("open() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "i", -1);
    }
    
    /* Return our result. */
    return xmlrpc_build_value(envP, "i", fd); // return fd for client to reference later
}
static xmlrpc_value* rpc_release(xmlrpc_env *   const envP,
                                xmlrpc_value * const paramArrayP,
                                void *  const serverInfo,
                                void *  const channelInfo) {
    xmlrpc_int f;

    /* Parse our argument array. */
    xmlrpc_decompose_value(envP, paramArrayP, "(i)", &f);
    int fd = f;
    if (envP->fault_occurred)
        return NULL;

    if (close(fd) < 0) {
        logMessage("closeOB() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "i", -1);
    }

    /* Return our result. */
    return xmlrpc_build_value(envP, "i", 0);
}
static xmlrpc_value* rpc_create(xmlrpc_env * const envP,
                              xmlrpc_value * const paramArrayP,
                              void *  const serverInfo,
                              void *  const callInfo) {
    xmlrpc_value *_path;
    xmlrpc_int _mode;

    /* Parse our argument array. */
    xmlrpc_decompose_value(envP, paramArrayP, "(si)", &_path, &_mode);
    const char *path = (char *)_path;
    mode_t mode = (mode_t)_mode;

    if (envP->fault_occurred)
        return NULL;

    // Compute the full path name
    size_t pathLen = getFullPathLength(path);
    char fullPath[pathLen];
    getFullPath(path, fullPath, pathLen);
    printf("Creating file %s\n", fullPath);

    int fd = creat(fullPath, mode);
    if (fd < 0) {
        printf("creat() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "i", -1);    }
    /* Return our result. */
    return xmlrpc_build_value(envP, "i", fd); // return fd for client to reference later
}
static xmlrpc_value* rpc_unlink(xmlrpc_env *   const envP,
                                 xmlrpc_value * const paramArrayP,
                                 void *  const serverInfo,
                                 void *  const channelInfo) {
     xmlrpc_value *_path;

    /* Parse our argument array. */
    // const char * _path;
    xmlrpc_decompose_value(envP, paramArrayP, "(s)", &_path);
    const char *path = (char *)_path;
    // xmlrpc_read_string(&env, &path, &_path);
    if (envP->fault_occurred)
        return NULL;

    // Compute the full path name
    size_t pathLen = getFullPathLength(path);
    char fullPath[pathLen];
    getFullPath(path, fullPath, pathLen);
    logMessage("Deleting file/directory %s\n", fullPath);

    int retVal = remove(fullPath);
    if (retVal < 0) {
        logMessage("remove() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "i", -1);    }

    /* Return our result. */
    return xmlrpc_build_value(envP, "i", 0);
    }    
static xmlrpc_value* rpc_rmdir(xmlrpc_env *   const envP,
                              xmlrpc_value * const paramArrayP,
                              void *  const serverInfo,
                              void *  const channelInfo) {

    return rpc_unlink(envP, paramArrayP, serverInfo, channelInfo);
}
static xmlrpc_value* rpc_getAttr(xmlrpc_env *   const envP,
                               xmlrpc_value * const paramArrayP,
                               void *  const serverInfo,
                               void *  const channelInfo) {
    xmlrpc_value *_path;
    xmlrpc_value *_st;


    /* Parse our argument array. */
    xmlrpc_decompose_value(envP, paramArrayP, "(sS)", &_path, &_st);
    const char *path = (char*)_path;
    struct stat *stbuf = (struct stat *)_st;

    if (envP->fault_occurred)
        return NULL;

    size_t pathLen = getFullPathLength(path);
    char fullPath[pathLen];
    getFullPath(path, fullPath, pathLen);
    logMessage("Getting attributes for %s\n", fullPath);

    // Fetch file stats from local file system
    if (lstat(fullPath, (struct stat *)stbuf) == -1) {
        logMessage("lstat() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "s", NULL);    }
    /* Return our result. */
    return xmlrpc_build_value(envP, "(S)", stbuf);
}
static xmlrpc_value* rpc_read(xmlrpc_env *   const envP,
                                 xmlrpc_value * const paramArrayP,
                                 void *  const serverInfo,
                                 void *  const channelInfo) {
    xmlrpc_int _fd, _size, _offset;

    /* Parse our argument array. */
    xmlrpc_decompose_value(envP, paramArrayP, "(iii)", &_fd, &_size, &_offset);
    int fd = (int) _fd;
    int size = (int) _size;
    int offset = (int) _offset;

    char buf[size];
    if (envP->fault_occurred)
        return NULL;
    
    logMessage("Reading from open file\n");

    // Go to file offset
    if (lseek(fd, offset, SEEK_SET) < 0) {
        logMessage("lseek() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "i", NULL);    }

    // Read bytes
    ssize_t readBytes = read(fd, buf, size);
    if (readBytes < 0) {
        logMessage("read() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "i", NULL);    }

    /* Return our result. */
    return xmlrpc_build_value(envP, "(s)", buf);
}
static xmlrpc_value* rpc_write(xmlrpc_env *   const envP,
                              xmlrpc_value * const paramArrayP,
                              void *  const serverInfo,
                              void *  const channelInfo) {
    xmlrpc_int _fd, _size, _offset;
    xmlrpc_value *_buf;

    /* Parse our argument array. */
    xmlrpc_decompose_value(envP, paramArrayP, "(isii)", &_fd, &_buf, &_size, &_offset);
    int fd = _fd;
    int size = _size;
    int offset = _offset;
    char *buf = (char *)_buf;

    if (envP->fault_occurred)
        return NULL;

    // Go to file offset
    if (lseek(fd, offset, SEEK_SET) < 0) {
        logMessage("lseek() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "i", -1);    }

    // Write bytes
    ssize_t writtenBytes = write(fd, buf, size);
    if (writtenBytes < 0) {
        logMessage("write() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "i", -1);    }

    /* Return our result. */
    return xmlrpc_build_value(envP, "i", (int) writtenBytes);
}

struct xmlrpc_method_info3 const readdirInfo = {
        .methodName = "rpc.readdir",
        .methodFunction = &rpc_readdir,
};

struct xmlrpc_method_info3 const openInfo = {
        .methodName = "rpc.open",
        .methodFunction = &rpc_open,
};

struct xmlrpc_method_info3 const releaseInfo = {
        .methodName = "rpc.release",
        .methodFunction = &rpc_release,
};

struct xmlrpc_method_info3 const createInfo = {
        .methodName = "rpc.create",
        .methodFunction = &rpc_create,
};

struct xmlrpc_method_info3 const unlinkInfo = {
        .methodName = "rpc.unlink",
        .methodFunction = &rpc_unlink,
};

struct xmlrpc_method_info3 const rmdirInfo = {
        .methodName = "rpc.rmdir",
        .methodFunction = &rpc_rmdir,
};

struct xmlrpc_method_info3 const getAttrInfo = {
        .methodName = "rpc.getAttr",
        .methodFunction = &rpc_getAttr,
};

struct xmlrpc_method_info3 const readInfo = {
        .methodName = "rpc.read",
        .methodFunction = &rpc_read,
};

struct xmlrpc_method_info3 const writeInfo = {
        .methodName = "rpc.write",
        .methodFunction = &rpc_write,
};

int main(int    const argc, const char ** const argv) {
    xmlrpc_server_abyss_parms serverparm;
    xmlrpc_registry * registryP;
    xmlrpc_env env;

    if (argc-1 != 1) {
        fprintf(stderr, "You must specify 1 argument:  The TCP port "
                        "number on which the server will accept connections "
                        "for RPCs (8080 is a common choice).  "
                        "You specified %d arguments.\n",  argc-1);
        exit(1);
    }

    xmlrpc_env_init(&env);
    registryP = xmlrpc_registry_new(&env);

    if (env.fault_occurred) {
        printf("xmlrpc_registry_new() failed.  %s\n", env.fault_string);
        exit(1);
    }

    xmlrpc_registry_add_method3(&env, registryP, &readdirInfo);
    xmlrpc_registry_add_method3(&env, registryP, &openInfo);
    xmlrpc_registry_add_method3(&env, registryP, &releaseInfo);
    xmlrpc_registry_add_method3(&env, registryP, &createInfo);
    xmlrpc_registry_add_method3(&env, registryP, &unlinkInfo);
    xmlrpc_registry_add_method3(&env, registryP, &getAttrInfo);
    xmlrpc_registry_add_method3(&env, registryP, &rmdirInfo);
    xmlrpc_registry_add_method3(&env, registryP, &readInfo);
    xmlrpc_registry_add_method3(&env, registryP, &writeInfo);

    if (env.fault_occurred) {
        printf("xmlrpc_registry_add_method() failed.  %s\n",
               env.fault_string);
        exit(1);
    }

    serverparm.config_file_name = NULL;   /* Select the modern normal API */
    serverparm.registryP        = registryP;
    serverparm.port_number      = atoi(argv[1]);
    serverparm.log_file_name    = "./xmlrpc_log";

    printf("Running XML-RPC server on port %d\n", serverparm.port_number);

    xmlrpc_server_abyss(&env, &serverparm, XMLRPC_APSIZE(log_file_name));
    if (env.fault_occurred) {
        printf("xmlrpc_server_abyss() failed.  %s\n", env.fault_string);
        exit(1);
    }
    /* xmlrpc_server_abyss() never returns unless it fails */

    return 0;
}


