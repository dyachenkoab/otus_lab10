#ifndef ASIO_ASYNC_SERVER_H
#define ASIO_ASYNC_SERVER_H
//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <memory>
#include <thread>
#include <set>
#include <atomic>
#include <utility>
#include <boost/asio.hpp>
#include <async.h>

using boost::asio::ip::tcp;
class server;

struct Counts
{
    size_t id;
    size_t count;
};

class session
{
public:
    session(size_t id, tcp::socket socket, server *server);
    void start();

private:
    void do_read();
    bool isValid(const std::string &command);

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];

    size_t id;
    std::vector<std::string> bulks;

    server *m_server;
//    static std::mutex mutex_;
//    std::string data;

    std::atomic_size_t m_counter { 0 };
    std::atomic_bool m_nested { false };
};

class server
{
public:
    server(boost::asio::io_context &io_context, short port, size_t bulksize);
    ~server();

    void disconnect(const size_t id);

private:
    void do_accept();
    Counts *findFree();

    tcp::acceptor acceptor_;
    std::vector<Counts> pool;
    std::set<std::shared_ptr<session>> m_sessions;

    const size_t m_bulksize;
};

int main(int argc, char *argv[])
{
    try {
        if (argc != 3) {
            std::cerr << "Usage: async_tcp_echo_server <port> <bulksize>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        server server(io_context, std::atoi(argv[1]), std::atoi(argv[2]));

        io_context.run();
    } catch (const std::exception &ex) {
        std::cerr << "Exception: " << ex.what() << "\n";
    }

    return 0;
}

#endif // ASIO_ASYNC_SERVER_H
