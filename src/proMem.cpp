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
#include <windows.h>

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
//Para procesos en espera
bool Proceso::espera=false;         //no hay procesos esperando
AProcess* Proceso::esperaProceso=nullptr;

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

//Para proceso en espera
bool Proceso::getHayProcesoEsperando(){ return espera; }
void Proceso::setHayProcesoEsperando(bool estado) { espera= estado; }

AProcess* Proceso::getProcesoEsperando() { return esperaProceso; }
void Proceso::setProcesoEsperando(AProcess* proc) { esperaProceso = proc; }

int Proceso::getIdEspera() {return esperaProceso->id;}
int Proceso::getCuantoEspera() {return esperaProceso->cuanto;}
int Proceso::getMemoriaEspera() {return esperaProceso->memoria;}

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

void Proceso::setCuanto(const int c) {atributos.cuanto = c;}

int Proceso::getId() const {return atributos.id;}

int Proceso::getEspacio()const {return atributos.espacio;}

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

    int niveles = log2(tamanioTotalM / tamanoMinimo) + 1;       //el vector con el tamaño de niveles
    D = std::vector<int>(niveles, 0);
}

void Memoria::mostrar() const {
    std::cout << BOLD << "  ➤ Estado de la Memoria:" << RESET << "\n      ";
    for (Proceso proceso : listaProcesos) {
        proceso.imprimirProceso();
        std::cout << " "; // Añade espacio entre bloques
    }
    std::cout << std::endl;
}

int Memoria::calcularNivel(int tam) {       //regresa el numero de la posicion en donde debe ir ese bloque
    return log2(tam/tamanoMinimo);
}

