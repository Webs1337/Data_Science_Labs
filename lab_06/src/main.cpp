#include "CameraProvider.hpp"
#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"
#include "Display.hpp"

int main() {
    CameraProvider camera(0);
    KeyProcessor keyProc;
    FrameProcessor frameProc;
    Display display("Lab 6 - OpenCV");

    cv::Mat frame;

    while (!keyProc.shouldExit()) {
        if (!camera.getFrame(frame)) {
            break;
        }

        int intensity = display.getEffectIntensity();
        cv::Point mousePos = display.getMousePos();
        bool isPressed = display.isMousePressed();
        
        frameProc.process(frame, keyProc.getCurrentMode(), intensity, mousePos, isPressed);
        
        display.show(frame);

        int key = cv::waitKey(1) & 0xFF; 
        if (key != 255) {
            keyProc.processKey(key);
        }
    }

    return 0;
}
