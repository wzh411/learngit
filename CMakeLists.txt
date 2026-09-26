cmake_minimum_required(VERSION 3.24)
project(rmcs_vision)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-O2 -Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

# ---- plugin 动态库：只编 contour_detector.cpp，排除带 main 的节点 ----
ament_auto_add_library(
    ${PROJECT_NAME} SHARED
    src/contour_detector.cpp
)

pluginlib_export_plugin_description_file(rmcs_executor plugins.xml)

# ---- 独立可执行节点：lunkuo_vision ----
ament_auto_add_executable(lunkuo_vision src/lunkuo_vision.cpp)

ament_auto_package()
