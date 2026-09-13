#include <bits/stdc++.h>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
using namespace std;
void cb(const std_msgs::msg::String::SharedPtr m){
	RCLCPP_INFO(rclcpp::get_logger("subscriber"),"%s",m->data.c_str());
}
int main(int argc,char*argv[]){
	rclcpp::init(argc,argv);
	auto n=rclcpp::Node::make_shared("subscriber");
	auto s=n->create_subscription<std_msgs::msg::String>("topic",10,cb);
	rclcpp::spin(n);
	rclcpp::shutdown();
	return 0;
}
