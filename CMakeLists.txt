CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
project(Vision2016)

SET(EXEC_NAME Vision2016)

# Set Cxx Standard for use.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set Cuda Static Runtime to be off
SET(CUDA_USE_STATIC_CUDA_RUNTIME OFF)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 OLD)
	cmake_policy(SET CMP0015 OLD)  
endif(COMMAND cmake_policy)

SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin")

find_package(OpenCV 2.4 COMPONENTS core highgui imgproc features2d REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

link_directories(${OpenCV_LIBRARY_DIRS})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

SET(SRC_FOLDER src)
SET(INCLUDE_FOLDER include)
FILE(GLOB_RECURSE SRC_FILES "${SRC_FOLDER}/*.cpp")
Set(SOURCE ${SRC_FOLDER}/main.cpp ${SRC_FOLDER}/GripPipeline.cpp)
Set(HEADERS ${INCLUDE_FOLDER}/GripPipeline.h ${INCLUDE_FOLDER}/WQueue.h ${INCLUDE_FOLDER}/CameraThread.h ${INCLUDE_FOLDER}/PipelineThread.h)






# Check and add warning flags
set(CLANG_WARNING_FLAGS -Wall -Wextra -pedantic)
set(GNU_WARNING_FLAGS -Wall -Wextra -pedantic)
add_compile_options("$<$<CXX_COMPILER_ID:Clang>:${CLANG_WARNING_FLAGS}>")
add_compile_options("$<$<CXX_COMPILER_ID:GNU>:${GNU_WARNING_FLAGS}>")

# Check and add compile flags
set(CLANG_COMPILER_FLAGS -O3)
set(GNU_COMPILER_FLAGS -O3)
add_compile_options("$<$<CXX_COMPILER_ID:Clang>:${CLANG_COMPILER_FLAGS}>")
add_compile_options("$<$<CXX_COMPILER_ID:GNU>:${GNU_COMPILER_FLAGS}>")

set(COMPILE_FLAGS -g)

add_executable(${EXEC_NAME} ${SOURCE} ${HEADERS})
add_executable(${EXEC_NAME}-test ${SOURCE} ${HEADERS})
SET_TARGET_PROPERTIES(${EXEC_NAME}-test PROPERTIES ${COMPILE_FLAGS} "-DSHOW")

TARGET_LINK_LIBRARIES(${EXEC_NAME}
			${OpenCV_LIBRARIES}
			)

TARGET_LINK_LIBRARIES(${EXEC_NAME}-test
			${OpenCV_LIBRARIES}
			)
