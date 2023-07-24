cmake_minimum_required(VERSION 3.12)
project(Yunying)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O2 -lpthread")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/lua)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/curl)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/build/external/lua/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/sol2/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/curl/include)

file(GLOB SOURCES src/*.cpp)
list(FILTER SOURCES EXCLUDE REGEX "main.cpp$")
file(GLOB TEST_SOURCES test/*.cpp)

add_library(libYunying STATIC ${SOURCES})
target_include_directories(libYunying PUBLIC src)
add_executable(yunying src/main.cpp)

target_link_libraries(yunying libYunying lua::lib libcurl)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/b796f7d44681514f58a683a3a71ff17c94edb0c1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  yunying_test
  ${TEST_SOURCES}
)
target_link_libraries(
  yunying_test
  libYunying
  GTest::gtest_main
  lua
)

include(GoogleTest)
gtest_discover_tests(yunying_test)
