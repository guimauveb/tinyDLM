cmake_minimum_required(VERSION 3.10)
project(tinyDLM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(${CURSES_INCLUDE_DIR})

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

if("${CMAKE_SYSTEM_NAME}" MATCHES Linux)
    target_link_libraries(tinyDLM pthread)
endif()

target_link_libraries(tinyDLM curl)
target_link_libraries(tinyDLM curlpp)
target_link_libraries(tinyDLM ncurses)
target_link_libraries(tinyDLM menu)
target_link_libraries(tinyDLM form)
