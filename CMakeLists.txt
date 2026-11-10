cmake_minimum_required(VERSION 3.16)

project(revise VERSION 1.4.0 LANGUAGES CXX)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml" CACHE PATH "Install path for QML" FORCE)
set(QML_IMPORT_PATH ${CMAKE_BINARY_DIR}/qml CACHE STRING "Extra QML Import Paths" FORCE)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(REVISE_BETA_BUILD "Is the Revise build an early access beta build?" FALSE)
option(REVISE_WIN32_EXECUTABLE "Build Revise on Windows as Win32 executable?" TRUE)

# Get hash of git last commit
find_package(Git QUIET)

if(GIT_FOUND)
    execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_COMMIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set_property(GLOBAL APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/.git/index")
    message(STATUS "Git commit hash: ${GIT_COMMIT_HASH}")
endif()
# ------------------------

if(LINUX OR UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2")
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG   "/Zi /Od")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()


if (LINUX OR UNIX AND NOT APPLE)
    set(CMAKE_INSTALL_PREFIX ./AppDir)
endif()

if (WIN32)
    set(CMAKE_INSTALL_PREFIX ./dist)
endif()

set(LINUXDEPLOYQT ~/linuxdeployqt-continuous-x86_64.AppImage CACHE FILEPATH "linuxdeployqt executable" FORCE)

if("${LINUXDEPLOYQT}" STREQUAL "" AND (UNIX AND NOT APPLE))
    message(FATAL_ERROR "Set the LINUXDEPLOYQT cmake variable. It must point to the path of the linux deploy qt executable file.")
endif()

option(REVISE_BUILD_TESTS "Need to build tests?" OFF)
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

# build appimage (for linux)
if (UNIX AND NOT APPLE)
    get_target_property(QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)

    add_custom_target(appimage
        COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR}

        COMMAND ${LINUXDEPLOYQT}
            AppDir/usr/share/applications/revise.desktop
            -qmldir=AppDir/qml
            -appimage
            -verbose=1
            -unsupported-allow-new-glibc
            -qmake=${QMAKE_EXECUTABLE}

        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS apprevise
        USES_TERMINAL
        COMMENT "Build AppImage"
    )
endif()

# build portable
if (WIN32)
    enable_language("RC")

    add_custom_target(portable
        COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR}

        COMMAND windeployqt --qmldir dist/qml --dir dist --libdir dist --compiler-runtime dist/apprevise.exe

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:zlib>
        ${CMAKE_BINARY_DIR}/dist

        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS apprevise
        USES_TERMINAL
        COMMENT "Build Windows Portable"
    )
endif()

