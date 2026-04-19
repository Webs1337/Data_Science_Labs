#include "Display.hpp"

Display::Display(const std::string& winName) : windowName(winName), mousePressed(false) {
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("Intensity", windowName, NULL, 50);
    cv::setTrackbarPos("Intensity", windowName, 10);

    cv::setMouseCallback(windowName, mouseCallback, this);
}

void Display::show(const cv::Mat& frame) {
    cv::imshow(windowName, frame);
}

int Display::getEffectIntensity() const {
    int pos = cv::getTrackbarPos("Intensity", windowName);
    return pos == 0 ? 1 : pos;
}

cv::Point Display::getMousePos() const { return mousePos; }
bool Display::isMousePressed() const { return mousePressed; }

void Display::mouseCallback(int event, int x, int y, int flags, void* userdata) {
    Display* display = reinterpret_cast<Display*>(userdata);
    
    if (event == cv::EVENT_MOUSEMOVE) {
        display->mousePos = cv::Point(x, y);
    } else if (event == cv::EVENT_LBUTTONDOWN) {
        display->mousePressed = true;
    } else if (event == cv::EVENT_LBUTTONUP) {
        display->mousePressed = false;
    }
}
