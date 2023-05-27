#include "conductor.h"

Conductor &Conductor::instance()
{
    static Conductor instance;
    return instance;
}

size_t Conductor::connect(const size_t bulkSize)
{
    return iohandler->connect(bulkSize);
}

void Conductor::recieve(std::string &&buf, const size_t context)
{
    iohandler->recieve(std::move(buf), context);
}

void Conductor::disconnect(const size_t id)
{
    iohandler->disconnect(id);
}
