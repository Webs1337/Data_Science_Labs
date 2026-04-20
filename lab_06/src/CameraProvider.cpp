#include "CameraProvider.hpp"
#include <iostream>

CameraProvider::CameraProvider(int cameraId) {
    cap.open(cameraId, cv::CAP_ANY);
    if (!cap.isOpened()) {
        std::cerr << "Помилка: Не вдалося відкрити камеру ID: " << cameraId << std::endl;
    } else {
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    }
}

CameraProvider::~CameraProvider() {
    if (cap.isOpened()) {
        cap.release();
    }
}

bool CameraProvider::getFrame(cv::Mat& frame) {
    if (!cap.isOpened()) return false;
    cap >> frame;
    return !frame.empty();
}
