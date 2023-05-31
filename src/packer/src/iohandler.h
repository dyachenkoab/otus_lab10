#ifndef IOHANDLER_H
#define IOHANDLER_H
#include <string>
#include <thread>
#include <unordered_map>
#include <mutex>

#include "printers.h"
class CommandThread;

class IOHandler : public std::enable_shared_from_this<IOHandler>
{
public:
    IOHandler() = default;
    ~IOHandler();

    size_t connect(const size_t bulkSize);
    void recieve(std::string &&buf, const size_t id);
    void disconnect(const size_t id);

private:
    std::unordered_map<size_t, std::unique_ptr<CommandThread>> threads;
    std::mutex m_mutex;
};

//-----------------CommandThread--------------------------
class CommandThread
{
public:
    CommandThread(std::shared_ptr<IOHandler> ptr, const size_t bulkSize);
    ~CommandThread();

    void feed(std::string &&data);
    size_t id();

private:
    std::unique_ptr<CommandProcessor> processor;
    std::shared_ptr<IOHandler> m_ptr;

    std::thread m_workerThread;
    std::array<std::thread, 3> threads;

    size_t m_id;

    void wait();
};

#endif // IOHANDLER_H
