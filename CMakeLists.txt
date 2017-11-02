cmake_minimum_required(VERSION 3.8)
project(ISA_LDAP_Server)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Connection.cpp Connection.h Messages.cpp Messages.h MessageID.h Database.cpp Database.h Decode.cpp Decode.h)
add_executable(ISA_LDAP_Server ${SOURCE_FILES} Messages.cpp Messages.h)