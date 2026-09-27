#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
typedef vector<vector<cv::Point>>Contours;

int thresholdVaule =127 ;
int para4Boxfilter =2 ;
int main(){
    cv::Mat frame = cv::imread("/workspaces/RMCS/WarHammer_40k.jpg");
    if(!frame.data)
        return -1;
    cv::Mat tempImage = frame.clone();
    Contours contours;
    cv::Size kSize(para4Boxfilter * 2 +1 ,para4Boxfilter *2 +1);
    cv::cvtColor(tempImage, tempImage, cv::COLOR_BGR2GRAY);
    cv::blur(tempImage, tempImage, kSize);
    cv::Mat edges;
    cv::Canny(tempImage, edges, 50, 150);
    cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    cv::imwrite("edges.jpg", edges);
    cv::Mat contourLines = cv::Mat::zeros(edges.size(), CV_8UC3);
    cv::drawContours(contourLines, contours, -1, cv::Scalar(255,255,255), 1);
    cv::imwrite("contour_lines.jpg", contourLines);
    cv::drawContours(frame, contours, -1, cv::Scalar(0, 255, 0), 2);

    cv::imshow("binary", tempImage);
    cv::imshow("result", frame);
    cv::imwrite("binary_out.jpg", tempImage);
    cv::imwrite("result_out.jpg", frame);
    cv::waitKey(0);
    return 0;
}