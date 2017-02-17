cmake_minimum_required (VERSION 3.0.0)
project (wristwatch)

find_package(PkgConfig REQUIRED)
pkg_search_module(HIDAPI REQUIRED hidapi-libusb)

include_directories(include)

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_executable(wristwatch ${SOURCES})
target_link_libraries(wristwatch ${HIDAPI_LIBRARIES})
target_include_directories(wristwatch PUBLIC ${HIDAPI_INCLUDE_DIRS})
target_compile_options(wristwatch PUBLIC ${HIDAPI_CFLAGS_OTHER})

