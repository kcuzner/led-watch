cmake_minimum_required (VERSION 3.0.0)
project (wristwatch)

find_package(PkgConfig REQUIRED)
pkg_search_module(HIDAPI REQUIRED hidapi-hidraw)
pkg_search_module(GTKMM REQUIRED gtkmm-3.0)


file(GLOB_RECURSE SOURCES
    src/*.cpp
)

include_directories(include)
include_directories(${HIDAPI_INCLUDE_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})

link_directories(${GTKMM_LIBRARY_DIRS})

add_executable(wristwatch ${SOURCES})
target_link_libraries(wristwatch ${HIDAPI_LIBRARIES} ${GTKMM_LIBRARIES})

