#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H
#include <vector>
#include <string>
#include <queue>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <iostream>
#include <atomic>
#include <condition_variable>
#include <set>

#include "iprocessor.h"

using time_point = std::chrono::system_clock::time_point;

struct BlockData
{
    std::vector<std::string> data;
    time_point m_time;
    std::set<ObserverType> visitors;
};

class CommandProcessor : public iProcessor
{
public:
    CommandProcessor(const std::string &openSymbol, const std::string &closeSymbol, const size_t blockSize);
    ~CommandProcessor() = default;
    bool empty(ObserverType type);
    bool stopped() const;

    void push(std::string &&command);
    BlockData pop(ObserverType type);

private:
    const std::string m_openSymbol;
    const std::string m_closeSymbol;
    const size_t m_blockSize;

    BlockData m_currentBlock;
    std::deque<BlockData> m_data;

    std::atomic_int m_counter = 0;
    std::atomic_bool m_stop = false;

    std::mutex m_blockMutex;
    std::shared_mutex m_mutex;
    std::condition_variable_any m_condition;

    bool nested(const std::string &command);
    bool validCommand(const std::string &command) const;
};

#endif // COMMANDPROCESSOR_H
