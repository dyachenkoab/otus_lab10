#include "commandprocessor.h"

bool contains(const BlockData &blockData, const ObserverType &type)
{
    auto it = blockData.visitors.find(type);
    if (it != blockData.visitors.end()) {
        return true;
    }
    return false;
}

CommandProcessor::CommandProcessor(const std::string &openSymbol, const std::string &closeSymbol,
                   const size_t blockSize)
    : m_openSymbol(openSymbol)
    , m_closeSymbol(closeSymbol)
    , m_blockSize(blockSize)
{
}

bool CommandProcessor::empty(ObserverType type)
{
    std::shared_lock lock(m_mutex);
    if (m_data.empty()) {
        return true;
    }

    if (std::all_of(m_data.begin(), m_data.end(),
            [&type](const BlockData &blockData) { return contains(blockData, type); })) {
        return true;
    }

    return false;
}

bool CommandProcessor::stopped() const
{
    return m_stop;
}

void CommandProcessor::push(std::string &&command)
{
    //-----------------------
    if (m_stop) {
        return;
    }

    if (command.empty()) {
        std::unique_lock lck { m_mutex };
        m_data.emplace_back(std::move(m_currentBlock));
        m_stop = true;
        m_condition.notify_all();
        return;
    }
    //-----------------------

    if (!nested(command)) {
        {
            std::unique_lock lck { m_mutex };
            m_data.emplace_back(std::move(m_currentBlock));
        }
        m_condition.notify_all();
        return;
    }

    if (!validCommand(command)) {
        return;
    }

    m_currentBlock.data.emplace_back(std::move(command));
    const size_t modelSize = m_currentBlock.data.size();

    if (modelSize == 1) {
        m_currentBlock.m_time = std::chrono::system_clock::now();
    }

    if (!m_counter && modelSize == m_blockSize) {
        {
            {
                std::unique_lock lck { m_mutex };
                m_data.emplace_back(std::move(m_currentBlock));
            }
            m_condition.notify_all();
        }
    }
}

BlockData CommandProcessor::pop(ObserverType type)
{
    std::unique_lock lck { m_mutex };
    m_condition.wait(lck, [this] { return !m_data.empty() || m_stop; });

    BlockData blockData;
    if (!m_data.empty()) {
        size_t i = 0;
        for (; i < m_data.size(); ++i) {
            if (contains(m_data[i], type)) {
                continue;
            }
            m_data[i].visitors.insert(type);
            blockData = m_data[i];
            break;
        }
        if (blockData.visitors.size() == static_cast<int>(ObserverType::MAX_TYPES)) {
            m_data.erase(m_data.begin() + i);
        }
        m_condition.notify_all();
        return blockData;
    }
    m_condition.notify_all();
    return blockData;
}

bool CommandProcessor::nested(const std::string &command)
{
    if (command == m_openSymbol) {
        if (++m_counter == 1) {
            return false;
        }
    }
    if (command == m_closeSymbol) {
        if (--m_counter == 0) {
            return false;
        }
    }
    return true;
}

bool CommandProcessor::validCommand(const std::string &command) const
{
    return !(command == m_openSymbol || command == m_closeSymbol);
}
