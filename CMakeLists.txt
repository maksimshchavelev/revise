cmake_minimum_required(VERSION 3.16)

project(revise VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Sql Concurrent)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp*
    ${CMAKE_CURRENT_LIST_DIR}/include/*.tpp*)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(apprevise ${SOURCES})

set_source_files_properties(
    ui/theme/Theme.qml
    ui/theme/Fonts.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(apprevise
    URI revise
    VERSION 1.0
    QML_FILES
        ui/Main.qml

        # Components
        ui/components/GlassRectangle.qml
        ui/components/StatefulIndicator.qml
        ui/components/Button.qml
        ui/components/Toggle.qml
        ui/components/ColorfulProgressbar.qml
        ui/components/ToolButton.qml
        ui/components/ToolBar.qml
        ui/components/StreakIndicator.qml
        ui/components/InfoPopup.qml
        ui/components/AddDeckPopup.qml
        ui/components/Deck.qml
        ui/components/AppText.qml
        ui/components/TrainingBar.qml
        ui/components/ScrollableInfoPopup.qml
        ui/components/CheckupableOption.qml
        ui/components/Checkbox.qml
        ui/components/DeckModalDialog.qml

        # Pages
        ui/pages/Home.qml
        ui/pages/Statistics.qml
        ui/pages/Training.qml

        # Theme
        ui/theme/Theme.qml
        ui/theme/Fonts.qml

)

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
        res/fonts/anonymous_pro/AnonymousPro-Bold.ttf
        res/fonts/anonymous_pro/AnonymousPro-BoldItalic.ttf
        res/fonts/anonymous_pro/AnonymousPro-Italic.ttf
        res/fonts/anonymous_pro/AnonymousPro-Regular.ttf

        # Qml files
        ui/theme/Theme.qml
)

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

set(BOOST_ENABLE_CMAKE ON)
set(BOOST_INCLUDE_LIBRARIES stacktrace headers config)
set(BOOST_EXCLUDE_LIBRARIES fiber serialization graph wave stacktrace_test)

FetchContent_Declare(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.84.0
)
FetchContent_MakeAvailable(Boost)

# add_subdirectory(${CMAKE_BINARY_DIR}/_deps/zstd-src/build/cmake)
# target_include_directories(zstd PUBLIC ${CMAKE_BINARY_DIR}/_deps/zstd-src/)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(apprevise PROPERTIES
#   MACOSX_BUNDLE_GUI_IDENTIFIER com.example.apprevise
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

set_property(TARGET apprevise APPEND PROPERTY
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
)

target_link_libraries(apprevise PRIVATE
    Qt6::Quick
    Qt6::Sql
    Qt6::Concurrent
    QuaZip::QuaZip
    Boost::stacktrace_basic
    libzstd_static
)

target_include_directories(apprevise PRIVATE
    include
    ${zstd_SOURCE_DIR}/lib
    Boost::stacktrace_basic
)

target_compile_definitions(apprevise PRIVATE BOOST_STACKTRACE_USE_BACKTRACE)

include(GNUInstallDirs)
install(TARGETS apprevise
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
