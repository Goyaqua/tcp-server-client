cmake_minimum_required(VERSION 3.28)
project(Client-Server C)

set(CMAKE_C_STANDARD 11)

add_executable(client client.c)

add_executable(server server.c)

add_executable(server_concurrent server-concurrent.c)
