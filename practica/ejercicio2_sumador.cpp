// Ejercicio 2: Uso de objetos función con estado
//
// La clase Sumador guarda un límite n y, al invocar operator(), calcula
// la suma de 1..n y almacena el resultado en el propio objeto.
//
// Se lanzan tres hilos con n = 10, 100 y 1000. Cada hilo utiliza su propia
// instancia de Sumador, por lo que cada una mantiene un estado independiente.
//
// Compilar: g++ -std=c++17 -pthread ejercicio2_sumador.cpp -o ejercicio2_sumador

#include <functional>
#include <iostream>
#include <thread>

class Sumador {
public:
    explicit Sumador(int limite) : limite_(limite), resultado_(0) {}

    // Objeto función: se ejecuta cuando el hilo invoca al functor.
    // No es const porque modifica el estado interno (resultado_).
    void operator()() {
        long long suma = 0;
        for (int i = 1; i <= limite_; ++i) {
            suma += i;
        }
        resultado_ = suma;
    }

    int limite() const { return limite_; }
    long long resultado() const { return resultado_; }

private:
    int limite_;
    long long resultado_;
};

int main() {
    Sumador sumador10(10);
    Sumador sumador100(100);
    Sumador sumador1000(1000);

    // std::thread copia por valor los argumentos. Si se pasara `sumador10`
    // directamente, el hilo trabajaría sobre una copia y el objeto original
    // conservaría resultado_ = 0. std::ref permite pasar una referencia,
    // de modo que el resultado queda almacenado en el objeto original.
    std::thread hilo10(std::ref(sumador10));
    std::thread hilo100(std::ref(sumador100));
    std::thread hilo1000(std::ref(sumador1000));

    hilo10.join();
    hilo100.join();
    hilo1000.join();

    // Cada objeto conserva su propio límite y su propio resultado.
    std::cout << "Sumador n = " << sumador10.limite()
              << "  ->  suma = " << sumador10.resultado() << '\n';
    std::cout << "Sumador n = " << sumador100.limite()
              << "  ->  suma = " << sumador100.resultado() << '\n';
    std::cout << "Sumador n = " << sumador1000.limite()
              << "  ->  suma = " << sumador1000.resultado() << '\n';

    return 0;
}
