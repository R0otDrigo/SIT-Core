# Ejercicios de hilos en C++

Cuatro programas independientes para practicar `std::thread`, funciones lambda,
objetos función (`operator()`) y sincronización con `std::mutex`.

## Contenido

| Archivo | Ejercicio | Conceptos |
|---|---|---|
| `ejercicio1_contador.cpp` | Pares e impares del 1 al 20 | `std::thread`, `join()` |
| `ejercicio2_sumador.cpp` | Tres sumadores con estado | Functor, `operator()`, `std::ref` |
| `ejercicio3_lambda.cpp` | Lambda que imprime 5 veces | Lambda, `sleep_for` |
| `ejercicio4_sincronizacion.cpp` | Salida sincronizada | `std::mutex`, `std::lock_guard` |

## Requisitos

- Compilador con C++17 (`g++`, `clang++` o MSVC).
- CMake 3.16 o superior (opcional).
- En Linux/macOS, la biblioteca de hilos del sistema (`-pthread`).

## Compilación con CMake

```bash
cmake -S . -B build
cmake --build build
```

Esto genera los cuatro ejecutables dentro de `build/`. Para ejecutarlos:

```bash
./build/ejercicio1_contador
./build/ejercicio2_sumador
./build/ejercicio3_lambda
./build/ejercicio4_sincronizacion
```

## Compilación individual

```bash
g++ -std=c++17 -pthread ejercicio1_contador.cpp -o ejercicio1_contador
./ejercicio1_contador
```

En macOS se puede omitir `-pthread`; en Windows con MSVC basta con
`cl /std:c++17 ejercicio1_contador.cpp`.

## Explicación de cada ejercicio

### Ejercicio 1: contador con dos hilos

Se crean dos hilos: uno recorre los pares y otro los impares. Ambos empiezan a
ejecutarse al construir el `std::thread`. La pequeña pausa de 10 ms hace visible
la alternancia, y `join()` garantiza que `main` no termine antes que ellos.

Salida esperada (el orden exacto varía en cada ejecución):

```text
Par:   2
Impar: 1
Par:   4
Impar: 3
...
Ambos hilos han terminado.
```

Puntos clave:

- Un `std::thread` debe unirse con `join()` o separarse con `detach()` antes de
  ser destruido; si no, el programa llama a `std::terminate()`.
- El orden de ejecución entre hilos no está garantizado.

### Ejercicio 2: objetos función con estado

`Sumador` guarda su límite `n` y el resultado de la suma. Cada hilo recibe una
instancia distinta, así que cada objeto conserva su propio estado.

```text
Sumador n = 10    ->  suma = 55
Sumador n = 100   ->  suma = 5050
Sumador n = 1000  ->  suma = 500500
```

Punto clave: `std::thread` **copia** sus argumentos. Si se pasa `sumador10`
directamente, el hilo opera sobre una copia y el resultado no se refleja en el
objeto original. Por eso se usa `std::ref(sumador10)`. Alternativamente, se
podría devolver el resultado con `std::future`/`std::async`.

### Ejercicio 3: lambda en un hilo

La tarea del hilo es una lambda sin capturas que imprime cinco mensajes con
`std::this_thread::sleep_for(100ms)` entre ellos. Mientras el hilo secundario
duerme, el principal continúa y muestra su propio mensaje.

Punto clave: cualquier invocable sirve como tarea de un hilo (función libre,
lambda, functor o `std::bind`).

### Ejercicio 4: sincronización de la salida

Es el ejercicio 1 con un `std::mutex` compartido. La escritura en `std::cout`
se realiza dentro de una sección crítica delimitada por `std::lock_guard`, de
modo que cada línea aparece completa y las salidas no se mezclan:

```text
Par    -> 2  (hilo 140258272343744)
Impar  -> 1  (hilo 140258263951040)
Par    -> 4  (hilo 140258272343744)
...
```

Puntos clave:

- `std::lock_guard` usa RAII: bloquea al construirse y libera al salir del
  ámbito, incluso si se produce una excepción.
- La pausa se hace **fuera** del bloqueo para no retener el mutex mientras el
  hilo duerme.
- Sin mutex, las distintas llamadas a `operator<<` de ambos hilos pueden
  intercalarse y producir texto corrupto, como se observa en el ejercicio 1.

## Experimentos sugeridos

1. Ejecutar el ejercicio 1 varias veces y comprobar que el orden cambia.
2. Quitar `std::ref` en el ejercicio 2 y ver que los resultados quedan en 0.
3. Eliminar el `std::mutex` del ejercicio 4 y observar cómo se mezclan las
   líneas (el resultado depende del planificador del sistema).
4. Añadir más hilos o cambiar los rangos y comprobar el efecto.
