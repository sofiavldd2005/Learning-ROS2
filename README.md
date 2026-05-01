# Learning ROS 2

## My 1st workspace
1. Creating the workspace:
```bash
mkdir -p ~/learning_ros_ws/src
cd ~/learning_ros_ws/src
```
2.  Generate the C++ package:
```bash
ros2 pkg create --build-type ament_cmake learning_nodes --dependencies rclcpp std_msgs
```
3. Added my minimal publisher and subscriber `.cpp` files
4. Edit CMakeLists.txt to add the rules to compile the `.cpp` files into executables:
    - Rules added before `ament_package()` section
```cmake
# Add the publisher executable
add_executable(talker src/minimal_publisher.cpp)
ament_target_dependencies(talker rclcpp std_msgs)

# Add the subscriber executable
add_executable(listener src/minimal_subscriber.cpp)
ament_target_dependencies(listener rclcpp std_msgs)

# Tell ROS 2 where to install the executables so it can find them later
install(TARGETS
  talker
  listener
  DESTINATION lib/${PROJECT_NAME})
```

5. Build the workspace:
```bash
cd ~/learning_ros_ws
colcon build
```
6. Source and Run
    - The install folder contains the setup scripts needed to inject this specific workspace into your terminal's environment
```bash
source ~/learning_ros_ws/install/setup.zsh
ros2 run learning_nodes talker
```
and
```bash
source ~/learning_ros_ws/install/setup.zsh
ros2 run learning_nodes listener
``
