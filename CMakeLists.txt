cmake_minimum_required(VERSION 3.16)

project(revise VERSION 1.3.0 LANGUAGES CXX)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml" CACHE PATH "Install path for QML" FORCE)
set(QML_IMPORT_PATH ${CMAKE_BINARY_DIR}/qml CACHE STRING "Extra QML Import Paths" FORCE)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

set(CMAKE_INSTALL_PREFIX ./AppDir)

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

# install

install(TARGETS apprevise RUNTIME DESTINATION usr/bin)

if(UNIX AND NOT APPLE)
    install(FILES deploy/linux/revise.desktop DESTINATION usr/share/applications)
    install(FILES deploy/linux/revise.png DESTINATION usr/share/icons/hicolor/256x256/apps)
    install(FILES deploy/linux/copyright DESTINATION usr/share/doc/libc6)
endif()

install(DIRECTORY ${CMAKE_PREFIX_PATH}/resources/ DESTINATION usr/resources)
install(DIRECTORY ${CMAKE_PREFIX_PATH}/translations/qtwebengine_locales/ DESTINATION usr/translations/qtwebengine_locales)
install(PROGRAMS ${CMAKE_PREFIX_PATH}/libexec/QtWebEngineProcess DESTINATION usr/libexec)

# build appimage (for linux)
if (UNIX AND NOT APPLE)
    add_custom_target(appimage
        COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR}

        COMMAND ${LINUXDEPLOYQT} AppDir/usr/share/applications/revise.desktop -qmldir=AppDir/qml -appimage -verbose=1 -unsupported-allow-new-glibc

        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS apprevise
        USES_TERMINAL
        COMMENT "Build AppImage"
    )
endif()

