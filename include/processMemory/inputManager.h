//
// Created by rubfv on 28/11/25.
//

#ifndef SOPROY_INPUTMANAGER_H
#define SOPROY_INPUTMANAGER_H

#ifdef  _WIN32

#else
    #include <termios.h>
#endif

class InputManager {
    #ifndef _WIN32
    termios terminalOriginal = {};
    void configurarTerminal();
    void restaurarTerminal() const;
    #endif

public:
    InputManager();
    ~InputManager();

    static bool teclaPresionada();
    static char leerTecla();
};

#endif //SOPROY_INPUTMANAGER_H