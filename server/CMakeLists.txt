cmake_minimum_required(VERSION 3.21)
project(server)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES
        main.cpp
        db_controller.cpp
        ngrok_controller.cpp
        config.h.in
)

add_executable(server ${SOURCES})

include(FetchContent)

find_package(CURL REQUIRED)
find_package(mongocxx REQUIRED)
find_package(bsoncxx REQUIRED)
find_package(Crow REQUIRED)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

target_include_directories(server PRIVATE /opt/homebrew/include)

target_link_libraries(server PRIVATE
        CURL::libcurl
        nlohmann_json::nlohmann_json
        pthread
        dl
        mongo::mongocxx_shared
        mongo::bsoncxx_shared
        Crow::Crow
)

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