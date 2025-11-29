/*
 *  Proyecto: ProcessMemory
 *  Equipo:
 *  Rubén
 *  Aranza
 *  Laura
 *  Jesús
 */

#ifndef DOCUMENTOZU_PROCESSMEMORY_H
#define DOCUMENTOZU_PROCESSMEMORY_H

#include <list>
#include <queue>

inline int tamanioTotalM, tamanioMaximoP, cuantoProc, maxCuanto, administrador;

struct AProcess{
    int id;
    int cuanto;
    int memoria;
    int espacio;
    int dirBase;
};

class Proceso {
    AProcess atributos{};
    static int id;
    static bool espera;
    static AProcess *esperaProceso;
public:
    // <| constructores |>
    Proceso();
    explicit Proceso(int);
    // <| metodos del proceso |>
    void GenerarProceso();
    void decrementarCuanto();
    void imprimirProceso() const;
    // <| gets |>
    [[nodiscard]] int getId() const;
    [[nodiscard]] int getCuanto() const;
    [[nodiscard]] int getMemoria() const;
    [[nodiscard]] int getEspacio() const;
    [[nodiscard]] int getDirBase() const;

    [[nodiscard]] static int getIdEspera();
    [[nodiscard]] static int getCuantoEspera();
    [[nodiscard]] static int getMemoriaEspera();

    static bool getHayProcesoEsperando();
    static AProcess* getProcesoEsperando();
    // <| sets |>
    void setId(int);
    void setCuanto(int);
    void setMemoria(int);
    void setEspacio(int);
    void setDirBase(int);
    static void setHayProcesoEsperando(bool);
    static void setProcesoEsperando(AProcess*);
    // <| validacion |>
    [[nodiscard]] bool esEspacio() const;
};

class Memoria {
    // <| Estructuras de datos |>
    std::list<Proceso> listaProcesos;
    std::queue<Proceso*> colaProcesosRB;
    std::vector<int> D;                     //para el control de liberar en lazy
    // <| variables estaticas |>
    static const int tamanoMinimo; //Tamaño minimo del split
    // <| metodos privados de la memoria |>
    bool partirMemoria(std::list<Proceso>::iterator it, int tamRequerido);
    void liberarProceso(Proceso*);
public:
    // <| constructores |>
    Memoria();
    // <| metodos de la memoria |>
    static int calcularNivel(int);                    //para lazzy
    void asignarProceso();
    void juntar(const Proceso*);
    void roundRobin();
    void mostrar() const;
};

// <| Funcion global del programa |>
void processMemory();

#endif //DOCUMENTOZU_PROCESSMEMORY_H