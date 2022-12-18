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
#include <xmlrpc-c/client.h>
#include <xmlrpc-c/server_abyss.h>
#include <sys/stat.h>
#include <utime.h>

#include "registryItems.h"

static char *fileSystemRoot = "./";

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

static xmlrpc_value *rpc_chown(xmlrpc_env *const envP,  xmlrpc_value *const paramArrayP, void *const serverInfo, void *const channelInfo) {
    const char *path;
    xmlrpc_int _uid;
    xmlrpc_int _gid;

    xmlrpc_decompose_value(envP, paramArrayP, "(sii)", &path, &_uid, &_gid);

    uid_t uid = (uid_t) _uid;
    gid_t gid = (gid_t) _gid;

    logMessage("Calling chown on %s\n", path);
    if(chown(path, uid, gid) < 0) {
        logMessage("chmod() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "i", -errno);
    }

    return xmlrpc_build_value(envP, "i", 0);
}

static xmlrpc_value *rpc_chmod(xmlrpc_env *const envP,  xmlrpc_value *const paramArrayP, void *const serverInfo, void *const channelInfo) {
    const char *path;
    xmlrpc_int _mode;

    xmlrpc_decompose_value(envP, paramArrayP, "(si)", &path, &_mode);
    mode_t mode = (mode_t) _mode;

    logMessage("calling chmod on %s with mode %d\n", path, mode);
    if(chmod(path, mode) < 0) {
        logMessage("chmod() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "i", -errno);
    }

    return xmlrpc_build_value(envP, "i", 0);
}

static xmlrpc_value *rpc_utime(xmlrpc_env *const envP,  xmlrpc_value *const paramArrayP, void *const serverInfo, void *const channelInfo) {
    const char *path;

    xmlrpc_decompose_value(envP, paramArrayP, "(s)", &path);

    struct utimbuf buf;

    /* Utime */
    logMessage("calling utime on %s\n", path);
    if(utime(path, &buf) < 0) {
        logMessage("utime() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "i", -errno);
    }

    return xmlrpc_build_value(envP, "{s:i,s:i}", "ac", buf.actime, "mod", buf.modtime);
}

static xmlrpc_value* rpc_readdir(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo) {
    char *path;

    /* Parse our argument array. */
    xmlrpc_decompose_value(envP, paramArrayP, "(s)", &path);
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
    char dirs[4000];
    struct dirent *dirEntry;
    while ((dirEntry = readdir(dirPtr)) != NULL) {
        if (strcmp(dirEntry->d_name, arg) != 0) {
            strcat(dirs, dirEntry->d_name);
            strcat(dirs, " ");
            // printf("valid directory item!\t%s\n", arg);

            continue;
        }
    }

    closedir(dirPtr);
    /* Return our result. */
    return xmlrpc_build_value(envP, "s", dirs);
}
static xmlrpc_value* rpc_open(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo) {
    xmlrpc_int flags;

    /* Parse our argument array. */
    const char * path;
    xmlrpc_decompose_value(envP, paramArrayP, "(si)", &path, &flags);

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
static xmlrpc_value* rpc_release(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo) {
    int fd;

    /* Parse our argument array. */
    xmlrpc_decompose_value(envP, paramArrayP, "(i)", &fd);
    if (envP->fault_occurred)
        return NULL;

    if (close(fd) < 0) {
        logMessage("closeOB() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "i", -1);
    }

    /* Return our result. */
    return xmlrpc_build_value(envP, "i", 0);
}
static xmlrpc_value* rpc_create(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo) {
    const char *path;
    int mode;

    printf("start...");
    /* Parse our argument array. */
    xmlrpc_decompose_value(envP, paramArrayP, "(si)", &path, &mode);
    printf("decompose...");

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
static xmlrpc_value* rpc_unlink(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo) {
    // xmlrpc_value *path;
    const char *path;

    /* Parse our argument array. */
    // const char * _path;
    xmlrpc_decompose_value(envP, paramArrayP, "(s)", &path);
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
static xmlrpc_value* rpc_rmdir(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo) {
    const char *path;

    /* Parse our argument array. */
    xmlrpc_decompose_value(envP, paramArrayP, "(s)", &path);
    if (envP->fault_occurred)
        return xmlrpc_build_value(envP, "i", -1);

    return rpc_unlink(envP, paramArrayP, serverInfo, channelInfo);
}
static xmlrpc_value* rpc_getAttr(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo) {
    const char *path;
    struct stat stbuf;

    /* Parse our argument array. */
    xmlrpc_decompose_value(envP, paramArrayP, "(s)", &path);
    if (envP->fault_occurred)
        return NULL;

    size_t pathLen = getFullPathLength(path);
    char fullPath[pathLen];
    getFullPath(path, fullPath, pathLen);
    logMessage("Getting attributes for %s\n", fullPath);

    // Fetch file stats from local file system
    if (lstat(fullPath, &stbuf) == -1) {
        logMessage("lstat() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "s", NULL);
    }
    /* Return our result. */
    return xmlrpc_build_value(envP, "{s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i}",
                              "st_dev", stbuf.st_dev,
                              "st_rdev",stbuf.st_rdev,
                              "st_ino",stbuf.st_ino,
                              "st_mode",stbuf.st_mode,
                              "st_nlink",stbuf.st_nlink,
                              "st_uid",stbuf.st_uid,
                              "st_gid",stbuf.st_gid,
                              "st_atime",stbuf.st_atime,
                              "st_mtime",stbuf.st_mtime,
                              "st_ctime",stbuf.st_ctime,
                              "st_size",stbuf.st_size);
}

static xmlrpc_value *rpc_setAttr(xmlrpc_env *const envP,  xmlrpc_value *const paramArrayP, void *const serverInfo, void *const channelInfo) {

    /* XMLRPC Attributes to unload parameter values into */
    const char *path;
    const char *attr_name;
    const char *attr_val;

    xmlrpc_int _size;
    xmlrpc_int flags;

    /* Syphon off values from parameters */
    xmlrpc_decompose_value(envP, paramArrayP, "(sssii)", &path, &attr_name, &attr_val, &_size, &flags);

    /* Unload from decomposition */
    size_t size = (size_t) size;

    logMessage("setting attributes for %s\n", path);

    if (setxattr(path, attr_name, attr_val, size, flags) < 0) {
        logMessage("setxattr() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "i", -errno);
    }

    return xmlrpc_build_value(envP, "i", 0);
}

static xmlrpc_value* rpc_rename(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo) {
    const char *path;
    const char *newpath;


    xmlrpc_decompose_value(envP, paramArrayP, "(ss)", &path, &newpath);
    size_t pathLen = getFullPathLength(path);
    size_t _pathLen = getFullPathLength(newpath);

    char fullPath[pathLen];
    char _fullPath[_pathLen];

    getFullPath(path, fullPath, pathLen);
    getFullPath(newpath, _fullPath, _pathLen);

    printf("path: %s\n", path);
    printf("newpath: %s\n", newpath);


    if (envP->fault_occurred)
        return xmlrpc_build_value(envP, "i", -1);

    if (rename(path, newpath) != 0) {
        logMessage("rename() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "i", -errno);
    }
    return xmlrpc_build_value(envP, "i", 0);
}

static xmlrpc_value* rpc_read(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo) {
    int fd, size, offset;

    /* Parse our argument array. */
    xmlrpc_decompose_value(envP, paramArrayP, "(iii)", &fd, &size, &offset);
    char buf[size];
    if (envP->fault_occurred)
        return NULL;
    
    logMessage("Reading from open file\n");

    // Go to file offset
    if (lseek(fd, offset, SEEK_SET) < 0) {
        logMessage("lseek() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "s", NULL); }

    // Read bytes
    ssize_t readBytes = read(fd, buf, size);
    if (readBytes < 0) {
        logMessage("read() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "s", NULL);    }

    /* Return our result. */
    // printf("read: %s\n", buf);
    return xmlrpc_build_value(envP, "s", buf);
}
static xmlrpc_value* rpc_write(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo) {
    int fd, size, offset;
    char *path; char *buf;

    /* Parse our argument array. */
    xmlrpc_decompose_value(envP, paramArrayP, "(isii)", &fd, &buf, &size, &offset);
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
        return xmlrpc_build_value(envP, "i", -1);
    }

    /* Return our result. */
    return xmlrpc_build_value(envP, "i", (int) writtenBytes);
}

static xmlrpc_value *rpc_truncate(xmlrpc_env *const envP,  xmlrpc_value *const paramArrayP, void *const serverInfo, void *const channelInfo) {
    const char *path;
    xmlrpc_int _size;

    xmlrpc_decompose_value(envP, paramArrayP, "(si)", &path, &_size);

    off_t size = (off_t) _size;
    size_t pathLen = getFullPathLength(path);
    char fullPath[pathLen];
    getFullPath(path, fullPath, pathLen);

    /* Truncate */
    logMessage("Truncating file/directory %s\n", fullPath);
    if(truncate(fullPath, size) < 0) {
        logMessage("truncate() failed: %s\n", strerror(errno));
        return xmlrpc_build_value(envP, "i", -errno);
    }

    return xmlrpc_build_value(envP, "i", 0);
}

int main(int const argc, const char ** const argv) {
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

    char cwd[100];
    fileSystemRoot = getcwd(cwd, 100);

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
    xmlrpc_registry_add_method3(&env, registryP, &getattrInfo);
    xmlrpc_registry_add_method3(&env, registryP, &setattrInfo);
    xmlrpc_registry_add_method3(&env, registryP, &utimeInfo);
    xmlrpc_registry_add_method3(&env, registryP, &renameInfo);
    xmlrpc_registry_add_method3(&env, registryP, &chmodInfo);
    xmlrpc_registry_add_method3(&env, registryP, &chownInfo);
    xmlrpc_registry_add_method3(&env, registryP, &rmdirInfo);
    xmlrpc_registry_add_method3(&env, registryP, &readInfo);
    xmlrpc_registry_add_method3(&env, registryP, &writeInfo);
    xmlrpc_registry_add_method3(&env, registryP, &truncateInfo);

    if (env.fault_occurred) {
        printf("xmlrpc_registry_add_method() failed.  %s\n",
               env.fault_string);
        exit(1);
    }

    serverparm.config_file_name = NULL;   /* Select the modern normal API */
    serverparm.registryP        = registryP;
    serverparm.port_number      = atoi(argv[1]);
    serverparm.log_file_name    = "/tmp/xmlrpc_log";

    printf("Running XML-RPC server...\n");

    xmlrpc_server_abyss(&env, &serverparm, XMLRPC_APSIZE(log_file_name));
    if (env.fault_occurred) {
        printf("xmlrpc_server_abyss() failed.  %s\n", env.fault_string);
        exit(1);
    }
    /* xmlrpc_server_abyss() never returns unless it fails */

    return 0;
}


