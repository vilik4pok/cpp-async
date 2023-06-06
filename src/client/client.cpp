//
// blocking_tcp_echo_client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <thread>
//#include <memory>
//#include <string.h>
using boost::asio::ip::tcp;

enum { max_length = 1024 };


void send_requests(tcp::resolver::results_type ip, size_t count, size_t id) {
    {
        std::ostringstream ss;
        ss << "thread " << id << ": started\n";
        std::cout << ss.str();
    }
    try {
        boost::asio::io_context io_context;
        tcp::socket s(io_context);
        boost::asio::connect(s, ip);
        char request[max_length] = "12345";
        char response[max_length];
        for (size_t i = 0; i < count; i++) {
            size_t request_length = std::strlen(request);
            boost::asio::write(s, boost::asio::buffer(request, request_length));
            size_t response_length = boost::asio::read(s, boost::asio::buffer(response, request_length));
            std::ostringstream ss;
            ss << "thread " << id << ": request = " << request << " response = ";
            ss.write(response, response_length);
            ss << "\n";
            std::cout << ss.str();
        }
    }
    catch (std::exception& e) {
        std::cout << "thread " << id << ": " << e.what() << "\n";
    }
    {
        std::ostringstream ss;
        ss << "thread " << id << ": ended\n";
        std::cout << ss.str();
    }
}

int main(int argc, char* argv[])
{
    if (argc < 3)
        return 0;
    size_t thread_count = 5;
    size_t read_write_count = 10;
    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);
    std::list<std::thread> threads(thread_count);
    auto ip = resolver.resolve(argv[1], argv[2]);
    for (size_t i = 1; i < thread_count + 1; i++) {
        threads.push_back(std::thread(send_requests, ip, read_write_count, i));
    }
    for (auto& t : threads) {
        t.join();
    }

    return 0;
}