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
            
            double lowpass = (lastlow) +(alpha_) *(raw-lastlow);
            lastlow =lowpass;

            mediapass .push_back(raw);
            if(mediapass .size() >5){
                mediapass .erase(mediapass .begin());
            }
            vector<double> t =mediapass;
            sort (t.begin(),t.end());
            double median = t[t.size()/2];

            auto msg_low = std_msgs::msg::Float64();
            msg_low.data = lowpass;
            pub_lowpass_->publish(msg_low);
            
            auto med = std_msgs::msg::Float64();
            med.data = median;
            pub_median_->publish(med);
        RCLCPP_INFO(this->get_logger(), "原始：%.3f | 低通: %.3f | 中通: %.3f", raw, lowpass, median);  
        
        };
    subscription_ =
    this->create_subscription<std_msgs::msg::Float64>("sin",10,topic);
    pub_lowpass_ = this->create_publisher<std_msgs::msg::Float64>("sin_lowpass", 10);
    pub_median_ = this->create_publisher<std_msgs::msg::Float64>("sin_median", 10);
    alpha_ = 0.1;                     
    lastlow = 0.0;
    }
    private:
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr subscription_;
    double alpha_;           
    double lastlow;
    vector <double> mediapass;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_lowpass_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_median_;

};
int main(int argc,char*argv[]){
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<Subscriber_sin>());
    rclcpp::shutdown();
    return 0;
}