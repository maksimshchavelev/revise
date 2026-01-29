cmake_minimum_required(VERSION 3.16)

find_package(Qt6 REQUIRED COMPONENTS Core)

file(GLOB_RECURSE PLATFORM_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/private_include/*.hpp
)

# OBJECT library
add_library(platform_objects OBJECT ${PLATFORM_SOURCES})

target_include_directories(platform_objects
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private_include
)

target_link_libraries(platform_objects
    PUBLIC core
    PRIVATE Qt6::Core
)

add_library(platform_headers INTERFACE)

target_include_directories(platform_headers
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
