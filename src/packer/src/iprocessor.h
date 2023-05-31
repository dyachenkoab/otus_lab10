#ifndef IPROCESSOR_H
#define IPROCESSOR_H
#include "observer.h"
#include <set>
#include <mutex>
class iProcessor
{
public:
    ~iProcessor() {};
    void attach(Observer *o);
    void detach(Observer *o);

protected:
    iProcessor() = default;

private:
    std::set<Observer*> m_observers;
    std::mutex m_observersMutex;
};

#endif // IPROCESSOR_H
