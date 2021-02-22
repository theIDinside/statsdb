cmake_minimum_required(VERSION 3.17)
project(statsdb LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/statsdb_bin)
set(STATSDB_LIB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

option(JSON_BuildTests OFF) # We don't want to build the damn tests. It takes 5 times as long as building my project.
set(JSON_BuildTests OFF CACHE INTERNAL "")
# For deserializing json data

# So we can see progress of FetchContent
Set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
        GIT_TAG master
        GIT_PROGRESS TRUE
)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG master
        GIT_PROGRESS TRUE
)

FetchContent_GetProperties(json)
if (NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_GetProperties(fmt)
if (NOT fmt_POPULATED)
   FetchContent_Populate(fmt)
   add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
endif ()

add_library(statsdb
        src/game_data.cpp
        src/calendar_date.cpp
        src/deserialize.cpp
        src/deserialize.hpp
        src/db.cpp
        src/stats.cpp

        include/statsdb/statsdb.h
        include/statsdb/defs.hpp
        include/statsdb/game_data.hpp
        include/statsdb/game_info.hpp
        include/statsdb/calendar_date.hpp
        include/statsdb/decimal_number.hpp
        include/statsdb/db.hpp
        include/statsdb/stats.hpp
        include/statsdb/utils.hpp
        include/statsdb/enumerate.hpp
        )

target_precompile_headers(
        statsdb
        PUBLIC
        <string>
        <string_view>
        <filesystem>
        <optional>
        <vector>
        <algorithm>
        <ranges>
        <span>
        <numeric>
        <cassert>
        <map>
        <set>
)

target_link_libraries(statsdb nlohmann_json::nlohmann_json fmt)
target_include_directories(
        statsdb
        INTERFACE
        $<BUILD_INTERFACE:${STATSDB_LIB_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(statsdb PUBLIC cxx_std_20)
set_target_properties(statsdb PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED TRUE
        CXX_EXTENSIONS FALSE
)

# flags taken from; https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md
set(CLANG_FLAGS -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wconversion -Wsign-conversion -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op
        -Wnull-dereference -Wuseless-cast -Wdouble-promotion -Wformat=2 -Wlifetime)

set(GCC_FLAGS ${CLANG_FLAGS})
set(MSVC_FLAGS /W4 /w14242 /w14254 /w14263 /w14265 /w14287 /we4289 /w14296 /w14311 /w14545 /w14546 /w14547 /w14549 /w14555 /w14619 /w14640 /w14826 /w14905 /w14906 /w14928)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    message("You are using CLANG. Setting CLANG flags: ${CLANG_FLAGS}")
    target_compile_options(statsdb PUBLIC ${CLANG_FLAGS})
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message("You are using GCC. Setting GCC flags: ${GCC_FLAGS}")
    target_compile_options(statsdb PUBLIC ${GCC_FLAGS})
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message("You are using MSVC. Setting MSVC flags: ${MSVC_FLAGS}")
    target_compile_options(statsdb PUBLIC ${MSVC_FLAGS})
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Debug build set")
    add_compile_definitions(statsdb DEBUG)
endif()

add_executable(test_deserialize tests/deserialize.cpp)
target_link_libraries(test_deserialize statsdb)
add_subdirectory(benchmark)