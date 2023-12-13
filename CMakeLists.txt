cmake_minimum_required(VERSION 3.26)
project(WeatherP C)

set(CMAKE_C_STANDARD 11)

# Find packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONC REQUIRED json-c)

# Include and link directories
include_directories(${JSONC_INCLUDE_DIRS})
link_directories(${JSONC_LIBRARY_DIRS})

add_executable(WeatherP main.c)
target_link_libraries(WeatherP ${JSONC_LIBRARIES})

# Find CURL
find_package(CURL REQUIRED)
if(CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    target_link_libraries(WeatherP ${CURL_LIBRARIES})
endif()