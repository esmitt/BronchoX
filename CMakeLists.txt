cmake_minimum_required(VERSION 2.8)

PROJECT(BronchoX)

SET(VTK_DIR C:/VTK/buildx64)
SET(OPENCV_DIR C:/opencv/buildx64)
SET(OPENCV_LIB C:/opencv/build/x64/vc14)
SET(OPENCV_BIN C:/opencv/build/x64/vc14/bin)
#SET(OPENCV_DIR C:/opencv/build/x64/vc14)

find_package(VTK REQUIRED)
find_package(OPENCV REQUIRED)
include(${VTK_USE_FILE})

# bring the headers
include_directories(include ${CMAKE_SOURCE_DIR}/include C:/opencv/build/include)
# add the sources
file(GLOB SOURCES "src/*.cpp")

add_executable(BronchoX ${SOURCES})

link_directories(${CMAKE_SOURCE_DIR}/lib)
target_link_libraries(BronchoX ${VTK_LIBRARIES} ${OPENCV_LIB}/lib/opencv_world330d.lib ${OPENCV_LIBRARIES})