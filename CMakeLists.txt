cmake_minimum_required(VERSION 3.0.0)

project(chat)

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -W -Werror -fPIC -g")

set(SOURCE_FILES main.cpp chat.cpp)
add_executable(chat ${SOURCE_FILES})

# <------------ add hiredis dependency here --------------->
find_path(HIREDIS_HEADER hiredis)
target_include_directories(chat PUBLIC ${HIREDIS_HEADER})

find_library(HIREDIS_LIB hiredis)
target_link_libraries(chat ${HIREDIS_LIB} pthread)

# <------------ add redis-plus-plus dependency here -------------->
find_path(REDIS_PLUS_PLUS_HEADER sw)      # NOTE: this should be *sw* NOT *redis++*
target_include_directories(chat PUBLIC ${REDIS_PLUS_PLUS_HEADER})

find_library(REDIS_PLUS_PLUS_LIB redis++)
target_link_libraries(chat ${REDIS_PLUS_PLUS_LIB})