#include <chrono>

#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

class HelloWorldNode : public rclcpp::Node
{
public:
  HelloWorldNode()
      : Node("helloworld_node")
  {
    // 0.5 秒触发一次，持续打印 hello, world
    timer_ = this->create_wall_timer(500ms, std::bind(&HelloWorldNode::timer_callback, this));
  }

private:
  void timer_callback()
  {
    RCLCPP_INFO(this->get_logger(), "hello, world");
  }

  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<HelloWorldNode>());
  rclcpp::shutdown();
  return 0;
}
