#include "FaceDetector.hpp"
#include <iostream>
#include <chrono>

FaceDetector::FaceDetector(const std::string& prototxt, const std::string& model) {
    try {
        net = cv::dnn::readNetFromCaffe(prototxt, model);
        loaded = true;
        running = true;
        workerThread = std::thread(&FaceDetector::workerLoop, this);
        std::cout << "[FaceDetector] Модель успішно завантажена." << std::endl;
    } catch (const cv::Exception& e) {
        std::cerr << "[FaceDetector] Помилка завантаження моделі: " << e.what() << std::endl;
        loaded = false;
    }
}

FaceDetector::~FaceDetector() {
    running = false;
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void FaceDetector::updateFrame(const cv::Mat& frame) {
    std::lock_guard<std::mutex> lock(mtx);
    frameToProcess = frame.clone();
    hasNewFrame = true;
}

std::vector<cv::Rect> FaceDetector::getDetectedFaces() {
    std::lock_guard<std::mutex> lock(mtx);
    return faces;
}

void FaceDetector::workerLoop() {
    while (running) {
        cv::Mat img;
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (hasNewFrame) {
                img = frameToProcess.clone();
                hasNewFrame = false;
            }
        }

        if (img.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        // Рівень 2: Штучне навантаження (500мс), щоб довести роботу потоків
        // std::this_thread::sleep_for(std::chrono::milliseconds(500));

        int h = img.rows;
        int w = img.cols;
        cv::Mat blob = cv::dnn::blobFromImage(img, 1.0, cv::Size(300, 300), cv::Scalar(104.0, 177.0, 123.0));
        
        net.setInput(blob);
        cv::Mat detections = net.forward();
        
        cv::Mat detectionMat(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());
        std::vector<cv::Rect> foundFaces;

        for (int i = 0; i < detectionMat.rows; i++) {
            float confidence = detectionMat.at<float>(i, 2);
            if (confidence > 0.5) {
                int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * w);
                int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * h);
                int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * w);
                int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * h);
                
                foundFaces.push_back(cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2)));
            }
        }

        {
            std::lock_guard<std::mutex> lock(mtx);
            faces = foundFaces;
        }
    }
}
