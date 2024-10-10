#project(${exe_name} LANGUAGES CXX CUDA)
# add CUDA
option(CUDA_USE_STATIC_CUDA_RUNTIME OFF)
find_package(CUDA  REQUIRED "C:\\Program Files\\NVIDIA GPU Computing Toolkit\\CUDA\\v11.6")
enable_language(CUDA)
message(STATUS "cuda version: " ${CUDA_VERSION_STRING})
#CUDA 
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS};-std=c++11;-g;-G;-gencode;arch=compute_75;code=sm_75)
# add TensorRT8
set(TENSORRT_LIB_PATH "..//..\\HSdk\\thirdpart\\TensorRT-8.5.3.1\\lib")
file(GLOB Tensorrt_LIBS "${TENSORRT_LIB_PATH}/*.lib")


# add OpenCV
set(CMAKE_PREFIX_PATH "..//..\\HSdk\\thirdpart\\opencv\\x64\\vc15\\lib")
find_package(OpenCV REQUIRED)

#打印调试信息
MESSAGE(STATUS "Project: ${PROJECT_NAME}")
MESSAGE(STATUS "OpenCV library status:")
MESSAGE(STATUS "    version: ${OpenCV_VERSION}")
MESSAGE(STATUS "    libraries: ${OpenCV_LIBS}")
MESSAGE(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

file(GLOB app_sources app_*.cpp)
set(
        app_sources
        "${app_sources};"
)
file(GLOB myProject ./include/*.h ./src/*.cpp)
set(
        myProject000
        "${myProject};"
)

foreach(app_source ${app_sources})

        get_filename_component(exe_name ${app_source} NAME_WE)
        add_executable(
                ${exe_name}
                ${app_source}
                ${myProject}
        )
        target_include_directories(
                ${exe_name}
                PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/include
                ${CUDA_INCLUDE_DIRS}
                ${OpenCV_INCLUDE_DIRS}
                "..//..\\HSdk\\thirdpart\\TensorRT-8.5.3.1\\include"
                
        )
        target_link_libraries (
                ${exe_name}
                ${CUDA_LIBRARIES}
                cudart
                cudnn
                nvonnxparser 
                nvinfer 
                nvinfer_plugin
                HUtils
                ${Tensorrt_LIBS}
                ${OpenCV_LIBS}
        )
        IF(MSVC)
        install(
                TARGETS ${exe_name}
                RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/install-win
        )
        ELSEIF(UNIX)
        install(
                TARGETS ${exe_name}
                RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/install-ubuntu/bin
        )
        ENDIF()
endforeach(app_source)



# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/qk/QKWORK/gitlab/wicriuuvsimuplat/install-ubuntu/lib