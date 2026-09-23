"""
ros2 launch launch_learning basic.launch.py
"""

from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    # 入口函数名固定为 generate_launch_description，必须返回一个 LaunchDescription，
    publisher_node = Node(
        package='topic_learning_pub',
        executable='publisher_node',
        name='publisher_node',
        output='screen',
    )

    subscriber_node = Node(
        package='topic_learning_sub',
        executable='subscriber_node',
        name='subscriber_node',
        output='screen',
    )

    return LaunchDescription([
        publisher_node,
        subscriber_node,
    ])
