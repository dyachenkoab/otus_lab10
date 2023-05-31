#include "printers.h"

std::mutex ConsolePrinter::mtx_;

template<typename T>
void fillBuffer(std::ostream &buffer, const T &data)
{
    buffer << "bulk: ";
    if (!data.empty()) {
        buffer << *data.begin();

        for (auto it = std::next(data.begin()); it != data.end(); ++it) {
            buffer << "," << *it;
        }
    }
    buffer << '\n';
}

std::string fileName(time_point t)
{
    using namespace std::chrono;

    const auto ms = duration_cast<milliseconds>(t.time_since_epoch()) % 1000;
    const time_t tt = system_clock::to_time_t(t);
    const tm lt = *localtime(&tt);

    std::ostringstream oss;

    oss << "bulk" << std::put_time(&lt, "%H%M%S");
    oss << std::setfill('0') << std::setw(3) << ms.count();
    oss << ".log";

    return oss.str();
}

ConsolePrinter::ConsolePrinter(CommandProcessor &processor) : Observer(ObserverType::CONSOLE_)
{
    m_processor = &processor;
    m_processor->attach(this);
}

ConsolePrinter::~ConsolePrinter()
{
    if (m_processor) {
        m_processor->detach(this);
    }
}

void ConsolePrinter::update()
{
    while (!m_processor->stopped() || !m_processor->empty(m_type)) {
        BlockData data(m_processor->pop(type()));
        if (!data.data.empty()) {
            go(data);
        }
    }
}

void ConsolePrinter::go(const BlockData &data)
{
    std::ostringstream out;
    fillBuffer(out, data.data);
    std::unique_lock { mtx_ };
    std::cout << out.str() << std::endl;
}

FilePrinter::FilePrinter(CommandProcessor &processor) : Observer(ObserverType::FILE_)
{
    m_processor = &processor;
    m_processor->attach(this);
}

FilePrinter::~FilePrinter()
{
    if (m_processor) {
        m_processor->detach(this);
    }
}

void FilePrinter::update()
{
    while (!m_processor->stopped() || !m_processor->empty(ObserverType::FILE_)) {
        BlockData data(m_processor->pop(type()));
        if (!data.data.empty()) {
            go(data);
        }
    }
}

void FilePrinter::go(const BlockData &data)
{
    std::ofstream out;
    out.open(fileName(data.m_time));
    if (out.is_open()) {
        fillBuffer(out, data.data);
        out.close();
    }
}
