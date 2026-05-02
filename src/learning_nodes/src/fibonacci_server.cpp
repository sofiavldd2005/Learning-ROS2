#include <functional>
#include <memory>
#include <thread>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "example_interfaces/action/fibonacci.hpp"
//struct and then implementing a **trait**:`impl rclcpp::Node for FibonacciActionServer`. By inheriting rclcpp::Node, our server instantly gets all the logging, timing, and network discovery features built into ROS
class FibonacciActionServer : public rclcpp::Node
{
public:
  using Fibonacci = example_interfaces::action::Fibonacci;
  using GoalHandleFibonacci = rclcpp_action::ServerGoalHandle<Fibonacci>;

  FibonacciActionServer() : Node("fibonacci_action_server")
  {
    using namespace std::placeholders;

    // Create the action server and bind our 3 core functions
    this->action_server_ = rclcpp_action::create_server<Fibonacci>(
      this,
      "fibonacci",
      std::bind(&FibonacciActionServer::handle_goal, this, _1, _2),
      std::bind(&FibonacciActionServer::handle_cancel, this, _1),
      std::bind(&FibonacciActionServer::handle_accepted, this, _1));
      // handle_goal is a class method. It requires a pointer to the specific object instance (this) to know whose memory to access.
	  // Solution: std::bind packages the object instance (this) and the function pointer together into a single callable object. 
	  // The _1 and _2 are placeholders. They literally mean: "When DDS triggers this callback, take the 1st and 2nd network arguments and plug them into these slots."
    RCLCPP_INFO(this->get_logger(), "Fibonacci Action Server Ready!");
  }

private:
  rclcpp_action::Server<Fibonacci>::SharedPtr action_server_;

  // 1. Check if the goal is valid. 
  rclcpp_action::GoalResponse handle_goal(
    const rclcpp_action::GoalUUID & uuid,
    std::shared_ptr<const Fibonacci::Goal> goal)
  {
    RCLCPP_INFO(this->get_logger(), "Received goal to compute sequence of order %d", goal->order);
    (void)uuid;
    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
  }

  // 2. Handle what happens if the user hits "Cancel"
  rclcpp_action::CancelResponse handle_cancel(
		  // equivalent to Arc<T> (Atomic Reference Counted pointer). 
		  // ROS 2 uses these everywhere so that multiple nodes or threads can pass network messages around without copying the actual data, and without memory leaks.
    const std::shared_ptr<GoalHandleFibonacci> goal_handle)
  {
    RCLCPP_INFO(this->get_logger(), "Received request to cancel goal");
    (void)goal_handle;
    return rclcpp_action::CancelResponse::ACCEPT;
  }

  // 3. If accepted, start a NEW thread so we don't freeze the ROS node
  void handle_accepted(const std::shared_ptr<GoalHandleFibonacci> goal_handle)
  { //std::thread::spawn(move || { self.execute(goal_handle) }).
    std::thread{std::bind(&FibonacciActionServer::execute, this, std::placeholders::_1), goal_handle}.detach();
  }

  // 4. The actual work thread
  void execute(const std::shared_ptr<GoalHandleFibonacci> goal_handle)
  {
    rclcpp::Rate loop_rate(1); // 1 Hz loop to simulate a long task
    const auto goal = goal_handle->get_goal();
    auto feedback = std::make_shared<Fibonacci::Feedback>();
    auto result = std::make_shared<Fibonacci::Result>();

    auto & sequence = feedback->sequence;
    sequence.push_back(0);
    sequence.push_back(1);

    for (int i = 1; (i < goal->order) && rclcpp::ok(); ++i) {
      // Check if a cancel request came in while we were computing
      if (goal_handle->is_canceling()) {
        result->sequence = sequence;
        goal_handle->canceled(result);
        RCLCPP_INFO(this->get_logger(), "Goal canceled gracefully!");
        return;
      }
      
      // Do the math
      sequence.push_back(sequence[i] + sequence[i - 1]);
      
      // Send the intermediate feedback
      goal_handle->publish_feedback(feedback);
      RCLCPP_INFO(this->get_logger(), "Publishing feedback...");

      loop_rate.sleep(); // Wait 1 second
    }

    // Task finished successfully!
    if (rclcpp::ok()) {
      result->sequence = sequence;
      goal_handle->succeed(result);
      RCLCPP_INFO(this->get_logger(), "Goal succeeded!");
    }
  }
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<FibonacciActionServer>());
  rclcpp::shutdown();
  return 0;
}
