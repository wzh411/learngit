#include <bits/stdc++.h>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>
#include <motor_sim_msgs/msg/motor_state.hpp>
using namespace std;
using namespace std::chrono_literals;
class Motor : public rclcpp::Node{
public:
  Motor() : Node("motor_simulator"), tau(0), w(0), th(0) {
    sub = create_subscription<std_msgs::msg::Float64>("torque_cmd", 10, [this](std_msgs::msg::Float64::SharedPtr m){ tau = m->data; });
    pub = create_publisher<motor_sim_msgs::msg::MotorState>("motor_state", 10);
    timer = create_wall_timer(10ms, [this]{ update(); });
  }
  private:
  void update() {
    w += (tau - 0.001*w) / 0.01 * 0.01;
    th += w * 0.01;
    auto m = motor_sim_msgs::msg::MotorState();
    m.av = w; m.angle = th;
    pub->publish(m);
    RCLCPP_INFO(get_logger(), "tau=%.2f w=%.2f th=%.2f", tau, w, th);
  }
  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr sub;
  rclcpp::Publisher<motor_sim_msgs::msg::MotorState>::SharedPtr pub;
  rclcpp::TimerBase::SharedPtr timer;
  double tau, w, th;
  };
int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(make_shared<Motor>());
  rclcpp::shutdown();
  return 0;
}
