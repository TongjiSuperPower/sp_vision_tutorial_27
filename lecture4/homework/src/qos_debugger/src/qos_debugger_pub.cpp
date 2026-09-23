
#include <chrono>
#include <cmath>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "nav_hw_interfaces/msg/sensor_data.hpp"

class SensorPublisher : public rclcpp::Node
{
public:
    SensorPublisher()
        : Node("sensor_publisher")
    {
        this->declare_parameter("reliability", "best_effort");
        this->declare_parameter("depth", 10);
        this->declare_parameter("rate", 100.0);

        const std::string reliability = this->get_parameter("reliability").as_string();
        const int depth = this->get_parameter("depth").as_int();
        const double rate = this->get_parameter("rate").as_double();

        rclcpp::QoS qos{rclcpp::KeepLast(depth)};
        if (reliability == "best_effort")
        {
            qos.reliability(rclcpp::ReliabilityPolicy::BestEffort);
        }
        else
        {
            qos.reliability(rclcpp::ReliabilityPolicy::Reliable);
        }

        publisher_ = this->create_publisher<nav_hw_interfaces::msg::SensorData>("sensor_data", qos);

        RCLCPP_INFO(
            this->get_logger(),
            "Publisher on /sensor_data: reliability=%s, depth=%d, rate=%.1f Hz",
            reliability.c_str(), depth, rate);

        auto period = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::duration<double>(1.0 / rate));
        timer_ = this->create_wall_timer(period, std::bind(&SensorPublisher::timer_callback, this));
    }

private:
    void timer_callback()
    {
        auto msg = nav_hw_interfaces::msg::SensorData();
        msg.header.stamp = this->now();
        msg.header.frame_id = "sensor_link";
        msg.seq = seq_++;
        msg.distance = 2.0f + static_cast<float>(std::sin(seq_ * 0.1f)) * 0.5f;
        msg.ranges = {1.0f, 2.0f, 3.0f, 4.0f};

        publisher_->publish(msg);

        RCLCPP_INFO_THROTTLE(
            this->get_logger(), *this->get_clock(), 2000,
            "Published seq=%u", msg.seq);
    }

    rclcpp::Publisher<nav_hw_interfaces::msg::SensorData>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    uint32_t seq_{0};
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SensorPublisher>());
    rclcpp::shutdown();
    return 0;
}
