cmake_minimum_required(VERSION 3.31)
project(client)

set(CMAKE_CXX_STANDARD 17)

add_executable(client
        main.cpp
        db_controller.h
        db_controller.cpp
        address_list.h
        address_list.cpp
        config.h.in)

find_package(mongocxx REQUIRED)
find_package(bsoncxx REQUIRED)

target_include_directories(client PRIVATE /opt/homebrew/include)

target_link_libraries(client PRIVATE
        mongo::mongocxx_shared
        mongo::bsoncxx_shared
)

find_package(CURL REQUIRED)
target_link_libraries(client PRIVATE CURL::libcurl)

if(EXISTS "${CMAKE_SOURCE_DIR}/secrets.cmake")
    include("${CMAKE_SOURCE_DIR}/secrets.cmake")
else()
    message(FATAL_ERROR "Missing secrets.cmake. Please create one with required values.")
endif()

configure_file(
        ${CMAKE_SOURCE_DIR}/config.h.in
        ${CMAKE_BINARY_DIR}/config.h
)
include_directories(${CMAKE_BINARY_DIR})