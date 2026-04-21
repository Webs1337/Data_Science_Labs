#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

class FaceDetector {
public:
    FaceDetector(const std::string& prototxt, const std::string& model);
    ~FaceDetector();

    void updateFrame(const cv::Mat& frame);
    std::vector<cv::Rect> getDetectedFaces();
    bool isLoaded() const { return loaded; }

private:
    void workerLoop();

    cv::dnn::Net net;
    bool loaded = false;

    std::thread workerThread;
    std::mutex mtx;
    std::atomic<bool> running{false};

    cv::Mat frameToProcess;
    bool hasNewFrame = false;
    std::vector<cv::Rect> faces;
};
