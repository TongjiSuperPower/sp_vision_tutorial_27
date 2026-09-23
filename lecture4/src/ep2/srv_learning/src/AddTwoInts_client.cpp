#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/add_two_ints.hpp"

using namespace std::chrono_literals;

// 返回当前时间的格式化字符串，例如 "2026-09-16 10:30:00.123"
std::string current_timestamp()
{
  auto now = std::chrono::system_clock::now();
  std::time_t t = std::chrono::system_clock::to_time_t(now);
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
    now.time_since_epoch()) % 1000;

  std::tm tm{};
  localtime_r(&t, &tm);

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
      << "." << std::setw(3) << std::setfill('0') << ms.count();
  return oss.str();
}

// 客户端节点：调用 add_two_ints 服务，发送两个整数并打印结果
class AddTwoIntsClient : public rclcpp::Node
{
public:
  AddTwoIntsClient()
  : Node("add_two_ints_client")
  {
    client_ = this->create_client<example_interfaces::srv::AddTwoInts>("add_two_ints");
  }

  bool wait_for_service()
  {
    while (!client_->wait_for_service(1s)) {
      if (!rclcpp::ok()) {
        RCLCPP_ERROR(this->get_logger(), "等待服务时被中断。");
        return false;
      }
      RCLCPP_INFO(this->get_logger(), "等待服务出现...");
    }
    return true;
  }

  void send_request(int64_t a, int64_t b)
  {
    auto request = std::make_shared<example_interfaces::srv::AddTwoInts::Request>();
    request->a = a;
    request->b = b;

    auto future = client_->async_send_request(request);

    if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), future) ==
      rclcpp::FutureReturnCode::SUCCESS)
    {
      std::string ts = current_timestamp();
      RCLCPP_INFO(
        this->get_logger(), "[%s] %ld + %ld = %ld",
        ts.c_str(), a, b, future.get()->sum);
    } else {
      RCLCPP_ERROR(this->get_logger(), "调用 add_two_ints 服务失败。");
    }
  }

private:
  rclcpp::Client<example_interfaces::srv::AddTwoInts>::SharedPtr client_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<AddTwoIntsClient>();

  if (argc != 3) {
    RCLCPP_INFO(node->get_logger(), "用法: ros2 run srv_learning add_two_ints_client <a> <b>");
    rclcpp::shutdown();
    return 1;
  }

  int64_t a = std::atoll(argv[1]);
  int64_t b = std::atoll(argv[2]);

  if (!node->wait_for_service()) {
    rclcpp::shutdown();
    return 1;
  }

  node->send_request(a, b);

  rclcpp::shutdown();
  return 0;
}
