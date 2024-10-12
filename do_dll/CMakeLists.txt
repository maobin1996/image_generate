cmake_minimum_required(VERSION 3.10)

# 项目名称
project(test)


# 设置 OpenCV 的固定路径
set(OpenCV_DIR "./opencv46")
find_package(OpenCV)
include_directories(${OpenCV_INCLUDE_DIRS})
# 打印 OpenCV 的版本和路径
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include directories: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")

# 设置 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)  # 禁用编译器扩展
# 查找库的路径
set(LIBRARY_PATH "./libs")  
set(INCLUDE_PATH "./include")  
# 添加库搜索路径
link_directories(${LIBRARY_PATH})

# 添加可执行文件
add_executable(test main.cpp)

# 包含头文件目录
include_directories(${INCLUDE_PATH})

# 链接库
target_link_libraries(test stitch_images)  # 库名称
target_link_libraries(test ${OpenCV_LIBS})