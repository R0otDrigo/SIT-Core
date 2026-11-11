// Ejercicio 1: Contador con dos hilos
//
// Dos hilos imprimen simultáneamente los números pares y los impares del 1 al 20.
// El hilo principal espera a que ambos terminen mediante join().
//
// Compilar: g++ -std=c++17 -pthread ejercicio1_contador.cpp -o ejercicio1_contador

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

// Tarea que ejecuta cada hilo: recorre el rango [inicio, fin] con el paso indicado.
void imprimirNumeros(int inicio, int fin, int paso, const std::string& etiqueta) {
    for (int i = inicio; i <= fin; i += paso) {
        std::cout << etiqueta << i << '\n';
        // Pequeña pausa para que la alternancia entre hilos sea visible.
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    // Cada std::thread comienza a ejecutarse en cuanto se construye.
    std::thread hiloPares(imprimirNumeros, 2, 20, 2, "Par:   ");
    std::thread hiloImpares(imprimirNumeros, 1, 20, 2, "Impar: ");

    // join() bloquea al hilo actual hasta que el hilo indicado finalice.
    // Es obligatorio llamarlo (o detach()) antes de destruir el std::thread.
    hiloPares.join();
    hiloImpares.join();

    std::cout << "Ambos hilos han terminado.\n";
    return 0;
}
