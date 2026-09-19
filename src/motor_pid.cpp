#include <bits/stdc++.h>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>
#include <motor_sim_msgs/msg/motor_state.hpp>
using namespace std;
using namespace std::chrono_literals;

class Pid : public rclcpp::Node {
public:
    Pid() : Node("motor_pid"), kp(2.0), ki(0.0), kd(0.01),
            target(100), current(0), last_e(0), i(0),
            il(10.0), mo(50.0), ib(10.0) {
        sub = create_subscription<motor_sim_msgs::msg::MotorState>("motor_state", 10,
            [this](motor_sim_msgs::msg::MotorState::SharedPtr m) {
                current = m->av;
            });
        pub = create_publisher<std_msgs::msg::Float64>("torque_cmd", 10);
        tim = create_wall_timer(2ms, [this] {
            e = target - current;
            p = kp * e;
            if (fabs(e) < ib) i += ki * e;
            else i = 0;
            i = max(-il, min(i, il));
            d = kd * (e - last_e);
            out = p + i + d;
            out = max(-mo, min(out, mo));
            last_e = e;
            auto msg = std_msgs::msg::Float64();
            msg.data = out;
            pub->publish(msg);
            RCLCPP_INFO(get_logger(), "target=%.1f current=%.1f out=%.2f", target, current, out);
        });
    }
private:
    double kp, ki, kd, target, current, last_e, i, il, mo, ib, e, p, d, out;
    rclcpp::Subscription<motor_sim_msgs::msg::MotorState>::SharedPtr sub;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub;
    rclcpp::TimerBase::SharedPtr tim;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Pid>());
    rclcpp::shutdown();
    return 0;
}
