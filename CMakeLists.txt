cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
project(myshell CXX)
find_package(BISON 2.4.1)
find_package(FLEX 2.5.35)

BISON_TARGET(MyParser src/yacc.y ${CMAKE_CURRENT_BINARY_DIR}/yacc.yy.cpp)
FLEX_TARGET(MyScanner src/lex.l  ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.hpp)
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(include)

file(GLOB expansion_files ${PROJECT_SOURCE_DIR}/src/expansion/*.cpp)
file(GLOB ast_files ${PROJECT_SOURCE_DIR}/src/ast/*.cpp)
file(GLOB job_files ${PROJECT_SOURCE_DIR}/src/job_control/*.cpp)
file(GLOB built_in_files ${PROJECT_SOURCE_DIR}/src/built_ins/*.cpp)
file(GLOB shell_util_files ${PROJECT_SOURCE_DIR}/src/shell_utils/*.cpp)
file(GLOB autocomplete_files ${PROJECT_SOURCE_DIR}/src/autocomplete/*.cpp)
file(GLOB read_line_files ${PROJECT_SOURCE_DIR}/src/read_line/*.cpp)
add_executable(myshell
    ${BISON_MyParser_OUTPUTS}
    ${FLEX_MyScanner_OUTPUTS}
    ${expansion_files}
    ${ast_files}
    ${job_files}
    ${built_in_files}
    ${shell_util_files}
    ${autocomplete_files}
    ${read_line_files}
    src/shell_main.cpp
    src/misc.cpp
    )
target_precompile_headers(myshell PRIVATE include/shell.h include/shell_util.h)
install(TARGETS myshell DESTINATION bin)

