#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/add_two_ints.hpp"
#include <chrono>
#include <memory>

using namespace std::chrono_literals;

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("add_two_ints_client");

  // Create the client pointing to the exact same service name
  auto client = node->create_client<example_interfaces::srv::AddTwoInts>("add_two_ints");

  // Create the request structure and fill it
  auto request = std::make_shared<example_interfaces::srv::AddTwoInts::Request>();
  request->a = 41;
  request->b = 1;

  // Wait for the server to appear on the network
  while (!client->wait_for_service(1s)) {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(node->get_logger(), "Interrupted while waiting for the service.");
      return 0;
    }
    RCLCPP_INFO(node->get_logger(), "Server not available yet, waiting...");
  }

  // Send the request
  RCLCPP_INFO(node->get_logger(), "Sending request to add %ld and %ld", request->a, request->b);
  auto result_future = client->async_send_request(request);

  // Wait for the response (blocking here until the future completes)
  if (rclcpp::spin_until_future_complete(node, result_future) == rclcpp::FutureReturnCode::SUCCESS) {
    RCLCPP_INFO(node->get_logger(), "Result received: %ld", result_future.get()->sum);
  } else {
    RCLCPP_ERROR(node->get_logger(), "Failed to call service add_two_ints");
  }

  rclcpp::shutdown();
  return 0;
}
