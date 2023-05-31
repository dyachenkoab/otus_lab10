cmake_minimum_required(VERSION 3.16)

set(PROJECT_VERSION 0.1)

project(async_server VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost COMPONENTS system REQUIRED)
find_package(Threads REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_subdirectory(src)

#--------------------------------
add_executable(${PROJECT_NAME}
    src/asio_async_server.cpp
    src/asio_async_server.h
)
target_link_libraries(${PROJECT_NAME} PUBLIC
        ${Boost_LIBRARIES}
        Threads::Threads
        packer
)

add_executable(asio_client src/asio_client.cpp)
target_link_libraries(asio_client PUBLIC
        ${Boost_LIBRARIES}
        Threads::Threads
)
#--------------------------------

