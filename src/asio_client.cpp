#include <boost/asio.hpp>

#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>

namespace ba = boost::asio;

int main(int, char *[])
{

    std::locale::global(std::locale(""));

    try {
        ba::io_context io_context;

        ba::ip::tcp::endpoint ep(ba::ip::address::from_string("127.0.0.1"), 5000);
        ba::ip::tcp::socket sock(io_context);
        sock.connect(ep);

        for (int i = 0; i < 4; ++i) {
            std::ostringstream oss;
            oss << "ping" << i;
            ba::write(sock, ba::buffer(oss.str(), 5));
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        {
            std::ostringstream oss;
            oss << "{";
            ba::write(sock, ba::buffer(oss.str(), 1));
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        for (int i = 0; i < 2; ++i) {
            std::ostringstream oss;
            oss << "ping" << i;
            ba::write(sock, ba::buffer(oss.str(), 5));
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        {
            std::ostringstream oss;
            oss << "{";
            ba::write(sock, ba::buffer(oss.str(), 1));
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        for (int i = 0; i < 3; ++i) {
            std::ostringstream oss;
            oss << "ping" << i;
            ba::write(sock, ba::buffer(oss.str(), 5));
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        {
            std::ostringstream oss;
            oss << "}";
            ba::write(sock, ba::buffer(oss.str(), 1));
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        {
            std::ostringstream oss;
            oss << "}";
            ba::write(sock, ba::buffer(oss.str(), 1));
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        for (int i = 0; i < 10; ++i) {
            std::ostringstream oss;
            oss << "ping" << i;
            ba::write(sock, ba::buffer(oss.str(), 5));
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

    } catch (const boost::system::system_error &ex) {
        std::cout << "boost exception! " << ex.what() << std::endl;
    } catch (const std::exception &ex) {
        std::cout << "std::exception! " << ex.what() << std::endl;
    }

    return 0;
}
