#include <rclcpp/node.hpp>
#include <rmcs_executor/component.hpp>
#include <rmcs_utility/rclcpp/node_mixin.hpp>

#include <cv_bridge/cv_bridge.hpp>
#include <image_transport/image_transport.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <sensor_msgs/msg/image.hpp>

#include <memory>
#include <string>
#include <vector>

namespace rmcs::vision {

class ContourDetector : public rmcs_executor::Component,
                        public rclcpp::Node,
                        public rmcs_utility::NodeMixin {
public:
    ContourDetector()
        : Node("contour_detector",
               rclcpp::NodeOptions{}.automatically_declare_parameters_from_overrides(true)) {
        threshold_   = this->declare_parameter("threshold", 130);
        blur_radius_ = this->declare_parameter("blur_radius", 2);
        input_topic_ = this->declare_parameter("input_topic", std::string{"image_raw"});

        sub_ = this->create_subscription<sensor_msgs::msg::Image>(
            input_topic_, 10,
            [this](const sensor_msgs::msg::Image::ConstSharedPtr msg) { on_image(msg); });

        pub_ = this->create_publisher<sensor_msgs::msg::Image>("contour_result", 10);
    }

    auto update() -> void override {}

private:
    void on_image(const sensor_msgs::msg::Image::ConstSharedPtr msg) {
        cv::Mat frame = cv_bridge::toCvShare(msg, "bgr8")->image;

        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        cv::Size kSize(blur_radius_ * 2 + 1, blur_radius_ * 2 + 1);
        cv::blur(gray, gray, kSize);

        cv::Mat binary = gray > threshold_;

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

        cv::drawContours(frame, contours, -1, cv::Scalar(0, 255, 0), 2);

        auto out = cv_bridge::CvImage(msg->header, "bgr8", frame).toImageMsg();
        pub_->publish(*out);
    }

    int threshold_;
    int blur_radius_;
    std::string input_topic_;

    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr sub_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr pub_;
};

} // namespace rmcs::vision

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(rmcs::vision::ContourDetector, rmcs_executor::Component)
