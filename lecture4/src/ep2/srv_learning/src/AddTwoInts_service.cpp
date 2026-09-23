#include <chrono>
#include <ctime>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/add_two_ints.hpp"

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

// 服务端节点：提供 add_two_ints 服务，返回两个整数之和
class AddTwoIntsService : public rclcpp::Node
{
public:
  AddTwoIntsService()
  : Node("add_two_ints_service")
  {
    // 创建服务，服务名为 "add_two_ints"
    service_ = this->create_service<example_interfaces::srv::AddTwoInts>(
      "add_two_ints",
      std::bind(&AddTwoIntsService::handle_add_two_ints, this, std::placeholders::_1, std::placeholders::_2));
    RCLCPP_INFO(this->get_logger(), "add_two_ints 服务已就绪。");
  }

private:
  void handle_add_two_ints(
    const std::shared_ptr<example_interfaces::srv::AddTwoInts::Request> request,
    std::shared_ptr<example_interfaces::srv::AddTwoInts::Response> response)
  {
    response->sum = request->a + request->b;
    std::string ts = current_timestamp();
    RCLCPP_INFO(
      this->get_logger(), "[%s] 收到请求: a = %ld, b = %ld, 结果 = %ld",
      ts.c_str(), request->a, request->b, response->sum);
  }

  rclcpp::Service<example_interfaces::srv::AddTwoInts>::SharedPtr service_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<AddTwoIntsService>());
  rclcpp::shutdown();
  return 0;
}
