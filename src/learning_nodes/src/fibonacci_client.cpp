#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "example_interfaces/action/fibonacci.hpp"

class FibonacciActionClient : public rclcpp::Node
{
public:
  using Fibonacci = example_interfaces::action::Fibonacci;
  using GoalHandleFibonacci = rclcpp_action::ClientGoalHandle<Fibonacci>;

  FibonacciActionClient() : Node("fibonacci_action_client")
  {
    // 1. Create the client
    this->client_ptr_ = rclcpp_action::create_client<Fibonacci>(this, "fibonacci");
    
    // 2. Start a timer to send the goal. 
    // We delay slightly to give the network time to find the server.
    this->timer_ = this->create_wall_timer(
      std::chrono::milliseconds(500),
      std::bind(&FibonacciActionClient::send_goal, this));
  }

private:
  rclcpp_action::Client<Fibonacci>::SharedPtr client_ptr_;
  rclcpp::TimerBase::SharedPtr timer_;

  void send_goal()
  {
    this->timer_->cancel(); // Cancel timer so we only send the goal once!

    // Ensure the server is online before shooting data into the void
    if (!this->client_ptr_->wait_for_action_server(std::chrono::seconds(5))) {
      RCLCPP_ERROR(this->get_logger(), "Action server not available!");
      rclcpp::shutdown();
      return;
    }

    auto goal_msg = Fibonacci::Goal();
    goal_msg.order = 5; // We want 5 Fibonacci numbers

    RCLCPP_INFO(this->get_logger(), "Sending goal...");

    // 3. Package our callback functions (closures)
    auto send_goal_options = rclcpp_action::Client<Fibonacci>::SendGoalOptions();
    
    send_goal_options.goal_response_callback =
      std::bind(&FibonacciActionClient::goal_response_callback, this, std::placeholders::_1);
      
    send_goal_options.feedback_callback =
      std::bind(&FibonacciActionClient::feedback_callback, this, std::placeholders::_1, std::placeholders::_2);
      
    send_goal_options.result_callback =
      std::bind(&FibonacciActionClient::result_callback, this, std::placeholders::_1);

    // 4. Send the goal asynchronously
    this->client_ptr_->async_send_goal(goal_msg, send_goal_options);
  }

  // Callback A: Did the server accept or reject our goal?
  void goal_response_callback(const GoalHandleFibonacci::SharedPtr & goal_handle)
  {
    if (!goal_handle) {
      RCLCPP_ERROR(this->get_logger(), "Goal was rejected by server");
    } else {
      RCLCPP_INFO(this->get_logger(), "Goal accepted by server, waiting for result...");
    }
  }

  // Callback B: Called every time the server publishes feedback
  void feedback_callback(
    GoalHandleFibonacci::SharedPtr,
    const std::shared_ptr<const Fibonacci::Feedback> feedback)
  {
    RCLCPP_INFO(this->get_logger(), "Next number: %d", feedback->sequence.back());
  }

  // Callback C: Called when the server finally finishes
  void result_callback(const GoalHandleFibonacci::WrappedResult & result)
  {
    switch (result.code) {
      case rclcpp_action::ResultCode::SUCCEEDED:
        RCLCPP_INFO(this->get_logger(), "Goal succeeded! Sequence complete.");
        break;
      case rclcpp_action::ResultCode::ABORTED:
        RCLCPP_ERROR(this->get_logger(), "Goal was aborted");
        break;
      case rclcpp_action::ResultCode::CANCELED:
        RCLCPP_ERROR(this->get_logger(), "Goal was canceled");
        break;
      default:
        RCLCPP_ERROR(this->get_logger(), "Unknown result code");
        break;
    }
    rclcpp::shutdown(); // Kill this node cleanly
  }
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<FibonacciActionClient>());
  rclcpp::shutdown();
  return 0;
}
