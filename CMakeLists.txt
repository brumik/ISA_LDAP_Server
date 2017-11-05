cmake_minimum_required(VERSION 3.8)
project(ISA_LDAP_Server)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Connection.cpp Connection.h Database.cpp Database.h Decode.cpp Decode.h Codes.h Encode.cpp Encode.h Error.cpp Error.h Logger.cpp Logger.h)
add_executable(ISA_LDAP_Server ${SOURCE_FILES})