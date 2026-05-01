#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class MinimalSubscriber : public rclcpp::Node
{
public:
  MinimalSubscriber() : Node("erocket_ground_station")
  {
    // Create a subscriber on the topic "/rocket_status"
    // When a message arrives, automatically trigger the topic_callback function
    subscription_ = this->create_subscription<std_msgs::msg::String>(
      "rocket_status", 10, std::bind(&MinimalSubscriber::topic_callback, this, std::placeholders::_1));
  }

private:
  // ISR Equivalent
  void topic_callback(const std_msgs::msg::String & msg) const
  {
    RCLCPP_INFO(this->get_logger(), "Ground Station heard: '%s'", msg.data.c_str());
  }

  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalSubscriber>()); // Block and listen
  rclcpp::shutdown();
  return 0;
}
