/************************************************
 * Author: Malik Mohamedali
 * Assignment Name: Program 6
 * Course: CSI 4337
 ************************************************/

#ifndef P6_P8_RPCSTRUCTS_H
#define P6_P8_RPCSTRUCTS_H

struct xmlrpc_method_info3 const getAttrInfo = {
        .methodName = "rpc.getAttr",
        .methodFunction = &rpcClient::rpc_getAttr,
};

struct xmlrpc_method_info3 const readdirInfo = {
        .methodName = "rpc.readdir",
        .methodFunction = &rpcClient::rpc_readdir,
};

struct xmlrpc_method_info3 const openInfo = {
        .methodName = "rpc.open",
        .methodFunction = &rpcClient::rpc_open,
};

struct xmlrpc_method_info3 const releaseInfo = {
        .methodName = "rpc.release",
        .methodFunction = &rpcClient::rpc_release,
};

struct xmlrpc_method_info3 const createInfo = {
        .methodName = "rpc.create",
        .methodFunction = &rpcClient::rpc_create,
};

struct xmlrpc_method_info3 const unlinkInfo = {
        .methodName = "rpc.unlink",
        .methodFunction = &rpcClient::rpc_unlink,
};

struct xmlrpc_method_info3 const rmdirInfo = {
        .methodName = "rpc.rmdir",
        .methodFunction = &rpcClient::rpc_rmdir,
};

struct xmlrpc_method_info3 const readInfo = {
        .methodName = "rpc.read",
        .methodFunction = &rpcClient::rpc_read,
};

struct xmlrpc_method_info3 const writeInfo = {
        .methodName = "rpc.write",
        .methodFunction = &rpcClient::rpc_write,
};

#endif //P6_P8_RPCSTRUCTS_H
