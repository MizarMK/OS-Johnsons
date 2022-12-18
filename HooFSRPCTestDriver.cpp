/************************************************
 * Author: Malik Mohamedali
 * Assignment Name: Program 6
 * Course: CSI 4337
 ************************************************/

#include <iostream>
#include "HooFSRPCClient.h"

using namespace std;
using namespace xmlrpc_c;

int main(int argc, char **argv) { // FIXME client won't actually run a main, just implement an interface for driver to use
    if (argc-1 != 2) {
        cerr << "ERROR: usage ./client <server name> <server port>" << endl;
        exit(1);
    }
    char *serverUrl = "mohamedali@earth.ecs.baylor.edu"; // FIXME make client use serverUrl and port args
    char *dir = "/home/csi/m/mohamedali/cfiles/CSI4337/P6/hoofs.c";

    rpcClient client(argv[1], argv[2]);
    int fd = client.rpc_create(dir,NULL);
    cout << "method return fd: " << fd << endl;

    return 0;
}