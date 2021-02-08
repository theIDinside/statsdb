cmake_minimum_required(VERSION 3.17)
project(statsdb LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/statsdb_bin)
set(STATSDB_LIB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

include(FetchContent)
option(JSON_BuildTests OFF) # We don't want to build the damn tests. It takes 5 times as long as building my project.
# For deserializing json data
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG master
        SOURCE_DIR ${PROJECT_SOURCE_DIR}/deps/fmt
)
if (NOT fmt_POPULATED)
    FetchContent_Populate(fmt)
    add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
endif ()

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG master
        SOURCE_DIR ${PROJECT_SOURCE_DIR}/deps/json)

if (NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR})
endif()

FetchContent_MakeAvailable(json fmt)

add_library(statsdb
        src/game_data.cpp
        src/calendar_date.cpp
        src/deserialize.cpp
        src/deserialize.hpp
        src/db.cpp

        include/statsdb/statsdb.h
        include/statsdb/defs.hpp
        include/statsdb/game_data.hpp
        include/statsdb/calendar_date.hpp
        include/statsdb/db.hpp
        )

target_link_libraries(statsdb nlohmann_json::nlohmann_json fmt)

target_include_directories(
        statsdb
        INTERFACE
        $<BUILD_INTERFACE:${STATSDB_LIB_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)

add_executable(test_deserialize tests/deserialize.cpp)
target_link_libraries(test_deserialize statsdb)