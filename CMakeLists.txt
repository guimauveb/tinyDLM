cmake_minimum_required(VERSION 3.10)
project(tinyDLM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Append find_package modules 
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

message(STATUS "Looking for curses...")
find_package(Curses REQUIRED)

if (CURSES_FOUND)
    message(STATUS "System has curses")
    message(STATUS "Using curses version: ${CURSES_INCLUDE_DIR}")
    message(STATUS "Using curses libraries: ${CURSES_LIBRARIES}")
    list(APPEND tinyDLM_INCLUDE_DIRS ${CURSES_INCLUDE_DIR})
else()
    message(FATAL_ERROR "Could not find curses.")
    if("${CMAKE_SYSTEM_NAME}" MATCHES Linux)
        message(STATUS "Install libncurses with the following command:")
        message(STATUS "sudo apt-get install libncurses-dev")
        # TODO - installation command with sudo and homebrew (for instance)
    elseif ("${CMAKE_SYSTEM_NAME}" MATCHES Darwin)
        message(STATUS "Install libncurses with homebrew using the following command:")
        message(STATUS "brew install ncurses\n")
    endif()
endif()

if (CURSES_HAVE_NCURSES_H)
    message(STATUS "ncurses.h is available\n")
endif()

message(STATUS "Looking for curl...")
find_package(CURL REQUIRED)

if (CURL_FOUND)
    message(STATUS "Found curl version: ${CURL_VERSION_STRING}")
    message(STATUS "Using curl version: ${CURL_INCLUDE_DIRS}")
    message(STATUS "Using curl libraries: ${CURL_LIBRARIES}\n")
    list(APPEND tinyDLM_INCLUDE_DIRS ${CURL_INCLUDE_DIRS})
else()
    # TODO - installation command with sudo and homebrew (for instance)
    message(FATAL_ERROR "Could not find curl.")
    if("${CMAKE_SYSTEM_NAME}" MATCHES Linux)
        message(STATUS "Install libcurl with the following command:\n")
        message(STATUS "sudo apt-get install libcurl-dev")
    elseif ("${CMAKE_SYSTEM_NAME}" MATCHES Darwin)
        message(STATUS "Install libcurl with homebrew using the following command:")
        message(STATUS "brew install curl\n")
    endif()
endif()


# Added a 'FindcURLpp.cmake' alongside LibFindMacros.cmake in ./cmake 
message(STATUS "Looking for curlpp...")
find_package(CURLPP REQUIRED)

if (CURLPP_FOUND)
    message(STATUS "Found curlpp version: ${CURLPP_VERSION}")
    message(STATUS "Using curlpp include dir: ${CURLPP_INCLUDE_DIR}")
    message(STATUS "Using curlpp libraries: ${CURLPP_LIBRARY}\n")
    list(APPEND tinyDLM_INCLUDE_DIRS ${CURLPP_INCLUDE_DIR})
else()
    message(FATAL_ERROR "Could not find curlpp.")
    if("${CMAKE_SYSTEM_NAME}" MATCHES Linux)
        message(STATUS "Install libcurlpp with the following command:\n")
        message(STATUS "sudo apt-get install libcurlpp-dev")
    elseif ("${CMAKE_SYSTEM_NAME}" MATCHES Darwin)
        message(STATUS "Install curlpp with homebrew using the following command:")
        message(STATUS "brew install curlpp\n")
    endif()
endif()

# Get all files recursively
file(GLOB_RECURSE tinyDLM_SOURCES "./*.cxx")
file(GLOB_RECURSE tinyDLM_HEADERS "./*.hxx")

set (tinyDLM_INCLUDE_DIRS "")
foreach (_headerFile ${tinyDLM_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND tinyDLM_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES tinyDLM_INCLUDE_DIRS)

add_executable (tinyDLM ${tinyDLM_SOURCES})
target_include_directories(tinyDLM PRIVATE ${tinyDLM_INCLUDE_DIRS})

# Need -lpthread flag on Linux
if("${CMAKE_SYSTEM_NAME}" MATCHES Linux)
    target_link_libraries(tinyDLM pthread)
endif()

target_link_libraries(tinyDLM ${CURL_LIBRARIES})
target_link_libraries(tinyDLM ${CURLPP_LIBRARY})
target_link_libraries(tinyDLM ${CURSES_LIBRARIES})

# TODO - better way to add them ?
target_link_libraries(tinyDLM menu )
target_link_libraries(tinyDLM form )


