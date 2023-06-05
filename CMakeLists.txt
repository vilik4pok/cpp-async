cmake_minimum_required(VERSION 3.15.0)
cmake_policy(SET CMP0016 NEW)

project(async_server VERSION 1.0.0 DESCRIPTION "Знакомство с асинхронным вводом/выводом на C++")
find_package(Boost 1.82 REQUIRED COMPONENTS system)
# include_directories(${CMAKE_SOURCE_DIR}/src)

# find_package(Boost 1.82 REQUIRED COMPONENTS asio)

add_executable(async_server src/async_server/async_server.cpp)
add_executable(client src/client/client.cpp)
target_link_libraries(async_server Boost::system)
target_link_libraries(client Boost::system)
# add_executable(main main.cpp)

#target_link_libraries(main Boost::system)

# enable_testing()
# add_subdirectory(test)
