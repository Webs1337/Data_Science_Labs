#pragma once
#include <opencv2/opencv.hpp>
#include "KeyProcessor.hpp"
#include <vector>

class FrameProcessor {
public:
    FrameProcessor();
    void process(cv::Mat& frame, ProcessingMode mode, int intensity, cv::Point mousePos, bool isMousePressed);
    
    void drawFaces(cv::Mat& frame, const std::vector<cv::Rect>& detectedFaces);

private:
    double lastTick;
    
    int frameCounter = 0;
    double fpsSum = 0;
    int currentFPS = 0;

    void drawFPS(cv::Mat& frame);
    void drawCrosshair(cv::Mat& frame, cv::Point center);
    
    void applyGlitch(cv::Mat& frame, int intensity);
    void applySobel(cv::Mat& frame, int intensity);
    void applyPixelate(cv::Mat& frame, int intensity);
};
