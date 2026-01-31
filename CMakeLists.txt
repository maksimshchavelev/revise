cmake_minimum_required(VERSION 3.16)

project(revise VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QML_IMPORT_PATH "${CMAKE_BINARY_DIR}/qml/modules/Revise" CACHE STRING "QML import path for Qt Creator" FORCE)
set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/modules/Revise" CACHE PATH "Extra QML import paths for IDE" FORCE)

find_package(Qt6 REQUIRED COMPONENTS
  Quick
  Sql
  Concurrent
  WebView
  Qml)

# include modules
add_subdirectory(modules/core)     # Core module
add_subdirectory(modules/platform) # Platform module

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp*
    ${CMAKE_CURRENT_LIST_DIR}/include/*.tpp*)

file(GLOB_RECURSE ABS_RESOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/res/*)

set(RESOURCES)
foreach(abs_file ${ABS_RESOURCES})
    file(RELATIVE_PATH rel_file
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${abs_file})
    list(APPEND RESOURCES ${rel_file})
endforeach()

qt_standard_project_setup(REQUIRES 6.8)

include(FetchContent)

set(QUAZIP_QT_MAJOR_VERSION 6 CACHE STRING "" FORCE)
set(QUAZIP_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(QUAZIP_ENABLE_TOOLS OFF CACHE BOOL "" FORCE)

# quazip
FetchContent_Declare(quazip
    GIT_REPOSITORY https://github.com/stachenov/quazip.git
    GIT_TAG v1.5
)
FetchContent_MakeAvailable(quazip)

# zstd
FetchContent_Declare(
  zstd
  GIT_REPOSITORY https://github.com/facebook/zstd.git
  GIT_TAG v1.5.5
  SOURCE_SUBDIR build/cmake
)
FetchContent_MakeAvailable(zstd)
get_target_property(ZSTD_SOURCE_DIR libzstd_static SOURCE_DIR)

# Boost stacktrace
set(BOOST_ENABLE_CMAKE ON)
set(BOOST_INCLUDE_LIBRARIES stacktrace headers config)
set(BOOST_EXCLUDE_LIBRARIES fiber serialization graph wave stacktrace_test)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  FetchContent_Declare(
      Boost
      GIT_REPOSITORY https://github.com/boostorg/boost.git
      GIT_TAG boost-1.84.0
  )
  FetchContent_MakeAvailable(Boost)
endif()

file(GLOB_RECURSE QML_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.qml"
)

qt_add_executable(apprevise ${SOURCES})

set_source_files_properties(
    ui/theme/Theme.qml
    ui/theme/Fonts.qml
    ui/navigation/Router.qml
    ui/debug/Debug.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(apprevise_qml
    URI Revise
    VERSION 1.0
    QML_FILES ${QML_FILES}
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/modules/Revise
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_link_libraries(apprevise PRIVATE
      Qt6::Quick
      Qt6::Sql
      Qt6::Concurrent
      Qt6::WebView
      Qt6::Qml
      QuaZip::QuaZip
      Boost::stacktrace_basic
      libzstd_static
      core
      platform
      apprevise_qml
  )

  target_include_directories(apprevise PRIVATE
      include
      ${zstd_SOURCE_DIR}/lib
      Boost::stacktrace_basic
  )
else() # Release
  target_link_libraries(apprevise PRIVATE
      Qt6::Quick
      Qt6::Sql
      Qt6::Concurrent
      Qt6::WebView
      Qt6::Qml
      QuaZip::QuaZip
      libzstd_static
      core
      platform
      apprevise_qml
  )

  target_include_directories(apprevise PRIVATE
      include
      ${zstd_SOURCE_DIR}/lib
  )
endif()

qt_add_resources(apprevise
    PREFIX "/"
    FILES
        res/img/background_blurred.png
        res/img/add_deck.png
        res/img/clocks.png
        res/img/decks.png
        res/img/decks_selected.png
        res/img/statistics.png
        res/img/statistics_selected.png
        res/img/streak.png
        res/img/streak_updated.png
        res/img/checkbox_checked.png

        # fonts
        ${RESOURCES}
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(apprevise PROPERTIES
#   MACOSX_BUNDLE_GUI_IDENTIFIER com.example.apprevise
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    QT_ANDROID_EXTRA_LIBS
        "${ANDROID_SDK_ROOT}/android_openssl/ssl_3/${CMAKE_ANDROID_ARCH_ABI}/libcrypto_3.so;${ANDROID_SDK_ROOT}/android_openssl/ssl_3/${CMAKE_ANDROID_ARCH_ABI}/libssl_3.so"
)

if(ANDROID)
  message(INFO "Building for android")

  include(${ANDROID_SDK_ROOT}/android_openssl/CMakeLists.txt)

  set_property(TARGET apprevise APPEND PROPERTY
      QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
  )
endif()

add_custom_command(TARGET apprevise POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "RELINK apprevise"
)

target_compile_definitions(apprevise PRIVATE BOOST_STACKTRACE_USE_BACKTRACE)

include(GNUInstallDirs)
install(TARGETS apprevise
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

