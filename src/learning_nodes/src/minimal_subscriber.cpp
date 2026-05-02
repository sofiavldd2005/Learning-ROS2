#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "rclcpp/qos.hpp" // Include the QoS library
class MinimalSubscriber : public rclcpp::Node
{
public:
  MinimalSubscriber() : Node("erocket_ground_station")
  {
    // Create a subscriber on the topic "/rocket_status"
    // When a message arrives, automatically trigger the topic_callback function
	// 1. Create a QoS profile object. Start with KEEP_LAST depth of 10.
	rclcpp::QoS sensor_qos_profile(10);

	// 2. ModifyI its settings
	sensor_qos_profile.best_effort();
	sensor_qos_profile.durability_volatile();

	// 3. Pass the object to your publisher or subscriber
	subscription_ = this->create_subscription<std_msgs::msg::String>(
			"fast_sensor_topic", sensor_qos_profile, std::bind(&MinimalSubscriber::topic_callback, this, std::placeholders::_1));
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
