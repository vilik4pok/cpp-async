#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <thread>

enum { max_length = 6 };

bool process_command_line(int argc, char** argv, std::string& ip, size_t& port, size_t& threads, size_t& write_read) {
    try
    {
        boost::program_options::options_description desc("Usage:\n  client (-h | --help)\n  client <ip> <port> <threads> <write_read>\nOptions:");
        desc.add_options()
            ("help,h", "Help screen")
            ("ip,i", boost::program_options::value<std::string>(&ip)->required(), "Server ip")
            ("port,p", boost::program_options::value <size_t>(&port)->required(), "Server port")
            ("threads,t", boost::program_options::value<size_t>(&threads)->required(), "Thread count = client count")
            ("write_read,w", boost::program_options::value<size_t>(&write_read)->required(), "Write-read count");
        boost::program_options::positional_options_description pos_desc;
        pos_desc.add("ip", 1);
        pos_desc.add("port", 1);
        pos_desc.add("threads", 1);
        pos_desc.add("write_read", 1);
        boost::program_options::command_line_parser parser(argc, argv);
        parser.options(desc).positional(pos_desc);
        boost::program_options::parsed_options parsed_options = parser.run();
        boost::program_options::variables_map vm;
        store(parsed_options, vm);
        if (vm.count("help") || argc == 1) {
            std::cout << desc << '\n';
            return false;
        }
        notify(vm);
    }
    catch (const boost::program_options::error& ex)
    {
        std::cerr << ex.what() << '\n';
        return false;
    }
    return true;
}

void send_requests(boost::asio::ip::tcp::resolver::results_type ip, size_t count, size_t id) {
    try {
        {
            std::ostringstream ss;
            ss << "thread " << id << ": started\n";
            std::cout << ss.str();
        }
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::socket s(io_context);
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
        {
            std::ostringstream ss;
            ss << "thread " << id << ": ended\n";
            std::cout << ss.str();
        }
    }
    catch (std::exception& e) {
        std::cout << "thread " << id << ": " << e.what() << "\n";
    }
}

int main(int argc, char* argv[])
{
    try {
        std::string ip;
        size_t port, thread_count, write_read;
        if (process_command_line(argc, argv, ip, port, thread_count, write_read)) {
            boost::asio::io_context io_context;
            boost::asio::ip::tcp::resolver resolver(io_context);
            std::list<std::thread> threads;
            auto address = resolver.resolve(ip, std::to_string(port));
            for (size_t i = 1; i < thread_count + 1; i++) {
                threads.push_back(std::thread(send_requests, address, write_read, i));
            }
            for (auto& t : threads) {
                t.join();
            }
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    
    return 0;
}