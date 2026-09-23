cmake_minimum_required(VERSION 3.8)
project(qos_debugger)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(nav_hw_interfaces REQUIRED)

add_executable(qos_debugger_pub src/qos_debugger_pub.cpp)
ament_target_dependencies(qos_debugger_pub rclcpp nav_hw_interfaces)

add_executable(qos_debugger_sub src/qos_debugger_sub.cpp)
ament_target_dependencies(qos_debugger_sub rclcpp nav_hw_interfaces)

install(TARGETS
  qos_debugger_pub
  qos_debugger_sub
  DESTINATION lib/${PROJECT_NAME})

ament_package()
