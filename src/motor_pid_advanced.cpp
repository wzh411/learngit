#include <bits/stdc++.h>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>
#include <motor_sim_msgs/msg/motor_state.hpp>
using namespace std;
using namespace std::chrono_literals;
class Pid : public rclcpp::Node {
     public:
    Pid() : Node("motor_pid_advanced"),akp(2.0), aki(0.0), akd(0.0),vkp(0.5), vki(0.0), vkd(0.05),target(2 * M_PI), angle(0), av(0),a_last_e(0), a_i(0), v_last_e(0), v_i(0),il(10.0), mo(5.0) {
        subscription = create_subscription<motor_sim_msgs::msg::MotorState>("motor_state", 10,[this](motor_sim_msgs::msg::MotorState::SharedPtr m) {
        angle = m->angle;
        av = m->av;
        });
        publisher = create_publisher<std_msgs::msg::Float64>("torque_cmd", 10);
        timer = create_wall_timer(2ms, [this] {
        });
    }
    private:
    double akp, aki , akd , vkp , vki , vkd , target , angle , av , a_last_e , a_i , v_last_e , v_i , il, mo;
    rclcpp::Subscription<motor_sim_msgs::msg::MotorState>::SharedPtr subscription;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr publisher;
    rclcpp::TimerBase::SharedPtr timer;
};
int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Pid>());
    rclcpp::shutdown();
    return 0;
}
