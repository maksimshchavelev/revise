# modules/ui/CMakeLists.txt

# ====== Find sources, resources and qmls

file(GLOB_RECURSE UI_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/private_include/*.hpp
)

file(GLOB_RECURSE QML_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.qml"
    "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.js"
)

file(GLOB_RECURSE ABS_RESOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/res/*)

set(RESOURCES)
foreach(abs_file ${ABS_RESOURCES})
    file(RELATIVE_PATH rel_file
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${abs_file})
    list(APPEND RESOURCES ${rel_file})
endforeach()

# ==========

set_source_files_properties(
    qml/theme/Theme.qml
    qml/theme/Fonts.qml
    qml/navigation/Router.qml
    qml/debug/Debug.qml

    PROPERTIES
        QT_QML_SINGLETON_TYPE TRUE)

add_library(ui SHARED
    ${UI_SOURCES}
)

set_target_properties(ui PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

target_compile_definitions(ui PRIVATE UI_BUILD_SHARED)

qt_add_qml_module(ui
    URI Revise
    VERSION 1.0
    SOURCES
        ${UI_SOURCES}
    QML_FILES
        ${QML_FILES}
)

qt_add_resources(ui
    PREFIX "/"
    FILES
        ${RESOURCES}
        ${QML_FILES}
)

target_include_directories(ui
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/private_include
        ${CMAKE_CURRENT_LIST_DIR}/include/ui
)

target_link_libraries(ui
    PUBLIC
        core
        platform
        utils
        Qt6::Core
        Qt6::Qml
        Qt6::Gui
)

if (UNIX AND NOT APPLE)
    install(TARGETS ui
        LIBRARY DESTINATION usr/lib
        RUNTIME DESTINATION usr/bin
    )
endif()

if (WIN32)
    install(TARGETS ui
        LIBRARY DESTINATION .
        RUNTIME DESTINATION .
    )
endif()

install(DIRECTORY qml
    DESTINATION qml
)
