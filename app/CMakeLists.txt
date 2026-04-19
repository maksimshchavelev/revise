
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp*
    ${CMAKE_CURRENT_LIST_DIR}/include/*.tpp*)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(apprevise ${SOURCES})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_link_libraries(apprevise PRIVATE
      Qt6::Quick
      Qt6::Sql
      Qt6::Concurrent
      Qt6::WebView
      Qt6::Qml
      Qt6::Core
      core
      platform
      ui
      io
      utils
      engine
  )

  target_include_directories(apprevise PRIVATE
      include
      ${zstd_SOURCE_DIR}/lib
  )
else() # Release
  target_link_libraries(apprevise PRIVATE
      Qt6::Quick
      Qt6::Sql
      Qt6::Concurrent
      Qt6::WebView
      Qt6::Qml
      Qt6::Core
      core
      platform
      ui
      io
      utils
      engine
  )

  target_include_directories(apprevise PRIVATE
      include
  )
endif()

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

# install

if (ANDROID)
  install(TARGETS apprevise
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
endif()

if(UNIX AND NOT APPLE AND NOT ANDROID)
    install(TARGETS apprevise RUNTIME DESTINATION usr/bin)

    install(FILES ${CMAKE_SOURCE_DIR}/deploy/linux/revise.desktop DESTINATION usr/share/applications)
    install(FILES ${CMAKE_SOURCE_DIR}/deploy/linux/revise.png DESTINATION usr/share/icons/hicolor/256x256/apps)
    install(FILES ${CMAKE_SOURCE_DIR}/deploy/linux/copyright DESTINATION usr/share/doc/libc6)

    install(DIRECTORY ${CMAKE_PREFIX_PATH}/resources/ DESTINATION usr/resources)
    install(DIRECTORY ${CMAKE_PREFIX_PATH}/translations/qtwebengine_locales/ DESTINATION usr/translations/qtwebengine_locales)
    install(PROGRAMS ${CMAKE_PREFIX_PATH}/libexec/QtWebEngineProcess DESTINATION usr/libexec)
endif()

if(WIN32)
    install(TARGETS apprevise RUNTIME DESTINATION .)
endif()
