from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # 1. Start the Publisher
        Node(
            package='learning_nodes',
            executable='talker',
            name='system_talker'
        ),
        
        # 2. Start the Subscriber
        Node(
            package='learning_nodes',
            executable='listener',
            name='system_listener'
        ),
        
        # 3. Start the PID Tuner and inject custom default parameters!
        Node(
            package='learning_nodes',
            executable='pid_tuner',
            name='custom_pid_tuner',
            parameters=[
                {'kp': 5.5},
                {'ki': 0.1},
                {'kd': 1.2}
            ]
        )
    ])
