# modules/platform/CMakeLists.txt

file(GLOB_RECURSE UI_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/private_include/*.hpp
)

add_library(ui STATIC
    ${UI_SOURCES}
)

target_include_directories(ui
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/private_include
)

target_link_libraries(ui
    PUBLIC
        core
        Qt6::Core
        Qt6::Qml
        Qt6::Gui
)

set_target_properties(ui PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

