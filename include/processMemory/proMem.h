//
// Created by rubfv on 11/11/25.
//


#ifndef DOCUMENTOZU_PROCESSMEMORY_H
#define DOCUMENTOZU_PROCESSMEMORY_H

#include <list>



struct AProcess{
    int id;
    int cuanto;
    int memoria;
    int dirBase;
};

class Proceso {
    AProcess atributos{};
    static int id;
public:
    Proceso();
    explicit Proceso(int);
    void GenerarProceso();
    void setMemoria(int);
    [[nodiscard]] int getMemoria() const;
    void setDirBase(int);
    [[nodiscard]] int getDirBase() const;
    void decrementarCuanto();
    void imprimirProceso() const;
    [[nodiscard]] bool esNada() const;
    [[nodiscard]] int getCuanto() const;
};

class Memoria {
    std::list<Proceso> listaProcesos;
public:
    void partirMemoria();
    void asignarProceso();
    void liberarProceso(std::list<Proceso>::iterator);
    void juntar();
    void roundRobin();
};

[[noreturn]] void processMemory();

#endif //DOCUMENTOZU_PROCESSMEMORY_H