#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/add_two_ints.hpp"
#include <memory>

// This is the callback that fires when a request arrives
void handle_add_two_ints(const std::shared_ptr<example_interfaces::srv::AddTwoInts::Request> request,
                         std::shared_ptr<example_interfaces::srv::AddTwoInts::Response> response)
{
  // Do the "work"
  response->sum = request->a + request->b;
  
  // Log it so we can see it happening
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Incoming request: a: %ld, b: %ld", request->a, request->b);
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Sending back response: %ld", response->sum);
}

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  
  // Create the node
  auto node = rclcpp::Node::make_shared("add_two_ints_server");

  // Create the Service on the network with the name "add_two_ints"
  auto service = node->create_service<example_interfaces::srv::AddTwoInts>(
    "add_two_ints", &handle_add_two_ints);

  RCLCPP_INFO(node->get_logger(), "Ready to add two ints. Waiting for clients...");

  // Block and spin, waiting for requests
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
