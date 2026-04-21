#pragma once
#include <opencv2/opencv.hpp>

class CameraProvider {
public:
    CameraProvider(int cameraId = 0);
    ~CameraProvider();
    bool getFrame(cv::Mat& frame);

private:
    cv::VideoCapture cap;
};
