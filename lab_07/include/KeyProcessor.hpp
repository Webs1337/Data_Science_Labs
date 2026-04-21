#pragma once

enum class ProcessingMode {
    NORMAL,
    GLITCH,
    SOBEL,
    PIXELATE,
    FACE
};

class KeyProcessor {
public:
    KeyProcessor();
    void processKey(int key);
    ProcessingMode getCurrentMode() const;
    bool shouldExit() const;

private:
    ProcessingMode currentMode;
    bool exitFlag;
};
