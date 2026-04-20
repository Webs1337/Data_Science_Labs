#pragma once
#include <opencv2/opencv.hpp>
#include "KeyProcessor.hpp"

class FrameProcessor {
public:
    FrameProcessor();
    void process(cv::Mat& frame, ProcessingMode mode, int intensity, cv::Point mousePos, bool isMousePressed);

private:
    double lastTick;
    void drawFPS(cv::Mat& frame);
    void drawCrosshair(cv::Mat& frame, cv::Point center);
    
    void applyGlitch(cv::Mat& frame, int intensity);
    void applySobel(cv::Mat& frame, int intensity);
    void applyPixelate(cv::Mat& frame, int intensity);
};
