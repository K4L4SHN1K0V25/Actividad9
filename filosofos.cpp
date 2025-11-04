/*
  CENA DE LOS FILOSOFOS - Implementación en C++ (concurrencia)
  - Comentarios y mensajes en español.
  - Solución usando un portero (semaphore) que permite como máximo N-1 filósofos sentarse
    para evitar interbloqueo.

  Compilar: g++ -std=c++17 "CENA DE LOS FILOSOFOS.cpp" -o filosofos -pthread
  Ejecutar: .\filosofos.exe  (PowerShell)
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <memory>
#include <clocale>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

class Semaphore {
public:
    Semaphore(int count=0): count(count) {}
    void notify(){
        unique_lock<mutex> lock(mtx);
        ++count;
        cv.notify_one();
    }
    void wait(){
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [&]{ return count>0; });
        --count;
    }
private:
    mutex mtx;
    condition_variable cv;
    int count;
};

int N = 5; // número de filósofos
vector<unique_ptr<mutex>> tenedores;
Semaphore portero( N-1 ); // permite que como máximo N-1 filósofos intenten tomar tenedores

void filosofar(int id, int iteraciones){
    for(int i=0;i<iteraciones;i++){
        // Pensar
        cout << "[Filosofo "<<id<<"] Pensando...\n";
        this_thread::sleep_for(chrono::milliseconds(200 + (id*50)));

        // Pedir permiso al portero (evita deadlock)
        portero.wait();

        // Tomar tenedores: primero el de la izquierda, luego el de la derecha
        int izq = id;
        int der = (id+1) % N;

        // Para evitar bloqueos adicionales, tomamos siempre el mutex con menor índice primero
        if(izq < der){
            tenedores[izq]->lock();
            tenedores[der]->lock();
        } else {
            tenedores[der]->lock();
            tenedores[izq]->lock();
        }

        // Comer
        cout << "[Filosofo "<<id<<"] Comiendo (iteracion "<< i+1 <<").\n";
        this_thread::sleep_for(chrono::milliseconds(300));

        // Soltar tenedores
        tenedores[izq]->unlock();
        tenedores[der]->unlock();

        // Avisar al portero que terminó
        portero.notify();
    }
    cout << "[Filosofo "<<id<<"] Termino todas las iteraciones.\n";
}

int main(int argc, char** argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Configurar locale y consola para mostrar correctamente caracteres UTF-8 (ñ, acentos)
    setlocale(LC_ALL, "");
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    // Parámetros por defecto para pruebas más serias
    // iter: número de veces que cada filósofo piensa/come
    // N: número de filósofos
    int iter = 50; // antes 3
    if(argc >= 2) iter = stoi(argv[1]);
    if(argc >= 3) N = stoi(argv[2]);

    cout << "Cena de los Filosofos. Filosofos: "<<N<<", iteraciones por filosofo: "<<iter<<"\n";

    tenedores.clear();
    tenedores.reserve(N);
    for(int i=0;i<N;i++) tenedores.emplace_back(make_unique<mutex>());

    // Crear hilos
    vector<thread> hilos;
    for(int i=0;i<N;i++){
        hilos.emplace_back(filosofar, i, iter);
    }

    for(auto &t : hilos) t.join();

    cout << "Fin de la simulacion de la Cena de los Filosofos.\n";
    return 0;
}
