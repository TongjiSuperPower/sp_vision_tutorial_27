import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy
from std_msgs.msg import String


class QosSubscriber(Node):
    def __init__(self):
        super().__init__('qos_subscriber')

        # 配置 QoS：可靠（reliable），队列深度 10
        # 注意：这里和发布者的 best_effort 不一致，演示「QoS 不匹配 → 收不到消息」
        qos = QoSProfile(
            reliability=ReliabilityPolicy.RELIABLE,
            history=HistoryPolicy.KEEP_LAST,
            depth=10,
        )

        self.subscription = self.create_subscription(
            String,
            'qos_topic',
            self.listener_callback,
            qos,
        )

    def listener_callback(self, msg):
        self.get_logger().info('Received: "%s"' % msg.data)


def main(args=None):
    rclpy.init(args=args)
    node = QosSubscriber()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
