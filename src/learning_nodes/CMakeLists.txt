cmake_minimum_required(VERSION 3.8)
project(learning_nodes)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(example_interfaces REQUIRED) # for the service
find_package(std_msgs REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()



# Add the publisher executable
add_executable(talker src/minimal_publisher.cpp)
ament_target_dependencies(talker rclcpp std_msgs)

# Add the subscriber executable
add_executable(listener src/minimal_subscriber.cpp)
ament_target_dependencies(listener rclcpp std_msgs)


# Add the server executable
add_executable(server src/simple_server.cpp)
ament_target_dependencies(server rclcpp example_interfaces)

# Add the client executable
add_executable(client src/simple_client.cpp)
ament_target_dependencies(client rclcpp example_interfaces)

# PID parameters example
add_executable(pid_tuner src/pid_tuner.cpp)
ament_target_dependencies(pid_tuner rclcpp)

# Tell ROS 2 where to install the executables so it can find them later
install(TARGETS
  talker
  listener
  server
  client
  pid_tuner
  DESTINATION lib/${PROJECT_NAME})

  #launch 
  # Install launch files.
install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}/
)
ament_package()
