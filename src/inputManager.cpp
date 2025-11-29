#include "processMemory/inputManager.h"
#include <iostream>

#ifdef _WIN32
#inlcude <conio.h>

InputManager::InputManager() = default;

InputManager::~InputManager() = default;

bool InputManager::teclaPresionada() {
    return _kbhit();
}

char InputManager::leerTecla() {
    return _getch();
}

#else
#include <unistd.h>
#include <sys/ioctl.h>
#include <cstdio>

InputManager::InputManager() {
    configurarTerminal();
}

InputManager::~InputManager() {
    restaurarTerminal();
}

void InputManager::configurarTerminal() {
    tcgetattr(STDIN_FILENO, &terminalOriginal);

    termios nuevaConfig = terminalOriginal;

    nuevaConfig.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &nuevaConfig);
}

void InputManager::restaurarTerminal() const {
    tcsetattr(STDIN_FILENO, TCSANOW, &terminalOriginal);
}

bool InputManager::teclaPresionada() {
    int bytesWaiting;
    ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);
    return bytesWaiting > 0;
}

char InputManager::leerTecla() {
    return static_cast<char>(getchar());
}

#endif
