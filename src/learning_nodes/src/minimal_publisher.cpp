#include <chrono>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp" // The standard message type we are using
#include "rclcpp/qos.hpp" // Include the QoS library
#include "rclcpp/qos.hpp" // Include the QoS library

using namespace std::chrono_literals;

// Inherit from the rclcpp::Node base class
class MinimalPublisher : public rclcpp::Node
{
public:
  MinimalPublisher() : Node("erocket_status_publisher"), count_(0)
  {
    

	// 1. Create a QoS profile object. Start with KEEP_LAST depth of 10.
	rclcpp::QoS sensor_qos_profile(10);

	// 2. Modify its settings
	sensor_qos_profile.best_effort();
	sensor_qos_profile.durability_volatile();
	// Create a publusher on the topic "/rocket_status with the specified QoS
	publisher_ = this->create_publisher<std_msgs::msg::String>(
    "fast_sensor_topic", sensor_qos_profile);
	
    publisher_ = this->create_publisher<std_msgs::msg::String>("rocket_status", 10);
    
    // Create a timer that fires every 500ms and calls the timer_callback function
    timer_ = this->create_wall_timer(
      500ms, std::bind(&MinimalPublisher::timer_callback, this));
  }

private:
  void timer_callback()
  {
    auto message = std_msgs::msg::String();
    message.data = "E-Rocket Systems Nominal! Count: " + std::to_string(count_++);
    
    // Print to the console (like printf or ESP_LOGI)
    RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
    
    // Actually send the message over DDS to the network
    publisher_->publish(message);
  }

  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  size_t count_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv); // Initialize the ROS 2 communication systems
  rclcpp::spin(std::make_shared<MinimalPublisher>()); // Block and keep the node alive, processing callbacks
  rclcpp::shutdown();
  return 0;
}
