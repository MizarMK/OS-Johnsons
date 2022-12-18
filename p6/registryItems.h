/************************************************
 * Author: Malik Mohamedali
 * Assignment Name: Program 6
 * Course: CSI 4337
 ************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <dirent.h>
#include <fcntl.h>
#include "include/xmlrpc-c/config.h"  /* information about this build environment */

#ifndef P7_REGISTRYITEMS_H
#define P7_REGISTRYITEMS_H

static xmlrpc_value* rpc_getAttr(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo);
static xmlrpc_value* rpc_setAttr(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo);
static xmlrpc_value* rpc_rename(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo);
static xmlrpc_value* rpc_chmod(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo);
static xmlrpc_value* rpc_chown(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo);
static xmlrpc_value* rpc_utime(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo);
static xmlrpc_value* rpc_truncate(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo);
static xmlrpc_value* rpc_readdir(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo);
static xmlrpc_value* rpc_rmdir(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo);
static xmlrpc_value* rpc_open(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo);
static xmlrpc_value* rpc_release(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo);
static xmlrpc_value* rpc_create(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo);
static xmlrpc_value* rpc_unlink(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo);
static xmlrpc_value* rpc_read(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo);
static xmlrpc_value* rpc_write(xmlrpc_env *   const envP, xmlrpc_value * const paramArrayP, void *  const serverInfo, void *  const channelInfo);

struct xmlrpc_method_info3 const getattrInfo = {
        .methodName = "rpc.getattr",
        .methodFunction = &rpc_getAttr,
};

struct xmlrpc_method_info3 const setattrInfo = {
        .methodName = "rpc.setxattr",
        .methodFunction = &rpc_setAttr,
};

struct xmlrpc_method_info3 const renameInfo = {
        .methodName = "rpc.rename",
        .methodFunction = &rpc_rename,
};

struct xmlrpc_method_info3 const chmodInfo = {
        .methodName = "rpc.chmod",
        .methodFunction = &rpc_chmod,
};

struct xmlrpc_method_info3 const chownInfo = {
        .methodName = "rpc.chown",
        .methodFunction = &rpc_chown,
};

struct xmlrpc_method_info3 const utimeInfo = {
        .methodName = "rpc.utime",
        .methodFunction = &rpc_utime,
};

struct xmlrpc_method_info3 const truncateInfo = {
        .methodName = "rpc.truncate",
        .methodFunction = &rpc_truncate,
};

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

struct xmlrpc_method_info3 const readInfo = {
        .methodName = "rpc.read",
        .methodFunction = &rpc_read,
};

struct xmlrpc_method_info3 const writeInfo = {
        .methodName = "rpc.write",
        .methodFunction = &rpc_write,
};


#endif //P7_REGISTRYITEMS_H
