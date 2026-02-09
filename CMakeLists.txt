cmake_minimum_required(VERSION 3.16)

project(revise VERSION 1.3.0 LANGUAGES CXX)

# set QTP0004 behavior to NEW so extra qmldir files are generated for subdirs
if (QT_KNOWN_POLICY_QTP0004)
  qt_policy(SET QTP0004 NEW)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

