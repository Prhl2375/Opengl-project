cmake_minimum_required(VERSION 3.3)
find_package(OpenGL REQUIRED)
project(project0)

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")




include_directories("${PROJECT_SOURCE_DIR}/include")

link_directories("${PROJECT_SOURCE_DIR}/lib")


add_executable(project0 main.cpp glad.c stb_image.cpp)

target_link_libraries(project0 opengl32 glfw3 gdi32)

