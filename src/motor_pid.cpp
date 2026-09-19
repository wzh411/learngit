#include <bits/stdc++.h>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>
#include <motor_sim_msgs/msg/motor_state.hpp>
using namespace std;
using namespace std::chrono_literals;
class Pid : public rclcpp::Node{
    public:
    
    private:
    double kp ,ki,kd,target,current,last_e,io,il,mo,ib,e,p,d,out;
    rclcpp::Subscription<motor_sim_msgs::msg::MotorState>::SharedPtr subscription;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr publisher;
    rclcpp::TimerBase::SharedPtr timer;
};
int main(int argc,char*argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Pid>());
    rclcpp::shutdown();
    return 0;
}
  