# =============================================================================
#  一键启动任务二：电机模拟器(dianjikongzhi) + 输入源(motor_test)
#
#  默认跑 18 s 自动测试序列（阶跃 / 正弦 / 方波 / 卸载 / 脉冲），数据写入 motor_sim_log.csv
#
#  用法：
#    ros2 launch my_pkg motor_sim.launch.py
#    ros2 launch my_pkg motor_sim.launch.py waveform:=step amplitude:=0.5 duration:=8.0
#    ros2 launch my_pkg motor_sim.launch.py waveform:=sine frequency:=0.5
#    ros2 launch my_pkg motor_sim.launch.py mode:=velocity_pid target_velocity:=20.0
#    ros2 launch my_pkg motor_sim.launch.py mode:=position_pid target_angle:=1.5708
#    ros2 launch my_pkg motor_sim.launch.py integrator:=euler physics_rate:=0.4   # 看数值发散
#
#  数值参数统一用 ParameterValue(..., value_type=float) 声明类型，避免字符串下发导致类型不匹配
# =============================================================================
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def fp(name):
    return ParameterValue(LaunchConfiguration(name), value_type=float)


def sp(name):
    return ParameterValue(LaunchConfiguration(name), value_type=str)


def generate_launch_description():
    args = [
        # ---- 模拟器参数 ----
        DeclareLaunchArgument('inertia', default_value='0.01', description='转动惯量 J [kg*m^2]'),
        DeclareLaunchArgument('damping', default_value='0.01', description='粘性阻尼 B [N*m*s/rad]'),
        DeclareLaunchArgument('max_torque', default_value='3.0', description='力矩限幅 [N*m]'),
        DeclareLaunchArgument('load_torque', default_value='0.0', description='恒定负载力矩 TL [N*m]'),
        DeclareLaunchArgument('physics_rate', default_value='1000.0', description='状态更新(积分)频率 [Hz]'),
        DeclareLaunchArgument('integrator', default_value='rk4', description='积分方法: rk4 | euler'),
        DeclareLaunchArgument('control_rate', default_value='100.0', description='控制频率 [Hz]'),
        DeclareLaunchArgument('csv_path', default_value='motor_sim_log.csv', description='数据记录 CSV'),

        # ---- 输入源参数 ----
        DeclareLaunchArgument('mode', default_value='open_loop',
                              description='open_loop | velocity_pid | position_pid'),
        DeclareLaunchArgument('waveform', default_value='sequence',
                              description='constant|step|sine|square|chirp|impulse|sequence'),
        DeclareLaunchArgument('amplitude', default_value='0.5', description='力矩幅值 [N*m]'),
        DeclareLaunchArgument('frequency', default_value='0.5', description='波形频率 [Hz]'),
        DeclareLaunchArgument('start_delay', default_value='0.0', description='信号启动延时 [s]'),
        DeclareLaunchArgument('duration', default_value='18.0', description='信号时长 [s]'),
        DeclareLaunchArgument('target_velocity', default_value='20.0', description='速度环目标 [rad/s]'),
        DeclareLaunchArgument('target_angle', default_value='1.5707963267948966', description='位置环目标 [rad]'),
    ]

    motor = Node(
        package='my_pkg',
        executable='motor',
        name='motor',
        output='screen',
        emulate_tty=True,
        parameters=[{
            'motor.inertia': fp('inertia'),
            'motor.damping': fp('damping'),
            'motor.max_torque': fp('max_torque'),
            'load_torque': fp('load_torque'),
            'physics_rate': fp('physics_rate'),
            'integrator': sp('integrator'),
            'control_rate': fp('control_rate'),
            'csv_path': sp('csv_path'),
        }],
    )

    test = Node(
        package='my_pkg',
        executable='motor_test',
        name='motor_test',
        output='screen',
        emulate_tty=True,
        parameters=[{
            'mode': sp('mode'),
            'waveform': sp('waveform'),
            'amplitude': fp('amplitude'),
            'frequency': fp('frequency'),
            'start_delay': fp('start_delay'),
            'duration': fp('duration'),
            'publish_rate': fp('control_rate'),
            'motor.inertia': fp('inertia'),
            'motor.damping': fp('damping'),
            'load_torque': fp('load_torque'),
            'target.velocity_rad_s': fp('target_velocity'),
            'target.angle_rad': fp('target_angle'),
        }],
    )

    return LaunchDescription(args + [motor, test])
