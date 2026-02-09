file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp*
    ${CMAKE_CURRENT_LIST_DIR}/include/*.tpp*)

add_library(core INTERFACE ${SOURCES})

target_include_directories(core
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(core INTERFACE Qt6::Core)

configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/include/core/Version.hpp.in
    ${CMAKE_CURRENT_SOURCE_DIR}/include/core/Version.hpp
)
