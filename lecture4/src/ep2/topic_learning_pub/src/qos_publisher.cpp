#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

class QosPublisher : public rclcpp::Node
{
public:
  QosPublisher()
      : Node("qos_publisher"), count_(0)
  {
    // 配置 QoS：尽力而为（best_effort），队列深度 10
    auto qos = rclcpp::QoS(rclcpp::KeepLast(10));
    // qos.reliability(RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT);
    qos.best_effort();

    publisher_ = this->create_publisher<std_msgs::msg::String>("qos_topic", qos);
    // 10Hz
    timer_ = this->create_wall_timer(100ms, std::bind(&QosPublisher::timer_callback, this));
  }

private:
  void timer_callback()
  {
    auto message = std_msgs::msg::String();
    message.data = "QoS best_effort message: " + std::to_string(count_++);
    publisher_->publish(message);
  }

  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  uint64_t count_;
};

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<QosPublisher>());
  rclcpp::shutdown();
  return 0;
}
