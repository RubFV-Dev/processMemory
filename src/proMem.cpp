/*
*  Proyecto: ProcessMemory
 *  Equipo:
 *  Rubén
 *  Aranza
 *  Laura
 *  Jesús
 */

#include "random"
#include "processMemory/proMem.h"
#include <iostream>
#include <ostream>
#include <queue>
#include <thread>
#include <chrono>

// Colores
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

std::queue<Proceso*> colaProcesos;

std::random_device rd;
std::mt19937 gen(rd());

// ================================================================================================================
// Clase proceso

//Dar a la variable estatica id el valor de 0
int Proceso::id =1;

//Constructor por defecto, all to 0
Proceso::Proceso() {atributos = {0,0,0,0,0};}

//Constructor para la incicializacion de la memoria
Proceso::Proceso(const int mem) {atributos = {0,0,mem,0,0};}

//Metodo para poner la memoria del proceso
void Proceso::setMemoria(const int mem) {atributos.memoria=mem;}

//Metodo para obtener la memoria del proceso
int Proceso::getMemoria() const {return atributos.memoria;}

//Metodo para poner la direccion base del proceso
void Proceso::setDirBase(const int dir) {atributos.dirBase=dir;}

//Metodo para obtener la dirección base del proceso
int Proceso::getDirBase() const {return atributos.dirBase;}

//Metodo para decrementar el cuánto en uno
void Proceso::decrementarCuanto() {atributos.cuanto = atributos.cuanto - cuantoProc;}

//Metodo para generar el proceso
void Proceso::GenerarProceso() {
    atributos.id = id;
    std::uniform_int_distribution distribution1(1,tamanioMaximoP);
    std::uniform_int_distribution distribution2(1,maxCuanto);
    atributos.memoria=distribution1(gen);
    atributos.cuanto=distribution2(gen);
    id++;
}

void Proceso::setId(const int i) {atributos.id = i;}

int Proceso::getId() const {return atributos.id;}

void Proceso::setEspacio(const int espacio) {atributos.espacio=espacio;}


//Metodo para imrprimir el proceso
void Proceso::imprimirProceso() const {
    if (esEspacio()) {
        // Magenta para los huecos (libres)
        std::cout << MAGENTA << "[" << atributos.id << "," << atributos.memoria << "," << atributos.cuanto << "]" << RESET;
    } else {
        // Cian para los procesos (ocupados)
        std::cout << CYAN << "[" << atributos.id << "," << atributos.memoria<< "(" << atributos.espacio << ")" << "," << atributos.cuanto << "]" << RESET;
    }
}

bool Proceso::esEspacio() const {return atributos.id==0 && atributos.cuanto==0;}

int Proceso::getCuanto() const {return atributos.cuanto;}



// Clase proceso
// ================================================================================================================
// Clase Memoria

const int Memoria::tamanoMinimo = 32;

Memoria::Memoria() {
    const Proceso totMemoria(tamanioTotalM);
    listaProcesos.insert(listaProcesos.begin(),totMemoria);
}

void Memoria::mostrar() const {
    std::cout << BOLD << "  ➤ Estado de la Memoria:" << RESET << "\n      ";
    for (Proceso proceso : listaProcesos) {
        proceso.imprimirProceso();
        std::cout << " "; // Añade espacio entre bloques
    }
    std::cout << std::endl;
}

void Memoria::asignarProceso() {
    Proceso newProceso;
    newProceso.GenerarProceso();

    std::cout << BOLD << "  [+] Nuevo Proceso Generado:" << RESET << "\n";
    std::cout << "      └── ID: " << GREEN << newProceso.getId() << RESET
              << " | Req: " << GREEN << newProceso.getMemoria() << " KB" << RESET
              << " | Quantum: " << GREEN << newProceso.getCuanto() << RESET << "\n";

    auto mejorUbi = listaProcesos.end();


    for(auto itr = listaProcesos.begin(); itr != listaProcesos.end(); ++itr) {
        if (itr->esEspacio() && itr->getMemoria() >= newProceso.getMemoria()) {
            if (mejorUbi == listaProcesos.end() || itr->getMemoria() < mejorUbi->getMemoria()) {
                mejorUbi = itr;
            }
        }
    }
    if (mejorUbi == listaProcesos.end()) {
        std::cout << "      └── " << RED << "Estado: No hay espacio (Rechazado) ❌" << RESET << "\n";
        return;
    }

    partirMemoria(mejorUbi,newProceso.getMemoria());

    const int newBase = mejorUbi->getDirBase();
    const int newCapa = mejorUbi->getMemoria();

    newProceso.setDirBase(newBase);

    *mejorUbi = newProceso;

    mejorUbi->setEspacio(newCapa);
    colaProcesosRB.push(&*mejorUbi);
}

void Memoria::liberarProceso(Proceso * proc) {
    const int dirBase = proc->getDirBase();
    const int memoria = proc->getMemoria();
    *proc = Proceso(memoria);
    proc->setDirBase(dirBase);
    juntar(proc);
}


