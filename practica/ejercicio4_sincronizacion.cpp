// Ejercicio 4: Sincronización de salida
//
// Versión del ejercicio 1 en la que un std::mutex protege el acceso a
// std::cout. Así cada línea se imprime completa: las salidas de los dos
// hilos no se mezclan entre sí.
//
// Compilar: g++ -std=c++17 -pthread ejercicio4_sincronizacion.cpp -o ejercicio4_sincronizacion

#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

// Mutex compartido por todos los hilos que escriben en la consola.
std::mutex mutexCout;

void imprimirNumeros(int inicio, int fin, int paso, const std::string& etiqueta) {
    for (int i = inicio; i <= fin; i += paso) {
        {
            // lock_guard adquiere el mutex al construirse y lo libera
            // automáticamente al salir del bloque (RAII), incluso si se
            // lanzara una excepción.
            std::lock_guard<std::mutex> bloqueo(mutexCout);

            // Toda la línea se escribe dentro de la sección crítica.
            // Sin el mutex, las llamadas a operator<< de ambos hilos
            // podrían intercalarse y producir texto mezclado.
            std::cout << etiqueta << " -> " << i
                      << "  (hilo " << std::this_thread::get_id() << ")\n";
        }  // Aquí se libera el mutex.

        // La pausa se hace fuera de la sección crítica: no es necesario
        // retener el mutex mientras el hilo duerme.
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    std::thread hiloPares(imprimirNumeros, 2, 20, 2, "Par   ");
    std::thread hiloImpares(imprimirNumeros, 1, 20, 2, "Impar ");

    hiloPares.join();
    hiloImpares.join();

    std::cout << "Ambos hilos han terminado.\n";
    return 0;
}
