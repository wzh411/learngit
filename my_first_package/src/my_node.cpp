#include <bits/stdc++.h>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
using namespace std;
using namespace std::chrono_literals;
int main(int argc,char*argv[]){
	rclcpp::init(argc,argv);
	auto n=rclcpp::Node::make_shared("publisher");
	auto p=n->create_publisher<std_msgs::msg::String>("topic",10);
	for(int i=0;rclcpp::ok();i++){
		auto m=std_msgs::msg::String();
		m.data="hello world "+to_string(i);
		RCLCPP_INFO(n->get_logger(),"%s",m.data.c_str());
		p->publish(m);
		this_thread::sleep_for(1s);
	}
	rclcpp::shutdown();
	return 0;
}


