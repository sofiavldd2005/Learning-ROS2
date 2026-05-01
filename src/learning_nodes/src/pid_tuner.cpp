#include <chrono>
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

class PidTunerNode : public rclcpp::Node
{
public:
  PidTunerNode() : Node("pid_tuner")
  {
    // 1. Declare the parameters with their default values
    this->declare_parameter("kp", 1.0);
    this->declare_parameter("ki", 0.0);
    this->declare_parameter("kd", 0.1);

    // 2. Create a timer to simulate a 1Hz control loop
    timer_ = this->create_wall_timer(
      1000ms, std::bind(&PidTunerNode::control_loop_callback, this));
      
    RCLCPP_INFO(this->get_logger(), "PID Tuner Node Started!");
  }

private:
  void control_loop_callback()
  {
    // 3. Read the current live values of the parameters
    double current_kp = this->get_parameter("kp").as_double();
    double current_ki = this->get_parameter("ki").as_double();
    double current_kd = this->get_parameter("kd").as_double();

    // 4. Print them out (In real life, you'd calculate motor PWM here)
    RCLCPP_INFO(this->get_logger(), 
      "Computing Thrust Vector with Kp: %.2f | Ki: %.2f | Kd: %.2f", 
      current_kp, current_ki, current_kd);
  }

  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PidTunerNode>());
  rclcpp::shutdown();
  return 0;
}
