#pragma once
#include <opencv2/opencv.hpp>
#include <string>

class Display {
public:
    Display(const std::string& windowName);
    void show(const cv::Mat& frame);
    int getEffectIntensity() const;
    cv::Point getMousePos() const;
    bool isMousePressed() const;

private:
    std::string windowName;
    cv::Point mousePos;
    bool mousePressed;

    static void mouseCallback(int event, int x, int y, int flags, void* userdata);
};
