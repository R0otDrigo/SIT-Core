// Ejercicio 3: Función lambda en un hilo
//
// Una función lambda se usa como tarea de un hilo. La lambda imprime un
// mensaje 5 veces, con una pausa de 100 ms entre impresiones.
//
// Compilar: g++ -std=c++17 -pthread ejercicio3_lambda.cpp -o ejercicio3_lambda

#include <chrono>
#include <iostream>
#include <thread>

int main() {
    // La lambda no captura nada; std::thread la copia y la ejecuta.
    auto tarea = [] {
        constexpr int repeticiones = 5;
        for (int i = 1; i <= repeticiones; ++i) {
            std::cout << "Mensaje " << i << " desde el hilo secundario\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    };

    std::thread hilo(tarea);

    // El hilo principal sigue su curso mientras el secundario duerme entre
    // impresiones; esto hace visible la ejecución concurrente.
    std::cout << "El hilo principal continúa trabajando...\n";

    hilo.join();  // Esperar a que la lambda termine.

    std::cout << "El hilo secundario ha finalizado.\n";
    return 0;
}
