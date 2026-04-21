#include "KeyProcessor.hpp"

KeyProcessor::KeyProcessor() : currentMode(ProcessingMode::NORMAL), exitFlag(false) {}

void KeyProcessor::processKey(int key) {
    if (key == -1) return; 

    switch (key) {
        case '1': currentMode = ProcessingMode::NORMAL; break;
        case '2': currentMode = ProcessingMode::GLITCH; break;
        case '3': currentMode = ProcessingMode::SOBEL; break;
        case '4': currentMode = ProcessingMode::PIXELATE; break;
        case 'F': currentMode = ProcessingMode::FACE; break; // Клавіша F (додав і велику, і маленьку)
        case 'q':
        case 'Q':
        case 27: // ESC
            exitFlag = true;
            break;
    }
}

ProcessingMode KeyProcessor::getCurrentMode() const { 
    return currentMode; 
}

bool KeyProcessor::shouldExit() const { 
    return exitFlag; 
}
