//
// Created by rubfv on 11/11/25.
//

#ifndef DOCUMENTOZU_PROCESSMEMORY_H
#define DOCUMENTOZU_PROCESSMEMORY_H

#include <list>
#include <queue>


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
    void setId(int);
    [[nodiscard]] int getId() const;
};

class Memoria {
    std::list<Proceso> listaProcesos;
    std::queue<Proceso*> colaProcesosRB;
    static const int tamanoMinimo; //Tamaño minimo del split
    bool partirMemoria(std::list<Proceso>::iterator it, int tamRequerido);
    void liberarProceso(Proceso*);
public:
    Memoria();
    void asignarProceso();
    void juntar();
    void roundRobin();
    void mostrar();
};

[[noreturn]] void processMemory();

#endif //DOCUMENTOZU_PROCESSMEMORY_H