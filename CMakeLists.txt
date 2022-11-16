cmake_minimum_required(VERSION 3.0)
project("OpenGL")

message(STATUS "This is SOURCE dir " ${PROJECT_SOURCE_DIR})

#添加头文件
link_directories(${PROJECT_SOURCE_DIR}/lib) #库目录

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/imgui) # 头文件目录

# 添加目标链接
# set(GLFW_LINK ${PROJECT_SOURCE_DIR}/lib/libglfw3.a)
# link_libraries(${GLFW_LINK})
# set(GLEW_LINK ${PROJECT_SOURCE_DIR}/lib/glew32s.lib,glew32.lib)
# link_libraries(${GLEW_LINK})
# link_directories(${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fexec-charset=GBK")

#set(SOURCE_FILES src)
aux_source_directory(${PROJECT_SOURCE_DIR}/src SOURCE_FILES)
add_executable(main ${SOURCE_FILES})    #源文件
# target_link_libraries(main libimgui.a)
target_link_libraries(main glfw3)
