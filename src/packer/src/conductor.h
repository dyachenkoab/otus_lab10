#ifndef CONDUCTOR_H
#define CONDUCTOR_H
#include "iohandler.h"

class Conductor
{
public:
    static Conductor &instance();
    size_t connect(const size_t bulkSize);
    void recieve(std::string &&buf, const size_t context);
    void disconnect(const size_t id);

private:
    Conductor() : iohandler(new IOHandler()) { }
    std::shared_ptr<IOHandler> iohandler;

public:
    Conductor(Conductor const &) = delete;
    void operator=(Conductor const &) = delete;
};

#endif // CONDUCTOR_H
