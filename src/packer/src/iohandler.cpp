#include "iohandler.h"

IOHandler::~IOHandler()
{
    for (auto &[_, thread] : threads) {
        thread->feed("");
    }
    threads.clear();
}

size_t IOHandler::connect(const size_t bulkSize)
{
    std::unique_lock { m_mutex };
    auto commandThread = std::make_unique<CommandThread>(shared_from_this(), bulkSize);
    auto id = commandThread->id();
    threads[id] = std::move(commandThread);
    return id;
}

void IOHandler::recieve(std::string &&buf, const size_t id)
{
    //    Передавать std'шные элементы через границу модуля std::thread::id, std::string часто нежелательно.
    //    Причина - ABI. Даже дебажная и релизная сборка одной и той же версией компилятора могут генерировать несовместимый код.
    std::unique_lock { m_mutex };
    threads.at(id)->feed(buf.data());
}

void IOHandler::disconnect(const size_t id)
{
    std::unique_lock { m_mutex };
    threads.at(id)->feed("");
    threads.erase(id);
}

//----------------------CommandThread----------------------------------
CommandThread::CommandThread(std::shared_ptr<IOHandler> ptr, const size_t bulkSize) : m_ptr(ptr)
{
    processor.reset(new CommandProcessor("{", "}", bulkSize > 0 ? bulkSize : 3));
    ConsolePrinter printer(*processor);
    FilePrinter fPrinter(*processor);
    FilePrinter fPrinter1(*processor);

    threads[0] = std::thread(&ConsolePrinter::update, printer);
    threads[1] = std::thread(&FilePrinter::update, fPrinter);
    threads[2] = std::thread(&FilePrinter::update, fPrinter1);

    m_workerThread = std::thread(&CommandThread::wait, this);
}

CommandThread::~CommandThread()
{
    m_workerThread.join();
}

void CommandThread::feed(std::string &&data)
{
    if (processor)
        processor->push(std::move(data));
}

size_t CommandThread::id()
{
    using namespace std::chrono;
    auto clock = system_clock::now();
    return duration_cast<nanoseconds>(clock.time_since_epoch()).count();
}

void CommandThread::wait()
{
    for (auto &thread : threads) {
        thread.join();
    }
}
