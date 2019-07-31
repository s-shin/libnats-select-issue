cmake_minimum_required(VERSION 3.0)

set(app libnats-select-issue)
project(${app} CXX)

set(deps_install_dir "${CMAKE_CURRENT_LIST_DIR}/tmp-deps")

find_library(libnats nats_static HINTS "${deps_install_dir}/lib")

file(GLOB_RECURSE source_files src/*.cc)
add_executable(${app} ${source_files})
target_compile_options(${app} PUBLIC -std=c++11 -Wall -Werror -Wextra)
target_include_directories(${app} PUBLIC "${deps_install_dir}/include")
target_link_libraries(${app} "${libnats}")
if(UNIX AND NOT APPLE)
  target_link_libraries(${app} pthread)
endif()
