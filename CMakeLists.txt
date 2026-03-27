cmake_minimum_required(VERSION 3.16)

project(revise VERSION 1.3.0 LANGUAGES CXX)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml" CACHE PATH "Install path for QML" FORCE)
set(QML_IMPORT_PATH ${CMAKE_BINARY_DIR}/qml CACHE STRING "Extra QML Import Paths" FORCE)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

option(REVISE_BUILD_TESTS "Need to build tests?" ON)
option(REVISE_BUILD_APP "Need to build revise application?" ON)

# Versions
set(REVISE_DB_SCHEMA_VERSION 2)

find_package(Qt6 REQUIRED COMPONENTS
  Quick
  Sql
  Concurrent
  WebView
  Qml)

# include modules
add_subdirectory(modules/core)     # Core module
add_subdirectory(modules/platform) # Platform module
add_subdirectory(modules/ui)       # UI module
add_subdirectory(modules/io)       # IO module
add_subdirectory(modules/utils)    # utils module
add_subdirectory(modules/engine)   # engine module

if (${REVISE_BUILD_TESTS})
    message(INFO "Tests enabled")
    add_subdirectory(tests) # tests
endif()

if (${REVISE_BUILD_APP})
    message(INFO "Building app")
    add_subdirectory(app) # app
endif()

# run clang-tidy
add_custom_target(clang-check
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run-clang-checks.sh
            -s app,modules
            -b ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running clang-tidy checks on source files"
)

# run cppcheck
add_custom_target(cppcheck
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run-cppcheck.sh
            -s app,modules
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running cppcheck on source files"
)

