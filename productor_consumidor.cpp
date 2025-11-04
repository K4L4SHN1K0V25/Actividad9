/*
  PRODUCTOR - CONSUMIDOR - Implementación en C++ (concurrencia)
  - Comentarios en español.
  - Búfer acotado, varios productores y consumidores.

  Compilar: g++ -std=c++17 "productor_consumidor.cpp" -o prodcons -pthread
  Ejecutar: .\prodcons.exe  (PowerShell)
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <vector>
#include <chrono>
#include <clocale>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

class BoundedBuffer {
public:
    BoundedBuffer(size_t capacity): cap(capacity) {}

    void produce(int item){
        unique_lock<mutex> lock(mtx);
        cv_full.wait(lock, [&]{ return buffer.size() < cap; });
        buffer.push_back(item);
    cout << "[Productor] Producido: "<< item <<". Tamano: "<<buffer.size()<<"\n";
        cv_empty.notify_one();
    }

    int consume(){
        unique_lock<mutex> lock(mtx);
        cv_empty.wait(lock, [&]{ return !buffer.empty() || finalizar; });
        if(buffer.empty() && finalizar) return -1; // señal de terminación
        int item = buffer.front();
        buffer.pop_front();
    cout << "[Consumidor] Consumido: "<< item <<". Tamano: "<<buffer.size()<<"\n";
        cv_full.notify_one();
        return item;
    }

    void stop(){
        unique_lock<mutex> lock(mtx);
        finalizar = true;
        cv_empty.notify_all();
    }

private:
    deque<int> buffer;
    size_t cap;
    mutex mtx;
    condition_variable cv_full;  // espera productores si buffer lleno
    condition_variable cv_empty; // espera consumidores si buffer vacío
    bool finalizar = false;
};

void productor(BoundedBuffer &bb, int id, int produce_count){
    for(int i=0;i<produce_count;i++){
        this_thread::sleep_for(chrono::milliseconds(100 + (id*20)));
        int item = id*100 + i;
        bb.produce(item);
    }
    cout << "[Productor "<<id<<"] Terminado.\n";
}

void consumidor(BoundedBuffer &bb, int id){
    while(true){
        int item = bb.consume();
        if(item == -1) break; // señal de terminación
        // procesar item
        this_thread::sleep_for(chrono::milliseconds(150));
    }
    cout << "[Consumidor "<<id<<"] Terminando.\n";
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

    // Parámetros por defecto para pruebas más serias (carga mayor)
    int productores = 10;        // antes 2
    int consumidores = 5;        // antes 2
    int capacidad = 50;          // antes 5
    int items_por_productor = 100; // antes 10

    if(argc >= 2) productores = stoi(argv[1]);
    if(argc >= 3) consumidores = stoi(argv[2]);
    if(argc >= 4) capacidad = stoi(argv[3]);
    if(argc >= 5) items_por_productor = stoi(argv[4]);

    cout << "Productor-Consumidor. Productores: "<<productores<<", Consumidores: "<<consumidores
        <<", Capacidad: "<<capacidad<<", Items por productor: "<<items_por_productor<<"\n";

    BoundedBuffer bb(capacidad);

    vector<thread> h_prod, h_cons;
    for(int i=0;i<productores;i++) h_prod.emplace_back(productor, ref(bb), i+1, items_por_productor);
    for(int i=0;i<consumidores;i++) h_cons.emplace_back(consumidor, ref(bb), i+1);

    for(auto &p : h_prod) p.join();

    // Despues de que los productores terminan, senalizamos a los consumidores que terminen
    bb.stop();

    for(auto &c : h_cons) c.join();

        cout << "Fin de la simulacion Productor-Consumidor.\n";
    return 0;
}
