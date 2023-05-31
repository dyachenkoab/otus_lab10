cmake_minimum_required(VERSION 3.16)

set(PROJECT_VERSION 0.1)

project(packer VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME}
    PRIVATE
    src/observer.h
    src/observer.cpp
    src/iprocessor.h
    src/iprocessor.cpp
    src/commandprocessor.h
    src/commandprocessor.cpp
    src/printers.h
    src/printers.cpp
    src/iohandler.h
    src/iohandler.cpp
    src/conductor.cpp
    src/conductor.h
    src/async.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -pedantic
    -Werror
)

target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
        # where top-level project will look for the library's public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        # where external projects will look for the library's public headers
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

set(public_header include/async.h)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${public_header}")

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION "/usr/lib"
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)


# generate and install export file
install(EXPORT "${PROJECT_NAME}Targets"
    FILE "${PROJECT_NAME}Targets.cmake"
    NAMESPACE packer::
    DESTINATION cmake
)

# generate the version file for the config file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY AnyNewerVersion
)
# create config file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION cmake
)
# install config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION cmake
)
# generate the export targets for the build tree
export(EXPORT "${PROJECT_NAME}Targets"
    FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Targets.cmake"
    NAMESPACE packer::
)

