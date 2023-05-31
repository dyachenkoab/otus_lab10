#include "asio_async_server.h"

//std::mutex session::mutex_;
bool session::isValid(const std::string &command)
{
    if (command == "{") {
        if (++m_counter == 1) {
            m_nested = true;
            return false;
        }
    }
    if (command == "}") {
        if (--m_counter == 0) {
            m_nested = false;
            return false;
        }
    }
    return true;
}

session::session(size_t id, tcp::socket socket, server *server)
    : id(id)
    , socket_(std::move(socket))
    , m_server(server)
{
}

void session::start()
{
    do_read();
}

void session::do_read()
{
//    boost::system::error_code ec;

//    const std::size_t bytes_transferred =
//        boost::asio::read_until(socket_, boost::asio::dynamic_buffer(data), '\n', ec);

//    if (ec && m_server) {
//        m_server->disconnect(id);

//    } else {

//        std::string command { data.substr(0, bytes_transferred - 1) }; // 1 is separator size
//        data.erase(0, bytes_transferred);

//        if (isValid(command) && !m_nested) {
//            packer::recieve(std::move(command), id);
//        } else {
//            bulks.emplace_back(std::move(command));
//            if (!m_nested) {
//                for (auto bulk : bulks) {
//                    packer::recieve(std::move(bulk), id);
//                }
//            }
//        }

//        do_read();
//    }

        socket_.async_read_some(boost::asio::buffer(data_, max_length), [this](boost::system::error_code ec,
                                               std::size_t length) {
            if ((boost::asio::error::eof == ec) || (boost::asio::error::connection_reset == ec)) {
                if (m_server) {
                    m_server->disconnect(id);
                }
            }
            if (!ec) {
                std::string command { data_, length };
                std::cout << "command: " << command << " size:" << length << '\n';
                if (isValid(command) && !m_nested) {
                    packer::recieve(std::move(command), id);
                } else {
                    bulks.emplace_back(std::move(command));
                    if (!m_nested) {
                        for (auto bulk : bulks) {
                            packer::recieve(std::move(bulk), id);
                        }
                    }
                }
                do_read();
            }
        });
}

server::server(boost::asio::io_context &io_context, short port, size_t bulksize)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    , m_bulksize(bulksize)
{
    do_accept();
}

server::~server()
{
    for (auto &counts : pool) {
        packer::disconnect(counts.id);
    }
    pool.clear();
    m_sessions.clear();
}

void server::disconnect(const size_t id)
{
    auto it = std::find_if(pool.begin(), pool.end(),
                   [&id](const Counts &counts) { return counts.id == id; });
    if (it != std::end(pool)) {
        --(it->count);
        if (!it->count) {
            std::cout << "disconnect from server " << id << '\n';
            packer::disconnect(it->id);
            pool.erase(it);
        }
    }
}

void server::do_accept()
{
    acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            size_t id = 0;
            if (auto freeCount = findFree()) {
                ++freeCount->count;
                id = freeCount->id;
                std::cout << "connect to created " << freeCount->id
                      << ", count: " << freeCount->count << '\n';

            } else {
                id = packer::connect(m_bulksize);
                pool.emplace_back(Counts { id, 1 });
                std::cout << "connect to new " << id << '\n';
            }

            auto session_ = std::make_shared<session>(id, std::move(socket), this);
            session_->start();
            m_sessions.insert(session_);
        }

        do_accept();
    });
}

Counts *server::findFree()
{
    for (auto &counts : pool) {
        if (counts.count < 3) // sessions communicates with command thread
            return &counts;
    }
    return nullptr;
}
