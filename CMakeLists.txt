cmake_minimum_required(VERSION 3.8)
project(my_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(motor_sim_msgs REQUIRED)

add_executable(publisher_sin src/publisher_sin.cpp)
ament_target_dependencies(publisher_sin rclcpp std_msgs)

add_executable(subscriber_sin src/subscriber_sin.cpp)
ament_target_dependencies(subscriber_sin rclcpp std_msgs)

# 电机模拟器：订阅力矩 -> 发布转速/角度
add_executable(dianjikongzhi src/dianjikongzhi.cpp)
ament_target_dependencies(dianjikongzhi rclcpp std_msgs motor_sim_msgs)

# 测试节点：发力矩 + 订阅自检
add_executable(motor_test src/motor_test.cpp)
ament_target_dependencies(motor_test rclcpp std_msgs motor_sim_msgs)

install(TARGETS
  publisher_sin
  subscriber_sin
  dianjikongzhi
  motor_test
  DESTINATION lib/${PROJECT_NAME}
)

# launch 文件与绘图脚本
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

install(PROGRAMS scripts/plot_motor_state.py
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
