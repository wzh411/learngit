#include <bits/stdc++.h>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>
using namespace std;
using namespace std::chrono_literals; 
int main(int argc, char*argv[]){
  rclcpp::init(argc,argv);
  auto n = std::make_shared<rclcpp::Node>("bo_publisher");
  auto p=n->create_publisher<std_msgs::msg::Float64>("bo",10);
  
  
  
  
  
  return 0;
}