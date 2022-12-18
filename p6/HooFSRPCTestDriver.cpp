/************************************************
 * Author: Malik Mohamedali
 * Assignment Name: Program 6
 * Course: CSI 4337
 ************************************************/

#include <iostream>
#include <fcntl.h>
#include "HooFSRPCClient.h"

using namespace std;
using namespace xmlrpc_c;

int main(int argc, char **argv) { // FIXME client won't actually run a main, just implement an interface for driver to use
    if (argc-1 != 2) {
        cerr << "ERROR: usage ./client <server name> <server port>" << endl;
        exit(1);
    }
    char *serverUrl = "mohamedali@earth.ecs.baylor.edu"; // FIXME make client use serverUrl and port args
    char *dir = "bruh.txt";

    rpcClient client(argv[1], argv[2]);
    // char a[2000]; client.rpc_readdir ("", a); printf("Dir: %s\n", a); // DONE
    // int fd = client.rpc_create(dir, S_IRWXG); // DONE
    // int fd = client.rpc_open (dir, O_RDWR); // DONE
    // client.rpc_unlink (dir); // DONE
    // client.rpc_release (fd); printf("released fd: %d\n", ret); // DONE
    // client.rpc_chmod (dir, S_IRWXG); printf("chmod on fd\n", fd); // DONE
    // client.rpc_chown (dir, NULL, NULL); // DONE
    // struct utimbuf ut; client.rpc_utime (dir, &ut); cout << "ut's actime: " << ut.actime << endl; // DONE
    // client.rpc_truncate (dir, 4); // DONE
    // client.rpc_rename(dir,"_bruh.txt"); // DONE
    struct stat fd; client.rpc_getAttr(dir, &fd); cout << "stat's ctime: " << fd.st_ctime << endl; // DONE
    // int fd = client.rpc_setxattr (dir, "broski", "woski", 5, 0); // DONE
    // fd = client.rpc_rmdir (dir); // DONE
    // client.rpc_write (fd, "That's wild\n", 12, 0); // DONE
    // char b[10]; client.rpc_read (b, fd, 11, 0); printf("%s\n", b); //DONE

    return 0;
}