
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/private_include/*.hpp
)

add_library(engine STATIC
    ${ENGINE_SOURCES}
)

target_include_directories(engine
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/private_include
)

target_link_libraries(engine
    PUBLIC
        core
        utils
        Qt6::Core
)

set_target_properties(engine PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

install(TARGETS engine
    LIBRARY DESTINATION usr/lib
    RUNTIME DESTINATION usr/bin
    ARCHIVE DESTINATION usr/lib
)

