#include <bits/stdc++.h>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>
using namespace std;
class Subscriber_sin : public rclcpp::Node{
    public: 
    Subscriber_sin():
    Node("subscriber_sin"){
        auto topic =
         [this](std_msgs::msg::Float64::UniquePtr msg) -> void{
            double raw =msg->data;
            double low = (lastlow) +(alpha_) *(raw-lastlow);
            lastlow =low;
          RCLCPP_INFO(this->get_logger(), "", msg->data);  
        
        };
    subscription_ =
    this->create_subscription<std_msgs::msg::Float64>("sin",10,topic);
    }
    private:
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr subscription_;
    double alpha_;           
    double lastlow;
};
int main(int argc,char*argv[]){
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<Subscriber_sin>());
    rclcpp::shutdown();
    return 0;
}