void Memoria::asignarProceso() {
    Proceso newProceso;
    if (!Proceso::getHayProcesoEsperando()) {
        newProceso.GenerarProceso();
        std::cout << BOLD << "  [+] Nuevo Proceso Generado:" << RESET << "\n";
    }
    else {
        newProceso.setCuanto(Proceso::getCuantoEspera());
        newProceso.setMemoria(Proceso::getMemoriaEspera());
        newProceso.setId(Proceso::getIdEspera());
        std::cout << BOLD << "  [+] Proceso En espera:" << RESET << "\n";
    }

    std::cout << "      └── ID: " << GREEN << newProceso.getId() << RESET
                  << " | Req: " << GREEN << newProceso.getMemoria() << " KB" << RESET
                  << " | Quantum: " << GREEN << newProceso.getCuanto() << RESET << "\n";


    auto mejorUbi = listaProcesos.end();                                                //encontro un bloque existente para ingresar el proceso(quizas puede partirse)
    for(auto itr = listaProcesos.begin(); itr != listaProcesos.end(); ++itr) {
        if (itr->esEspacio() && itr->getMemoria() >= newProceso.getMemoria()) {
            if (mejorUbi == listaProcesos.end() || itr->getMemoria() < mejorUbi->getMemoria()) {
                mejorUbi = itr;
            }
        }
    }

    if (mejorUbi == listaProcesos.end()) {
        std::cout << "      └── " << RED << "Estado: No hay espacio (Rechazado) ❌" << RESET << "\n";
        if (!Proceso::getHayProcesoEsperando()) {
            AProcess* esperando= new AProcess();
            esperando->id = newProceso.getId();
            esperando->cuanto = newProceso.getCuanto();
            esperando->memoria=newProceso.getMemoria();
            Proceso::setHayProcesoEsperando(true);
            Proceso::setProcesoEsperando(esperando);

            std::cout << "      └── " << YELLOW << "Proceso " << newProceso.getId()
          << " guardado en espera ⏳" << RESET << "\n";
        }
        return;
    }

    if (Proceso::getHayProcesoEsperando()) {//ya no hay esperando
        delete Proceso::getProcesoEsperando();
        Proceso::setProcesoEsperando(nullptr);
        Proceso::setHayProcesoEsperando(false);
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
    const int memoria = proc->getEspacio();     //Espacio real que ocupa
    *proc = Proceso(memoria);
    proc->setDirBase(dirBase);
    if (administrador==1) {//es buddy normal
        juntar(proc);
    }
    else {                  //si no es buddy normal es lazy
         int nivel=calcularNivel(memoria);
        if (D[nivel]>=2) {
            D[nivel]-=2;
        }
        else if (D[nivel]==1) {
            D[nivel]=0;
            juntar(proc);       //intenta juntarlo con su buddy
        }
        else {                  //D en 0
            D[nivel]=0;
            juntar(proc);       //intentamos liberar globalmente este bloque liberado
            // y buscamos un bloque extra de este tamaño que pueda unirse con su buddy
            for ( auto it=listaProcesos.begin();it!=listaProcesos.end();it++) {
                if (it->getId()==0 && it->getMemoria()==memoria) {              //si esta vacío y es del tamaño que buscamos
                    juntar(&*it);
                    break;
                }
            }
        }
    }
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

    if (proc->getCuanto() <= 0) {
        std::cout << RED << "Quantum Finalizado. Liberando memoria." << RESET << "\n";
        liberarProceso(proc);
    } else {
        std::cout << YELLOW << "Quantum restante: " << proc->getCuanto() << ". Regresa a la cola." << RESET << "\n";
        colaProcesosRB.push(proc);
    }
}


bool Memoria::partirMemoria(std::list<Proceso>::iterator it, int tamRequerido){
    int tamActual = it->getMemoria();
    int copiaTamActual = tamActual;
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
    int nivel= calcularNivel(tamActual);

    if (tamActual!=copiaTamActual) {        //si el tamaño es distinto quiere decir que si tuvo que partir
        D[nivel]++;                            //asignación global
    }
    else {
        D[nivel]+=2;
    }
    return true;
}

void Memoria::juntar(Proceso* proceso) {
    int dirBuddie = proceso->getDirBase() ^ proceso->getMemoria();                  // direccion de su buddi
    auto itBuddie = listaProcesos.end();
    for (auto itr = listaProcesos.begin(); itr != listaProcesos.end(); ++itr) {
        if (itr->getDirBase() == dirBuddie && itr->getMemoria() == proceso->getMemoria() && itr->esEspacio())  {
            // encontro a su buddi y lo guardo para usarlo después
            itBuddie=itr;
            break;
        }
    }
    if (itBuddie == listaProcesos.end()) {          //no encontro a su buddi
        return;
    }
    // calculamos la nueva direccion
    int nuevaDir=std::min( proceso->getDirBase(), itBuddie->getDirBase()); //sera la mas pequeña de las dos
    int nuevoTam=itBuddie->getMemoria()*2;                      //ya que ambos tienen la misma memoria

    //nuevo bloque unido
    Proceso unido(nuevoTam);
    unido.setDirBase(nuevaDir);

    //como tenemos que eliminar en la lista necesitamos la posicion del proceso que se mando
    auto itProceso = listaProcesos.begin();
    for (; itProceso != listaProcesos.end(); ++itProceso) {
        if (&*itProceso == proceso) break;
    }

    auto itN=listaProcesos.insert(itBuddie, unido);          //insertar despues del buddi(para mantener posicion)

    if (administrador==1) {             //si es buddy normal sigue juntando hasta mas no poder
        juntar(&*itN);          //recursivo por si el buddy del nuevo bloque unido esta libre y asi
    }

    //eliminar los bloques que se unieron
    listaProcesos.erase(itProceso);
    listaProcesos.erase(itBuddie);
}


// Clase Memoria
// ================================================================================================================
// Funcion ProcessMemory

void processMemory() {
#ifdef _WIN32											//para activar colores y caracteres especiales
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    SetConsoleOutputCP(65001);
#endif

    int ciclo = 1;
    using namespace std::chrono_literals;


    std::cout << YELLOW << "➤" << RESET << BOLD <<" Seleccione el administrador de memoria(1-Buddy 2-Lazy Buddy): " << RESET;
    std::cin >> administrador;
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
        std::cout << "\n";
        memoria.mostrar();
        std::cout << "\n" << "────────────────────────────────────────────────────────────\n";
        ciclo++;
        std::this_thread::sleep_for(5s);
    }
}

