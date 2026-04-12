
file(GLOB_RECURSE IO_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/private_include/*.hpp
)

include(FetchContent)

set(QUAZIP_QT_MAJOR_VERSION 6 CACHE STRING "" FORCE)
set(QUAZIP_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(QUAZIP_ENABLE_TOOLS OFF CACHE BOOL "" FORCE)
set(QUAZIP_INSTALL OFF CACHE BOOL "" FORCE)

# quazip
FetchContent_Declare(quazip
    GIT_REPOSITORY https://github.com/stachenov/quazip.git
    GIT_TAG v1.5
)
FetchContent_MakeAvailable(quazip)

set(ZSTD_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
set(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "" FORCE)
set(ZSTD_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(ZSTD_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(ZSTD_BUILD_TESTS OFF CACHE BOOL "" FORCE)

# zstd
FetchContent_Declare(
  zstd
  GIT_REPOSITORY https://github.com/facebook/zstd.git
  GIT_TAG v1.5.5
  SOURCE_SUBDIR build/cmake
)
FetchContent_MakeAvailable(zstd)
get_target_property(ZSTD_SOURCE_DIR libzstd_static SOURCE_DIR)


add_library(io STATIC
    ${IO_SOURCES}
)

target_include_directories(io
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/private_include
        ${zstd_SOURCE_DIR}/lib
)

target_link_libraries(io
    PUBLIC
        core
        utils
        libzstd_static
        QuaZip::QuaZip
        Qt6::Core
        Qt6::Qml
        Qt6::Gui
        Qt6::Sql
)

set_target_properties(io PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

if (UNIX AND NOT APPLE)
    install(TARGETS QuaZip
        LIBRARY DESTINATION usr/lib
        RUNTIME DESTINATION usr/bin
    )
endif()

if (WIN32)
    install(TARGETS QuaZip
        LIBRARY DESTINATION .
        RUNTIME DESTINATION .
    )
endif()

