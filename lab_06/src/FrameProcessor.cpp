#include "FrameProcessor.hpp"
#include <vector>

FrameProcessor::FrameProcessor() {
    lastTick = (double)cv::getTickCount();
}

void FrameProcessor::process(cv::Mat& frame, ProcessingMode mode, int intensity, cv::Point mousePos, bool isMousePressed) {
    
    switch (mode) {
        case ProcessingMode::GLITCH:
            applyGlitch(frame, intensity);
            break;
        case ProcessingMode::SOBEL:
            applySobel(frame, intensity);
            break;
        case ProcessingMode::PIXELATE:
            applyPixelate(frame, intensity);
            break;
        case ProcessingMode::NORMAL:
        default:
            break;
    }

    if (isMousePressed) {
        drawCrosshair(frame, mousePos);
    }

    drawFPS(frame);
}

void FrameProcessor::applyGlitch(cv::Mat& frame, int intensity) {
    std::vector<cv::Mat> channels;
    cv::split(frame, channels);

    int shift = intensity;
    cv::Mat trans_mat_R = (cv::Mat_<double>(2, 3) << 1, 0, shift, 0, 1, 0);
    cv::Mat trans_mat_B = (cv::Mat_<double>(2, 3) << 1, 0, -shift, 0, 1, 0);

    cv::warpAffine(channels[2], channels[2], trans_mat_R, channels[2].size());
    cv::warpAffine(channels[0], channels[0], trans_mat_B, channels[0].size());

    cv::merge(channels, frame);
}

void FrameProcessor::applySobel(cv::Mat& frame, int intensity) {
    cv::Mat gray, grad_x, grad_y, abs_grad_x, abs_grad_y;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);

    cv::Sobel(gray, grad_x, CV_16S, 1, 0, 3);
    cv::Sobel(gray, grad_y, CV_16S, 0, 1, 3);

    cv::convertScaleAbs(grad_x, abs_grad_x, intensity * 0.1);
    cv::convertScaleAbs(grad_y, abs_grad_y, intensity * 0.1);

    cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, frame);
    cv::cvtColor(frame, frame, cv::COLOR_GRAY2BGR);
}

void FrameProcessor::applyPixelate(cv::Mat& frame, int intensity) {
    int pixelSize = intensity; 
    cv::Mat temp;
    cv::resize(frame, temp, cv::Size(frame.cols / pixelSize, frame.rows / pixelSize), 0, 0, cv::INTER_LINEAR);
    cv::resize(temp, frame, frame.size(), 0, 0, cv::INTER_NEAREST);
}

void FrameProcessor::drawCrosshair(cv::Mat& frame, cv::Point center) {
    cv::Scalar color(0, 0, 255);
    int len = 20;
    cv::line(frame, cv::Point(center.x - len, center.y), cv::Point(center.x + len, center.y), color, 2);
    cv::line(frame, cv::Point(center.x, center.y - len), cv::Point(center.x, center.y + len), color, 2);
    cv::circle(frame, center, len / 2, color, 2);
}

void FrameProcessor::drawFPS(cv::Mat& frame) {
    double currentTick = (double)cv::getTickCount();
    double fps = cv::getTickFrequency() / (currentTick - lastTick);
    lastTick = currentTick;

    std::string fpsText = "FPS: " + std::to_string((int)fps);
    cv::putText(frame, fpsText, cv::Point(10, 30), cv::FONT_HERSHEY_DUPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
}
