//
// Created by rubfv on 11/11/25.
//
#include "random"
#include "processMemory/proMem.h"

#include <iostream>
#include <ostream>
#include <queue>
#include <thread>
#include <chrono>

std::queue<Proceso*> colaProcesos;

std::random_device rd;
std::mt19937 gen(rd());

// ================================================================================================================
// Clase proceso

//Dar a la variable estatica id el valor de 0
int Proceso::id =1;

//Constructor por defecto, all to 0
Proceso::Proceso() {atributos = {0,0,0,0};}

//Constructor para la incicializacion de la memoria
Proceso::Proceso(const int mem) {atributos = {0,0,mem,0};}

//Metodo para poner la memoria del proceso
void Proceso::setMemoria(const int mem) {atributos.memoria=mem;}

//Metodo para obtener la memoria del proceso
int Proceso::getMemoria() const {return atributos.memoria;}

//Metodo para poner la direccion base del proceso
void Proceso::setDirBase(const int dir) {atributos.dirBase=dir;}

//Metodo para obtener la dirección base del proceso
int Proceso::getDirBase() const {return atributos.dirBase;}

//Metodo para decrementar el cuánto en uno
void Proceso::decrementarCuanto() {atributos.cuanto--;}

//Metodo para generar el proceso
void Proceso::GenerarProceso() {
    atributos.id = id;
    std::uniform_int_distribution distribution1(1,2048);
    std::uniform_int_distribution distribution2(1,10);
    atributos.memoria=distribution1(gen);
    atributos.cuanto=distribution2(gen);
    atributos.dirBase=0;
    id++;
}

void Proceso::setId(const int i) {atributos.id = i;}

int Proceso::getId() const {return atributos.id;}



//Metodo para imrprimir el proceso
void Proceso::imprimirProceso() const {
    std::cout<< "[" << atributos.id << "," << atributos.memoria << "," << atributos.cuanto << "]";
}

bool Proceso::esNada() const {return atributos.id==0 && atributos.cuanto==0;}

int Proceso::getCuanto() const {return atributos.cuanto;}



// Clase proceso
// ================================================================================================================
// Clase Memoria

const int Memoria::tamanoMinimo = 32;

Memoria::Memoria() {
    const Proceso totMemoria(4096);
    listaProcesos.insert(listaProcesos.begin(),totMemoria);
}

void Memoria::mostrar() {
    for (Proceso proceso : listaProcesos) {
        proceso.imprimirProceso();
    }
    std::cout << std::endl;
}

void Memoria::asignarProceso() {
    Proceso newProceso;
    newProceso.GenerarProceso();
    newProceso.imprimirProceso(); std::cout << ":" << std::endl;

    auto mejorUbi = listaProcesos.end();


    for(auto itr = listaProcesos.begin(); itr != listaProcesos.end(); ++itr) {
        if (itr->esNada() && itr->getMemoria() >= newProceso.getMemoria()) {
            if (mejorUbi == listaProcesos.end() || itr->getMemoria() < mejorUbi->getMemoria()) {
                mejorUbi = itr;
            }
        }
    }
    if (mejorUbi == listaProcesos.end()) return;

    partirMemoria(mejorUbi,newProceso.getMemoria());

    int newBase = mejorUbi->getDirBase();
    int newCapa = mejorUbi->getMemoria();

    newProceso.setDirBase(newBase);

    *mejorUbi = newProceso;

    mejorUbi->setMemoria(newCapa);
    colaProcesosRB.push(&*mejorUbi);
}

void Memoria::liberarProceso(Proceso * proc) {
    const int memoria = proc->getMemoria();
    *proc = Proceso(memoria);
}


void Memoria::roundRobin() {
    if (colaProcesosRB.empty()) {
        return;
    }
    std::cout << "<-";
    for (int i = 0 ; i < colaProcesosRB.size(); i++) {
        auto proceso = colaProcesosRB.front();
        std::cout << "(" <<proceso->getId() << ")";
        colaProcesosRB.pop();
        colaProcesosRB.push(proceso);
    }
    std::cout << "<-" << std::endl << std::endl;

    Proceso* proc = colaProcesosRB.front();

    colaProcesosRB.pop();

    proc->decrementarCuanto();

    if (proc->getCuanto() == 0) {
        liberarProceso(proc);
    } else {
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

// Clase Memoria
// ================================================================================================================
// Funcion ProcessMemory

void processMemory() {
    Memoria memoria;

    using namespace std::chrono_literals;
    memoria.mostrar();
    while(true) {
        memoria.asignarProceso();
        memoria.mostrar();
        memoria.roundRobin();
        std::this_thread::sleep_for(5s);
    }
}

