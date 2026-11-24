# modules/network/CMakeLists.txt

file(GLOB_RECURSE NETWORK_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp
        ${CMAKE_CURRENT_LIST_DIR}/private_include/*.hpp
)

add_library(network STATIC ${NETWORK_SOURCES})

target_include_directories(network
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/private_include
)

target_link_libraries(network
    PUBLIC
        core
        Qt6::Core
        Qt6::Network
)

set_target_properties(network PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        AUTOUIC ON
)
