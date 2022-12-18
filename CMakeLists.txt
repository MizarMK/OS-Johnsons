cmake_minimum_required(VERSION 3.23)
project(P6_P8)

set(CMAKE_CXX_STANDARD 14)

add_executable(P6_P8 hoofs.c HooFSRPCServer.c HooFSRPCClient.cpp rpcstructs.h HooFSRPCClient.h HooFSRPCTestDriver.cpp HooFSFuse.c)
