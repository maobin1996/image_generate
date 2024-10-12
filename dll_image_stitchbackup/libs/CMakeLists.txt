cmake_minimum_required(VERSION 3.10)

# 项目名称
project(StitchImagesLibrary)
# 设置 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)  # 禁用编译器扩展

# 设置 OpenCV 的固定路径
set(OpenCV_DIR "./opencv46")
include_directories(${OpenCV_INCLUDE_DIRS})
# 查找 OpenCV
find_package(OpenCV REQUIRED)
# 打印 OpenCV 的版本和路径
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include directories: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
# 设置库的输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 添加库
add_library(stitch_images SHARED src/stitch_Images.cpp)
target_include_directories(stitch_images PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(stitch_images PRIVATE ${OpenCV_LIBS})


# add_library(stitch_images_Static STATIC  src/stitch_Images.cpp)
# target_include_directories(stitch_images_Static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_link_libraries(stitch_images_Static PRIVATE ${OpenCV_LIBS})

# 指定头文件搜索路径
# target_include_directories(stitch_images PUBLIC include)


# cmake_minimum_required(VERSION 3.10)
# project(Stitching)
# # 设置 C++17 标准
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(CMAKE_CXX_EXTENSIONS OFF)  # 禁用编译器扩展
# find_package(OpenCV REQUIRED)

# add_library(stitching SHARED stitching.cpp)

# target_include_directories(stitching PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# target_link_libraries(stitching ${OpenCV_LIBS})
