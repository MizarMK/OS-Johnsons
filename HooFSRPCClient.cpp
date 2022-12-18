/************************************************
 * Author: Malik Mohamedali
 * Assignment Name: Program 6
 * Course: CSI 4337
 ************************************************/

#include "HooFSRPCClient.h"
#include <iostream>


rpcClient::rpcClient (string serverUrl, string port) {
    endpoint = "http://"
            + serverUrl + ":"
            + port
            + "/RPC2";
}
rpcClient::~rpcClient() {}

string rpcClient::rpc_readdir(const char *path) {
    string const methodName("rpc.readdir");

    value result;
    try {
        myClient.call(endpoint, methodName, "s", &result, path);
        return (string) value_string(result);
    }
    catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }
    return NULL;
}
int rpcClient::rpc_open (const char *path, int flags) {
    string const methodName("rpc.open");

    value result;
    try{
        myClient.call(endpoint, methodName, "si", &result, path, flags);
    }
    catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }
    int fd = value_int(result);
    return fd;
}
int rpcClient::rpc_release(int fd) {
    string const methodName("rpc.release");

    value result;

    try {
        myClient.call(endpoint, methodName, "i", &result, fd);
    }
    catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }

    fd = value_int(result);
    return fd;
}
int rpcClient::rpc_create (const char *path, int mode) {
    string const methodName("rpc.create");

    value result;
    try {
        cout << "calling create..." << endl;
        myClient.call(endpoint, methodName, "si", &result, path, mode);
        cout << "after create..." << endl;
    }
    catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }

    int fd = value_int(result);
    return fd;
}
int rpcClient::rpc_unlink (const char *path) {
        string const methodName("rpc.unlink");

        value result;
        try {
            myClient.call(endpoint, methodName, "s", &result, path);
        }
        catch (exception const& e) {
            cerr << "Client threw error: " << e.what() << endl;
        } catch (...) {
            cerr << "Client threw unexpected error." << endl;
        }

        int fd = value_int(result);
        return fd;
}
int rpcClient::rpc_rmdir (const char *path) {
    string const methodName("rpc.rmdir");

    value result;
    try {
        myClient.call(endpoint, methodName, "s", &result, path);
    }
    catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }

    int fd = value_int(result);
    return fd;
}
struct stat *rpcClient::rpc_getAttr(const char *path, struct stat *stbuf) {
    string const methodName("rpc.getAttr");

    struct stat *st;
    value result;
    try {
        myClient.call(endpoint, methodName, "sS", &result, path, *stbuf);
        value_struct res(result);
        st = (struct stat *)(&res);
    }
    catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }

    // stbuf = value_struct(result);
    return st;
}
string rpcClient::rpc_read (int fd, int size, int offset) {
        string const methodName("rpc.read");

        string contain = "";
        value result;

        try {
            myClient.call(endpoint, methodName, "iii", &result, fd, size, offset);
        }
        catch (exception const& e) {
            cerr << "Client threw error: " << e.what() << endl;
        } catch (...) {
            cerr << "Client threw unexpected error." << endl;
        }
        contain = (string) value_string(result);

        if (contain.size() <= 0)
            exit(3);

        return contain;
}
int rpcClient::rpc_write (int fd, const char *buf, int size, int offset) {
    string const methodName("rpc.write");

    int bw = 0;
    value result;

    try {
        myClient.call(endpoint, methodName, "isii", &result, fd, buf, size, offset);
    }
    catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }
    bw = value_int(result);
    return bw;
}
