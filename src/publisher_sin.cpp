#include <bits/stdc++.h>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>
using namespace std;
using namespace std::chrono_literals; 
int main(int argc, char*argv[]){
  rclcpp::init(argc,argv);
  auto n = std::make_shared<rclcpp::Node>("bo_publisher");
  auto p=n->create_publisher<std_msgs::msg::Float64>("bo",10);
  double A = 3.0 , f = 20.0 , pai = M_PI;  
  random_device rd; 
  unsigned int s =rd();
  mt19937 g(s); 
  normal_distribution <double> no(0.0, 0.01 * A);
  auto start = n->now();
    for(int i=0;rclcpp::ok();i++){
        auto now = n->now();
        double t = (now -start).seconds();
        double y = A*sin(2*pai*f*t);
        y +=  no(g);
        auto m = std_msgs::msg::Float64();
        m.data = y;
        RCLCPP_INFO(n->get_logger(),"t =%.3f y = %.3f",t,y);
        p ->publish(m) ;
    }
  
  
  
  
  return 0;
}