/************************************************
 * Author: Malik Mohamedali
 * Assignment Name: Program 6
 * Course: CSI 4337
 ************************************************/

#include "HooFSRPCClient.h"
#include <XmlRpcCpp.h>
#include <iostream>

/**
 * create client obj
 * @param serverUrl server url
 * @param port server port
 */
rpcClient::rpcClient (string serverUrl, string port) {
    endpoint = "http://"
            + serverUrl + ":"
            + port
            + "/RPC2";
    cout << "endpoint: " << endpoint << endl;
}

/**
 * destroy client obj
 */
rpcClient::~rpcClient() {}

/**
 * read directory from server absolute path
 * @param path path to read
 * @return directory listing
 */
int rpcClient::rpc_readdir(const char *path, void *buf) {
    string const methodName("rpc.readdir");

    value result;
    try {
        myClient.call(endpoint, methodName, "s", &result, path);
        string s = (string) value_string(result);
        strcpy((char *)buf, s.c_str());
    }
    catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }
    return 0;
}

/**
 * open file on server
 * @param path path to file
 * @param flags open flags
 * @return server file fd value
 */
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

/**
 * close file on server
 * @param fd server file fd
 * @return server file fd
 */
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

int rpcClient::rpc_chmod (const char *path, mode_t mode) {
    string const methodName("rpc.chmod");

    value result;

    try {
        myClient.call(endpoint, methodName, "si", &result, path, mode);
    }
    catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }

    int fd = value_int(result);
    return fd;
}
int rpcClient::rpc_chown (const char *path, uid_t uid, gid_t gid) {
    string const methodName("rpc.chown");

    value result;

    try {
        myClient.call(endpoint, methodName, "sii", &result, path, uid, gid);
    }
    catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }

    int fd = value_int(result);
    return fd;
}
int rpcClient::rpc_utime (const char *path, struct utimbuf *ubuf) {
    string const methodName("rpc.utime");

    value result, ac, mod;
    struct utimbuf ut;

    try {
        myClient.call(endpoint, methodName, "s", &result, path);
        value_struct item(result);

        map<std::string, value> const vals(static_cast<map<std::string, xmlrpc_c::value>>(item));
        /*for(auto it = vals.cbegin(); it != vals.cend(); ++it)
        {
            std::cout << it->first << " " << value_int(it->second) << endl;
        }*/

        ac = vals.at("ac"); ut.actime = value_int(ac);
        mod = vals.at("mod"); ut.modtime = value_int(mod);
    }
    catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }
    *ubuf = ut;

    return 0;
}

int rpcClient::rpc_truncate (const char *path, off_t newsize) {
    string const methodName("rpc.truncate");

    value result;

    try {
        myClient.call(endpoint, methodName, "si", &result, path, newsize);
    }
    catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }

    int fd = value_int(result);
    return fd;
}

/**
 * create file on server
 * @param path path to file
 * @param mode file mode flags
 * @return new server file fd
 */
int rpcClient::rpc_create (const char *path, int mode) {
    string const methodName("rpc.create");

    value result;
    try {
        myClient.call(endpoint, methodName, "si", &result, path, mode);
    }
    catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }

    int fd = value_int(result);
    return fd;
}

/**
 * unlink file on server
 * @param path absolute path to file (from server home)
 * @return unlinked server file fd
 */
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

/**
 * remove directory from server
 * @param path path to directory (absolute)
 * @return 0 if successful
 */
int rpcClient::rpc_rmdir (const char *path) {
    return rpc_unlink(path);
}

/**
 * get file attribute
 * @param path path to file
 * @param stbuf pointer to store file info in
 * @return buffer with newly read file info
 */
int rpcClient::rpc_getAttr(const char *path, struct stat *stbuf) {
    string const methodName("rpc.getattr");

    struct stat st;

    value result;
    value _dev, _rdev, _ino, _mode, _nlink, _uid, _gid, _atime, _mtime, _ctime,_size;
    try {
        cout << "before call..." << endl;

        paramList p;
        p.add(value_string(path));
        myClient.call(endpoint, methodName, p, &result);
        value_struct item(result);

        map<std::string, value> const vals(static_cast<map<std::string, xmlrpc_c::value>>(item));
        /*for(auto it = vals.cbegin(); it != vals.cend(); ++it)
        {
            std::cout << it->first << " " << value_int(it->second) << endl;
        }*/

        _dev = vals.at("st_dev"); st.st_dev = value_int(_dev);
        _rdev = vals.at("st_rdev"); st.st_rdev = value_int(_rdev);
        _ino = vals.at("st_ino"); st.st_ino = value_int(_ino);
        _mode = vals.at("st_mode"); st.st_mode = value_int(_mode);
        _nlink = vals.at("st_nlink"); st.st_nlink = value_int(_nlink);
        _uid = vals.at("st_uid"); st.st_uid = value_int(_uid);
        _gid = vals.at("st_gid"); st.st_gid = value_int(_gid);
        _atime = vals.at("st_atime"); st.st_atime = value_int(_atime);
        _mtime = vals.at("st_mtime"); st.st_mtime = value_int(_mtime);
        _ctime = vals.at("st_ctime"); st.st_ctime = value_int(_ctime);
        _size = vals.at("st_size"); st.st_size = value_int(_size);
    }
    catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }

    *stbuf = st;
    return 0;
}

/**
 * set file attribute
 * @param path path to file
 * @param name  name of attr
 * @param _value attr value
 * @param size attr size
 * @param flags attr flags
 * @return
 */
int rpcClient::rpc_setxattr(const char *path, const char *name, const char *_value, size_t size, int flags) {
    string const methodName("rpc.setxattr");

    value result;

    try {
        myClient.call(endpoint, methodName, "sssii", &result, path, name, _value, (int) size, flags);
    }
    catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }

    int ret = value_int(result);
    return ret;
}

int rpcClient::rpc_rename(const char *path, const char *newpath) {
    string const methodName("rpc.rename");

    value result;

    try {
        myClient.call(endpoint, methodName, "ss", &result, path, newpath);
        return value_int(result);
    }
    catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }
    return -1;
}

/**
 * read file from server
 * @param fd server file fd
 * @param size size to read
 * @param offset offset to start at
 * @return sequence of bytes read from file
 */
int rpcClient::rpc_read (char *buf, int fd, int size, int offset) {
        string const methodName("rpc.read");

        value result;

        try {
            myClient.call(endpoint, methodName, "iii", &result, fd, size, offset);
            strcpy(buf, ((string) value_string(result)).c_str());
            return sizeof(buf);
        }
        catch (exception const& e) {
            cerr << "Client threw error: " << e.what() << endl;
        } catch (...) {
            cerr << "Client threw unexpected error." << endl;
        }
        return -1;
}

/**
 * write to server file
 * @param fd server file fd
 * @param buf buffer to write bytes from
 * @param size size of buf
 * @param offset offset tio begin writing at
 * @return bytes written
 */
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
