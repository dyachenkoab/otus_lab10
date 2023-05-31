#ifndef ASYNC_H
#define ASYNC_H
#include <thread>
#include <string>

namespace packer {
size_t connect(const size_t bulkSize);
void recieve(std::string &&buf, const size_t context);
void disconnect(const size_t id);
} // namespace packer

#endif // ASYNC
