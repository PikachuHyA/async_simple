cmake_minimum_required(VERSION 3.26)
project(async_simple CXX C ASM)

set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  include(cxx_modules_rules_clang.cmake)
endif()

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

find_package(Aio REQUIRED)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)

if(LIBAIO_INCLUDE_DIR AND LIBAIO_LIBRARIES)
    message(STATUS "aio found")
    message(STATUS "aio: ${LIBAIO_INCLUDE_DIR}, ${LIBAIO_LIBRARIES}.")
else()
    message(FATAL_ERROR "aio NOT found")
endif()

if(GTEST_INCLUDE_DIR AND GTEST_LIBRARIES)
    message(STATUS "GTest found.")
    message(STATUS "GTest: ${GTEST_INCLUDE_DIR}, ${GTEST_LIBRARIES}")
else()
    message(FATAL_ERROR "GTest NOT found.")
endif()

if(GMOCK_INCLUDE_DIR AND GMOCK_LIBRARIES)
    message(STATUS "GMock found.")
    message(STATUS "GMock: ${GMOCK_INCLUDE_DIR}, ${GMOCK_LIBRARIES}")
else()
    message(FATAL_ERROR "GMock NOT found.")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# set CXXFALGS
set(CXX_FLAGS
    -std=c++20
    -D_GLIBCXX_USE_CXX11_ABI=1
    -Wno-deprecated-register
    -D_FILE_OFFSET_BITS=64
    -fPIC
    -Wall
    -Werror
    -Wno-read-modules-implicitly
    -D__STDC_LIMIT_MACROS
    -g
    )
add_link_options(
    -fuse-ld=lld 
)

set(HEADERS_PATH
    ${PROJECT_SOURCE_DIR}
    ${GTEST_INCLUDE_DIR}
    ${GMOCK_INCLUDE_DIR}
    ${AIO_INCLUDE_DIR}
    )
include_directories(${HEADERS_PATH})

set(deplibs)
list(APPEND deplibs ${LIBAIO_LIBRARIES})
list(APPEND deplibs pthread)
set(testdeplibs)
list(APPEND testdeplibs ${GTEST_LIBRARIES})
list(APPEND testdeplibs ${GMOCK_LIBRARIES})

if(CMAKE_BUILD_BITS EQUAL 32)
    message("-- Use flag -m32")
    list(APPEND CXX_FLAGS "-m32")
    list(APPEND CXX_FLAGS "-DTARGET_32")
    list(APPEND deplibs "-m32")
else()
    message("-- Use flag -m64")
    list(APPEND CXX_FLAGS "-m64")
    list(APPEND CXX_FLAGS "-DTARGET_64")
    #list(APPEND deplibs "-m64")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("-- Use flag -fsanitize=address")
    list(APPEND CXX_FLAGS "-fsanitize=address")
endif()

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

add_subdirectory(third_party_module/asio)
add_subdirectory(third_party_module/stdmodules)
add_subdirectory(async_simple_module)
add_subdirectory(demo_example_module)
