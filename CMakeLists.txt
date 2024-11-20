cmake_minimum_required(VERSION 3.27)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_BUILD_TYPE Debug)

#standard
set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

# set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# msvc
add_compile_options(/W4)
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/execution-charset:utf-8>")

# gcc
# add_compile_options(-Wall -Wextra -Wpedantic)

project(test LANGUAGES C CXX)

file(GLOB_RECURSE SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE INCLUDE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

include_directories(include)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${INCLUDE_LIST})

find_package(EnTT REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE EnTT::EnTT)

find_package(proxy REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE msft_proxy)

find_package(Boost REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE boost::boost)

find_package(pugixml REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE pugixml::pugixml)

# get_target_property(include_dirs ${PROJECT_NAME} INTERFACE_INCLUDE_DIRECTORIES)
