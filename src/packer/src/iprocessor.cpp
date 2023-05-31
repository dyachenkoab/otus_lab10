#include "iprocessor.h"

void iProcessor::attach(Observer *o)
{
    std::lock_guard<std::mutex> lock(m_observersMutex);
    m_observers.insert(o);
}

void iProcessor::detach(Observer *o)
{
    std::lock_guard<std::mutex> lock(m_observersMutex);
    m_observers.erase(o);
}
