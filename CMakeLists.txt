cmake_minimum_required(VERSION 3.12)
project(Yunying)

set(CMAKE_CXX_STANDARD 20)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O2 -lpthread")

# Include directories
include_directories(
    src/third_party/sol
    /usr/local/include
    /usr/local/bin
)

# Source files
file(GLOB SOURCES src/*.cpp)

# Executable target
add_executable(yunying ${SOURCES})

# Link libraries
target_link_libraries(yunying lua)