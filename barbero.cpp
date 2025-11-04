/*
  BARBERO DORMILON - Implementación en C++ (concurrencia)
  - Comentarios y mensajes en español.
  - Modelo: un barbero y varias sillas de espera (problema del barbero durmiente).
  - Ejecución: el barbero duerme si no hay clientes; los clientes llegan, esperan si hay sillas,
    o se van si no hay lugar.

  Compilar: g++ -std=c++17 "BARBERO DORMILON.cpp" -o barbero -pthread
  Ejecutar: .\barbero.exe  (PowerShell)
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <random>
#include <vector>
#include <clocale>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

mutex mtx;
condition_variable cv_barbero; // barbero espera clientes

int sillas_espera; // capacidad de la sala de espera
queue<int> sala;   // ids de clientes esperando
bool cerrar = false;

// función que simula al barbero: espera clientes, corta cabello
void barbero(){
    unique_lock<mutex> lock(mtx);
    while(!cerrar){
        if(sala.empty()){
            // Si no hay clientes, el barbero duerme hasta que llegue uno
            cout << "[Barbero] No hay clientes. El barbero se duerme.\n";
            cv_barbero.wait(lock, []{ return !sala.empty() || cerrar; });
            if(cerrar) break;
        }

        // Atender al siguiente cliente
        int id = sala.front();
        sala.pop();
        cout << "[Barbero] Llamando al cliente " << id << ". Quedan " << sala.size() << " en espera.\n";

        // Simular corte
        lock.unlock();
        this_thread::sleep_for(chrono::milliseconds(500 + (rand()%500)));
        cout << "[Barbero] Terminado con el cliente " << id << ".\n";
        lock.lock();
    }

    cout << "[Barbero] Cerrando barberia.\n";
}

// función que simula la llegada de clientes
void cliente(int id, int tiempo_llegada_ms){
    this_thread::sleep_for(chrono::milliseconds(tiempo_llegada_ms));
    unique_lock<mutex> lock(mtx);
    cout << "[Cliente "<< id <<"] Llega a la barberia.\n";
    if((int)sala.size() < sillas_espera){
        // Hay silla disponible: se sienta y notifica al barbero
        sala.push(id);
        cout << "[Cliente "<< id <<"] Se sienta en la sala de espera. Posicion: " << sala.size() << ".\n";
        cv_barbero.notify_one();
    } else {
        // No hay sillas: cliente se va
        cout << "[Cliente "<< id <<"] No hay sillas. Se va sin atenderse.\n";
    }
}

int main(int argc, char** argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Configurar locale y consola para mostrar correctamente caracteres UTF-8 (ñ, acentos)
    setlocale(LC_ALL, "");
#ifdef _WIN32
    // En Windows, configurar la consola a UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    // Parámetros por defecto (ajustados para pruebas más pesadas):
    // num_clientes: cantidad de clientes que llegan en total
    // sillas_espera: capacidad de la sala de espera
    // Estos valores son grandes para pruebas de carga; se pueden sobrescribir por argumentos.
    int num_clientes = 100; // antes 10
    sillas_espera = 10;     // antes 3
    if(argc >= 2) num_clientes = stoi(argv[1]);
    if(argc >= 3) sillas_espera = stoi(argv[2]);

    cout << "Barberia iniciada. Clientes: "<< num_clientes << ", Sillas de espera: " << sillas_espera << "\n";

    // lanzar hilo del barbero
    thread t_barbero(barbero);

    // Generar clientes con tiempos aleatorios
    vector<thread> hilos_clientes;
    std::mt19937 rng((unsigned)chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(100, 1000);

    for(int i=1;i<=num_clientes;i++){
        int espera = dist(rng);
        hilos_clientes.emplace_back(cliente, i, espera);
    }

    // Esperar a que terminen los clientes
    for(auto &t : hilos_clientes) t.join();

    // Después de atender o ver que no hay más clientes, cerrar barbería
    {
        unique_lock<mutex> lock(mtx);
        cerrar = true;
        cv_barbero.notify_one();
    }

    t_barbero.join();
    cout << "Fin de la simulacion del Barbero Dormilon.\n";
    return 0;
}
