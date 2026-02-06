cmake_minimum_required(VERSION 3.16)

project(revise VERSION 1.3.0 LANGUAGES CXX)

# set QTP0004 behavior to NEW so extra qmldir files are generated for subdirs
if (QT_KNOWN_POLICY_QTP0004)
  qt_policy(SET QTP0004 NEW)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

qt_add_qml_module(apprevise
    URI Revise
    VERSION 1.0
    QML_FILES ${QML_FILES}
    NO_PLUGIN
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_link_libraries(apprevise PRIVATE
      Qt6::Quick
      Qt6::Sql
      Qt6::Concurrent
      Qt6::WebView
      Qt6::Qml
      Boost::stacktrace_basic
      core
      platform
      ui
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
      core
      platform
      ui
  )

  target_include_directories(apprevise PRIVATE
      include
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