void Memoria::roundRobin() {
    if (colaProcesosRB.empty()) {
        std::cout << "  " << BOLD << "➤ Atendiendo CPU:" << RESET << "\n";
        std::cout << "      " << BOLD << "CPU Ociosa (Cola vacía) 😴" << RESET << "\n";
        return;
        return;
    }
    std::cout << "  " << BOLD << "➤ Cola de Listos (Round Robin):" << RESET << "\n      ";
    std::cout << "IN <-- ";
    for (int i = 0 ; i < colaProcesosRB.size(); i++) {
        auto proceso = colaProcesosRB.front();
        std::cout << YELLOW << "[" << proceso->getId() << "]" << RESET << " ";
        colaProcesosRB.pop();
        colaProcesosRB.push(proceso);
    }
    std::cout << "<-- OUT\n";

    Proceso* proc = colaProcesosRB.front();

    colaProcesosRB.pop();

    std::cout << "  " << BOLD << "➤ Atendiendo CPU:" << RESET << "\n";
    std::cout << "      🚀 " << BOLD << "Proceso " << proc->getId() << RESET
              << " (Q=" << proc->getCuanto() << ") -> ";

    proc->decrementarCuanto();

    if (proc->getCuanto() == 0) {
        std::cout << RED << "Quantum Finalizado. Liberando memoria." << RESET << "\n";
        liberarProceso(proc);
    } else {
        std::cout << YELLOW << "Quantum restante: " << proc->getCuanto() << ". Regresa a la cola." << RESET << "\n";
        colaProcesosRB.push(proc);
    }
}


bool Memoria::partirMemoria(std::list<Proceso>::iterator it, int tamRequerido){
    int tamActual = it->getMemoria();
    int dirBase = it->getDirBase();
    
    //Verificar si es necesario partir la memoria
    while(tamActual / 2 >= tamRequerido && tamActual / 2 >= tamanoMinimo){
        tamActual /= 2;
        
        //Crear Buddies
        Proceso bloque1(tamActual);
        bloque1.setDirBase(dirBase);
        
        Proceso bloque2(tamActual);
        bloque2.setDirBase(dirBase + tamActual);
        
        //Reemplazar el bloque original con los Buddies
        *it = bloque1;
        listaProcesos.insert(std::next(it), bloque2);
        
        //Actualizar variables para seguir dividiendo
        tamActual = it->getMemoria();
        dirBase = it->getDirBase();
    }
    return true;
}

void Memoria::juntar(Proceso* proceso) {
    int dirBuddie = proceso->getDirBase() ^ proceso->getMemoria();
    for (auto itr = listaProcesos.begin(); itr != listaProcesos.end(); ++itr) {
        if (itr->getDirBase() == dirBuddie && itr->getMemoria() == proceso->getMemoria() && itr->getId() == 0)  {
            listaProcesos.erase(itr);
        }
    }
}


// Clase Memoria
// ================================================================================================================
// Funcion ProcessMemory

void processMemory() {

    int ciclo = 1;
    using namespace std::chrono_literals;



    std::cout << YELLOW << "➤" << RESET << BOLD <<" Ingrese el tamanio de la memoria total (KB): " << RESET;
    std::cin >> tamanioTotalM;
    std::cout << YELLOW << "➤" << RESET << BOLD <<" Ingrese el tamanio maximo de la memoria de los procesos (KB): " << RESET;
    std::cin >> tamanioMaximoP;
    while (tamanioMaximoP > tamanioTotalM) {
        std::cout << RED << "Ingrese el un valor no mas grande que el tamanio de la memoria total (" << tamanioTotalM << "): " << RESET;
        std::cin >> tamanioMaximoP;
    }
    std::cout << YELLOW << "➤" << RESET << BOLD <<" Ingrese el cuanto de procesamiento a asignar por ciclo: " << RESET;
    std::cin >> cuantoProc;
    std::cout << YELLOW << "➤" << RESET << BOLD <<" Ingrese el cuanto maximo de los procesos: " << RESET;
    std::cin >> maxCuanto;
    while (maxCuanto < cuantoProc) {
        std::cout << RED << "Ingrese el un valor no mas chico que el cuanto de procesamiento (" << cuantoProc << "): " << RESET;
        std::cin >> maxCuanto;
    }
    std::cout << std::endl;

    Memoria memoria;

    std::cout << BOLD << "--- INICIO DE SIMULACIÓN (Buddy System + RR) ---" << RESET << std::endl;
    memoria.mostrar();
    while(true) {

        std::cout << "\n" << YELLOW;
        std::cout << "╔══════════════════════════════════════════════════════════╗\n";
        // Añade un 0 para padding (ej: 01, 02... 10, 11)
        std::cout << "║                     ⚡  CICLO " << (ciclo < 10 ? "0" : "") << ciclo << "  ⚡                       ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════╝\n" << RESET;
        std::cout << "\n";
        memoria.asignarProceso();
        std::cout << "\n";
        memoria.mostrar();
        std::cout << "\n";
        memoria.roundRobin();
        std::cout << "\n" << "────────────────────────────────────────────────────────────\n";
        ciclo++;
        std::this_thread::sleep_for(5s);
    }
}

