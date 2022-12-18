cmake_minimum_required(VERSION 3.23)
project(p7)
include_directories(xmlrpc-c-1.54.06)

set(CMAKE_CXX_STANDARD 14)

add_executable(p7 HooFSRPCServer.c HooFSRPCClient.cpp registryItems.h HooFSFuse.c hoofs.c hoofs_interface.h hoofs_interface.cpp FuseBackup.cpp)
