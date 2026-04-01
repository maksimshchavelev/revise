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

target_link_libraries(unit_tests
    gtest_main
    core
    utils
)

include(GoogleTest)
gtest_add_tests(TARGET unit_tests)

