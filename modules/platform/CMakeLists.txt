# modules/platform/CMakeLists.txt

file(GLOB_RECURSE PLATFORM_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/private_include/*.hpp
)

add_library(platform STATIC
    ${PLATFORM_SOURCES}
)

target_include_directories(platform
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/private_include
)

# core — header-only, но как target, чтобы протащить include/
target_link_libraries(platform
    PUBLIC
        core
        Qt6::Core
)
