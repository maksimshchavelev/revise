
file(GLOB_RECURSE UTILS_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/private_include/*.hpp
)

add_library(utils STATIC
    ${UTILS_SOURCES}
)

target_include_directories(utils
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/private_include
)

target_link_libraries(utils
    PRIVATE
    Qt6::Core
)

install(TARGETS utils
    LIBRARY DESTINATION usr/lib
    RUNTIME DESTINATION usr/bin
    ARCHIVE DESTINATION usr/lib
)