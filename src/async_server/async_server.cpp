#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;

class session
    : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket, size_t client_id)
        : socket_(std::move(socket))
    {
        this->client_id = client_id;
        std::cout << "connection established: " << client_id << "\n";
    }

    void start()
    {
        do_read();
    }

    ~session() {
        std::cout << "connection lost: " << client_id << "\n";
    }

private:
    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    do_write(length);
                }
            });
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
            [this, self](boost::system::error_code ec, std::size_t)
            {
                if (!ec)
                {
                    do_read();
                }
            });
    }
    
    tcp::socket socket_;
    enum { max_length = 6 };
    char data_[max_length];
    size_t client_id;
};

class server
{
public:
    server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        do_accept();
    }

private:
    void do_accept()
    {
        client_count++;
        size_t client_id = client_count;
        acceptor_.async_accept(
            [this, client_id](boost::system::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<session>(std::move(socket), client_id)->start();
                }
                do_accept();
            });
    }
    tcp::acceptor acceptor_;
    size_t client_count = 0;
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }
        boost::asio::io_context io_context;
        server s(io_context, std::atoi(argv[1]));
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}