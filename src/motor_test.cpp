#include <bits/stdc++.h>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>
#include <motor_sim_msgs/msg/motor_state.hpp>
using namespace std;
using namespace std::chrono_literals;

class Test : public rclcpp::Node{
public:
  Test() : Node("motor_test"), t(0) {
    pub = create_publisher<std_msgs::msg::Float64>("torque_cmd", 10);
    sub = create_subscription<motor_sim_msgs::msg::MotorState>("motor_state", 10,
      [this](motor_sim_msgs::msg::MotorState::SharedPtr m){
        RCLCPP_INFO(get_logger(), "[自检] w=%.2f th=%.2f", m->angular_velocity, m->angle);
      });
    timer = create_wall_timer(100ms, [this]{
      t += 0.1;
      auto m = std_msgs::msg::Float64();
      m.data = (t < 5) ? 1.0 : 0.0;
      pub->publish(m);
    });
  }
private:
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub;
  rclcpp::Subscription<motor_sim_msgs::msg::MotorState>::SharedPtr sub;
  rclcpp::TimerBase::SharedPtr timer;
  double t;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(make_shared<Test>());
  rclcpp::shutdown();
  return 0;
}
