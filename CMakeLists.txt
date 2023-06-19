cmake_minimum_required(VERSION 3.15.0)
cmake_policy(SET CMP0016 NEW)
project(async_server VERSION 1.0.0 DESCRIPTION "Знакомство с асинхронным вводом/выводом на C++")
find_package(Boost 1.82 REQUIRED COMPONENTS system program_options)
add_executable(async_server src/async_server/async_server.cpp)
add_executable(client src/client/client.cpp)
target_link_libraries(async_server Boost::system )
target_link_libraries(client Boost::system Boost::program_options)

