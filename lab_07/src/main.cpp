#include "CameraProvider.hpp"
#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"
#include "Display.hpp"
#include "FaceDetector.hpp"

int main() {
    CameraProvider camera(0);
    KeyProcessor keyProc;
    FrameProcessor frameProc;
    Display display("Cyber Lab 7 - Multi-thread Face Detection");

    // Ініціалізація нейромережі
    FaceDetector faceDet("../deploy.prototxt", "../res10_300x300_ssd_iter_140000.caffemodel");

    cv::Mat frame;

    while (!keyProc.shouldExit()) {
        if (!camera.getFrame(frame)) break;

        ProcessingMode mode = keyProc.getCurrentMode();

        // Якщо режим FACE, відправляємо кадр у фоновий потік
        if (mode == ProcessingMode::FACE && faceDet.isLoaded()) {
            faceDet.updateFrame(frame);
        }

        int intensity = display.getEffectIntensity();
        cv::Point mousePos = display.getMousePos();
        bool isPressed = display.isMousePressed();
        
        // Виконуємо стандартну обробку (ефекти 6-ї лаби)
        frameProc.process(frame, mode, intensity, mousePos, isPressed);
        
        // Якщо режим FACE, забираємо ОСТАННІ знайдені координати
        if (mode == ProcessingMode::FACE) {
            std::vector<cv::Rect> faces = faceDet.getDetectedFaces();
            frameProc.drawFaces(frame, faces);
        }
        
        display.show(frame);

        int key = cv::waitKey(1) & 0xFF; 
        if (key != 255) {
            keyProc.processKey(key);
        }
    }

    return 0;
}
