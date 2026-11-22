include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TESTS_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/unit/*.hpp*
    ${CMAKE_CURRENT_LIST_DIR}/unit/*.tpp*
    ${CMAKE_CURRENT_LIST_DIR}/unit/*.cpp)

add_executable(unit_tests
    ${TESTS_SOURCES}
)

target_link_libraries(unit_tests PRIVATE
    gtest
    core
    io
    engine
    utils
)

target_include_directories(unit_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/helpers"
)

include(GoogleTest)
gtest_add_tests(TARGET unit_tests)

add_custom_command(
        TARGET unit_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/testdata"
        "$<TARGET_FILE_DIR:unit_tests>/testdata"
        COMMENT "Copying testdata directory to binary output folder"
)
