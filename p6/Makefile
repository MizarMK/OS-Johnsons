CFLAGS = -Wall -Wextra -Wconversion -Wno-unused-parameter -g `pkg-config fuse --cflags`
LDFLAGS = `pkg-config fuse --libs`

RPCDIR = $(CURDIR)/xmlrpc-c-1.54.06
INCLUDE = -I. -I$(CURDIR)/include

default: HooFSRPCTestDriver HooFSRPCServer HooFSFuse rpcserver rpcclient

HooFSRPCServer: HooFSRPCServer.o
	gcc -o hoofsrpcserver HooFSRPCServer.o $(CURDIR)/xmlrpc-c-1.54.06/src/libxmlrpc_server_abyss.a $(CURDIR)/xmlrpc-c-1.54.06/src/libxmlrpc_server.a $(CURDIR)/xmlrpc-c-1.54.06/lib/abyss/src/libxmlrpc_abyss.a -lssl -lcrypto    $(CURDIR)/xmlrpc-c-1.54.06/src/libxmlrpc.a $(CURDIR)/xmlrpc-c-1.54.06/lib/expat/xmlparse/libxmlrpc_xmlparse.a $(CURDIR)/xmlrpc-c-1.54.06/lib/expat/xmltok/libxmlrpc_xmltok.a $(CURDIR)/xmlrpc-c-1.54.06/lib/libutil/libxmlrpc_util.a -lpthread

HooFSFuse: HooFSFuse.o HooFSRPCClient.o
	g++ $(CFLAGS) -o hoofsfuse HooFSFuse.o HooFSRPCClient.o $(LDFLAGS) $(CURDIR)/xmlrpc-c-1.54.06/src/cpp/libxmlrpc_client++.a $(CURDIR)/xmlrpc-c-1.54.06/src/libxmlrpc_client.a $(CURDIR)/xmlrpc-c-1.54.06/src/cpp/libxmlrpc++.a $(CURDIR)/xmlrpc-c-1.54.06/lib/libutil++/libxmlrpc_util++.a $(CURDIR)/xmlrpc-c-1.54.06/src/libxmlrpc.a $(CURDIR)/xmlrpc-c-1.54.06/lib/expat/xmlparse/libxmlrpc_xmlparse.a $(CURDIR)/xmlrpc-c-1.54.06/lib/expat/xmltok/libxmlrpc_xmltok.a $(CURDIR)/xmlrpc-c-1.54.06/lib/libutil/libxmlrpc_util.a -lpthread  -lcurl   $(CURDIR)/xmlrpc-c-1.54.06/src/cpp/libxmlrpc_packetsocket.a

rpcserver: rpcserver.o
	gcc $(CFLAGS) -o sampleaddserver rpcserver.o $(CURDIR)/xmlrpc-c-1.54.06/src/libxmlrpc_server_abyss.a $(CURDIR)/xmlrpc-c-1.54.06/src/libxmlrpc_server.a $(CURDIR)/xmlrpc-c-1.54.06/lib/abyss/src/libxmlrpc_abyss.a -lssl -lcrypto    $(CURDIR)/xmlrpc-c-1.54.06/src/libxmlrpc.a $(CURDIR)/xmlrpc-c-1.54.06/lib/expat/xmlparse/libxmlrpc_xmlparse.a $(CURDIR)/xmlrpc-c-1.54.06/lib/expat/xmltok/libxmlrpc_xmltok.a $(CURDIR)/xmlrpc-c-1.54.06/lib/libutil/libxmlrpc_util.a -lpthread

rpcclient: rpcclient.o
	gcc $(CFLAGS) -o sampleaddclient rpcclient.o $(LDFLAGS) $(CURDIR)/xmlrpc-c-1.54.06/src/cpp/libxmlrpc_client++.a $(CURDIR)/xmlrpc-c-1.54.06/src/libxmlrpc_client.a $(CURDIR)/xmlrpc-c-1.54.06/src/cpp/libxmlrpc++.a $(CURDIR)/xmlrpc-c-1.54.06/lib/libutil++/libxmlrpc_util++.a $(CURDIR)/xmlrpc-c-1.54.06/src/libxmlrpc.a $(CURDIR)/xmlrpc-c-1.54.06/lib/expat/xmlparse/libxmlrpc_xmlparse.a $(CURDIR)/xmlrpc-c-1.54.06/lib/expat/xmltok/libxmlrpc_xmltok.a $(CURDIR)/xmlrpc-c-1.54.06/lib/libutil/libxmlrpc_util.a -lpthread  -lcurl   $(CURDIR)/xmlrpc-c-1.54.06/src/cpp/libxmlrpc_packetsocket.a

HooFSRPCTestDriver: HooFSRPCClient.o HooFSRPCTestDriver.o
	g++ -std=c++11 -o hoofsrpctestdriver HooFSRPCTestDriver.o HooFSRPCClient.o $(CURDIR)/xmlrpc-c-1.54.06/src/cpp/libxmlrpc_client++.a $(CURDIR)/xmlrpc-c-1.54.06/src/libxmlrpc_client.a $(CURDIR)/xmlrpc-c-1.54.06/src/cpp/libxmlrpc++.a $(CURDIR)/xmlrpc-c-1.54.06/lib/libutil++/libxmlrpc_util++.a $(CURDIR)/xmlrpc-c-1.54.06/src/libxmlrpc.a $(CURDIR)/xmlrpc-c-1.54.06/lib/expat/xmlparse/libxmlrpc_xmlparse.a $(CURDIR)/xmlrpc-c-1.54.06/lib/expat/xmltok/libxmlrpc_xmltok.a $(CURDIR)/xmlrpc-c-1.54.06/lib/libutil/libxmlrpc_util.a -lpthread  -lcurl   $(CURDIR)/xmlrpc-c-1.54.06/src/cpp/libxmlrpc_packetsocket.a

HooFSRPCServer.o: HooFSRPCServer.c
	gcc -c -I. $(INCLUDE) HooFSRPCServer.c

HooFSFuse.o: HooFSFuse.c
	g++ $(CFLAGS) -c -I. $(INCLUDE) -D_FILE_OFFSET_BITS=64 HooFSFuse.c

HooFSRPCTestDriver.o: HooFSRPCTestDriver.cpp
	g++ -std=c++11 -c -I. $(INCLUDE) HooFSRPCTestDriver.cpp

HooFSRPCClient.o: HooFSRPCClient.cpp
	g++ -std=c++11 -c -I. $(INCLUDE) HooFSRPCClient.cpp

rpcserver.o:
	gcc -c -I. $(INCLUDE) -o rpcserver.o xmlrpc_sample_add_server.c

rpcclient.o:
	gcc -c -I. $(INCLUDE) -o rpcclient.o xmlrpc_sample_add_client.c



clean:
	rm *.o hoofsrpcserver hoofsrpctestdriver hoofsfuse