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


