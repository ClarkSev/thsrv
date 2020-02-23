
# 代码编译
cmake_minimum_required(VERSION 2.6)
# setup the project
project(thsrv_test)

# CMAKE_CURRENT_SOURCE_DIR is the absoluted path of CMakeLists.txt

# set(PROJECT_SOURCE_DIR build)  # thsrv/base
# set(CMAKE_CURRENT_SOURCE_DIR ${PROJECT_BINARY_DIR})
set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# setup the compile mode.
set(CXX_FLAGS
 -g
 # -DVALGRIND
 # -DCHECK_PTHREAD_RETURN_VALUE
 # -D_FILE_OFFSET_BITS=64
 -Wall
 -Wextra
 # -Werror
 -Wconversion
 -Wno-unused-parameter
 -Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
 -Wshadow
 -Wwrite-strings
 -march=native
 # -MMD
 -std=c++11
 -rdynamic
 )
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
 
find_package(Boost REQUIRED)

# file(GLOB HEADERS "${PROJECT_INC_DIR}/*.h")

include_directories(
	${PROJECT_SOURCE_DIR}
	${Boost_INCLUDE_DIRS}
)

add_subdirectory(thsrv/base)
add_subdirectory(thsrv/net)
add_subdirectory(test)

# message("--------------------PROJECT-----------------")
# message("PROJECT_SOURCE_DIR = " ${PROJECT_SOURCE_DIR})
# message("PROJECT_BINARY_DIR = " ${PROJECT_BINARY_DIR})

# message("--------------------CMAKE-----------------")
# message("CMAKE_CURRENT_SOURCE_DIR = " ${CMAKE_CURRENT_SOURCE_DIR})
# message("CMAKE_CURRENT_BINARY_DIR = " ${CMAKE_CURRENT_BINARY_DIR})
# message("CMAKE_CURRENT_LIST_DIR = " ${CMAKE_CURRENT_LIST_DIR})
# message("EXECUTABLE_OUTPUT_PATH = " ${EXECUTABLE_OUTPUT_PATH})
# message("LIBRARY_OUTPUT_PATH = " ${LIBRARY_OUTPUT_PATH})

# message("HEADERS = " ${HEADERS})

# setup the target
# add_executable(logger_test ${TEST_SOURCE_DIR}/logger_test.cpp ${SRC_LIST})
# target_link_libraries(logger_test rt)
