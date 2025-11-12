//
// Created by rubfv on 11/11/25.
//
#include "random"
#include "processMemory/proMem.h"

#include <iostream>
#include <ostream>
#include <queue>

std::queue<Proceso*> colaProcesos;

std::random_device rd;
std::mt19937 gen(rd());

// ================================================================================================================
// Clase proceso

//Dar a la variable estatica id el valor de 0
int Proceso::id =0;

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

//Metodo para imrprimir el proceso
void Proceso::imprimirProceso() const {
    std::cout<< "[" << atributos.id << "," << atributos.memoria << "," << atributos.cuanto << "]" <<std::endl;
}

bool Proceso::esNada() const {return atributos.id==0 && atributos.cuanto==0;}

int Proceso::getCuanto() const {return atributos.cuanto;}



// Clase proceso
// ================================================================================================================
// Clase Memoria

void Memoria::roundRobin() {

}

// Clase Memoria
// ================================================================================================================
// Funcion ProcessMemory

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
