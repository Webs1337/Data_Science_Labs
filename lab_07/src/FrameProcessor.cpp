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
        case ProcessingMode::FACE:
            // Для режиму FACE сам кадр залишається оригінальним (NORMAL),
            // рамки будуть малюватися окремо через метод drawFaces у main.cpp.
            break;
        case ProcessingMode::NORMAL:
        default:
            break;
    }

    // Реакція на мишу (малюємо приціл, якщо натиснута ЛКМ)
    if (isMousePressed) {
        drawCrosshair(frame, mousePos);
    }

    // Завжди малюємо лічильник кадрів
    drawFPS(frame);
}

// НОВИЙ МЕТОД ДЛЯ ЛАБИ 7
void FrameProcessor::drawFaces(cv::Mat& frame, const std::vector<cv::Rect>& detectedFaces) {
    for (const auto& face : detectedFaces) {
        // Малюємо зелений прямокутник
        cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
        // Додаємо підпис "Face" над рамкою
        cv::putText(frame, "Face", cv::Point(face.x, face.y - 5), 
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
    }
}

// ЕФЕКТИ
void FrameProcessor::applyGlitch(cv::Mat& frame, int intensity) {
    std::vector<cv::Mat> channels;
    cv::split(frame, channels);

    int shift = intensity;
    if (shift > frame.cols / 2) shift = frame.cols / 2;

    if (shift > 0) {
        cv::Mat r_shifted = cv::Mat::zeros(channels[2].size(), channels[2].type());
        channels[2](cv::Rect(0, 0, frame.cols - shift, frame.rows))
            .copyTo(r_shifted(cv::Rect(shift, 0, frame.cols - shift, frame.rows)));
        channels[2] = r_shifted;

        cv::Mat b_shifted = cv::Mat::zeros(channels[0].size(), channels[0].type());
        channels[0](cv::Rect(shift, 0, frame.cols - shift, frame.rows))
            .copyTo(b_shifted(cv::Rect(0, 0, frame.cols - shift, frame.rows)));
        channels[0] = b_shifted;
    }

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

// UI ЕЛЕМЕНТИ
void FrameProcessor::drawCrosshair(cv::Mat& frame, cv::Point center) {
    cv::Scalar color(0, 0, 255);
    int len = 20;
    cv::line(frame, cv::Point(center.x - len, center.y), cv::Point(center.x + len, center.y), color, 2);
    cv::line(frame, cv::Point(center.x, center.y - len), cv::Point(center.x, center.y + len), color, 2);
    cv::circle(frame, center, len / 2, color, 2);
}

void FrameProcessor::drawFPS(cv::Mat& frame) {
    double currentTick = (double)cv::getTickCount();
    double timePassed = (currentTick - lastTick) / cv::getTickFrequency();
    lastTick = currentTick;

    double instantFPS = 1.0 / timePassed;

    fpsSum += instantFPS;
    frameCounter++;

    if (frameCounter >= 15) {
        currentFPS = (int)(fpsSum / frameCounter);
        frameCounter = 0;
        fpsSum = 0;
    }

    std::string fpsText = "FPS: " + std::to_string(currentFPS);
    cv::putText(frame, fpsText, cv::Point(10, 30), cv::FONT_HERSHEY_DUPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
}
