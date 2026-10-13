# Laboratorio de algoritmos

Implementación en C11 de algoritmos fundamentales para estudiar la relación
entre diseño, complejidad asintótica, uso de memoria y comportamiento práctico.
El proyecto está pensado como un laboratorio reproducible: cada algoritmo tiene
una API pequeña, una implementación independiente y pruebas ejecutables.

## Objetivos

| Problema | Implementaciones | Complejidad temporal | Memoria adicional |
| --- | --- | ---: | ---: |
| Potencia de un número | Secuencial / binaria | `O(n)` / `O(log n)` | `O(1)` |
| Potencia de una matriz `2×2` | Binaria | `O(log n)` multiplicaciones | `O(1)` |
| Fibonacci | Iterativo / fast doubling | `O(n)` / `O(log n)` | `O(1)` / `O(log n)` |
| Ordenamiento | Merge Sort estable | `O(n log n)` | `O(n)` |
| Multiplicación de matrices | Clásica / Strassen | `O(n³)` / `O(n^log₂7) ≈ O(n²·⁸¹)` | `O(1)` / auxiliar |

Las complejidades describen el crecimiento asintótico. En tamaños pequeños, la
multiplicación clásica puede ser más rápida que Strassen por sus menores costes
constantes de memoria, sumas y llamadas recursivas.

## Estructura del proyecto

```text
.
├── include/
│   ├── algorithms.h       # Potencias, Fibonacci y matrices 2×2
│   ├── benchmark.h        # Utilidades de medición de tiempo
│   ├── matrix.h           # Matriz dinámica y multiplicaciones
│   └── sorting.h          # API de Merge Sort
├── src/
│   ├── benchmark.c
│   ├── fibonacci.c
│   ├── matrix.c
│   ├── matrix_power.c
│   ├── power.c
│   └── sorting.c
├── tests/
│   ├── benchmark_main.c   # Mediciones en formato CSV
│   └── test_main.c        # Pruebas ejecutables de corrección
├── results/
│   └── benchmark_reference.csv
├── Makefile
└── README.md
```

## Requisitos

- Compilador compatible con C11.
- `make`.
- Una implementación de POSIX `clock_gettime` si se utilizan las utilidades
  de medición incluidas.

## Compilación y ejecución

```bash
make
./bin/algorithms_tests
```

Una ejecución exitosa muestra:

```text
Todas las pruebas pasaron.
```

Para limpiar artefactos generados:

```bash
make clean
```

Para compilar con advertencias estrictas:

```bash
make clean
make CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -O2 -Iinclude'
```

Para ejecutar el benchmark y guardar sus resultados:

```bash
mkdir -p results
./bin/algorithms_benchmark > results/benchmark.csv
```

También puede ejecutarse mediante:

```bash
make benchmark
```

El benchmark imprime tres columnas: `algorithm`, `n` y `time_seconds`. Cada
fila corresponde al promedio de tres ejecuciones. Los tamaños se eligen para
que puedan compararse algoritmos con órdenes de crecimiento distintos sin
convertir el experimento en una espera excesiva.

## Qué verifican las pruebas

El ejecutable comprueba:

- Potencias con ambos métodos.
- Fibonacci iterativo, fast doubling y potencia de la matriz base de Fibonacci.
- Merge Sort con valores negativos, repetidos y desordenados.
- Multiplicación clásica y Strassen sobre una matriz rectangular `3×2 · 2×4`.
- Equivalencia entre ambas multiplicaciones para matrices `33×33`.

El caso `33×33` es intencional: Strassen rellena la matriz hasta `64×64` y,
como la implementación usa multiplicación clásica para bloques de tamaño `32`
o menor, ese caso obliga a recorrer la parte recursiva de Strassen.

## Resultados de referencia

La siguiente tabla fue generada con `algorithms_benchmark` y se incluye como
referencia, no como una constante universal. Los tiempos dependen del
procesador, sistema operativo, compilador, optimización y carga del equipo.

### Potencia y Fibonacci

| `n` | Potencia `O(n)` (s) | Potencia `O(log n)` (s) | Fibonacci `O(n)` (s) | Fibonacci `O(log n)` (s) |
|---:|---:|---:|---:|---:|
| 10 | 0.000000115 | 0.000000096 | 0.000000070 | 0.000000068 |
| 1 000 | 0.000002362 | 0.000000085 | 0.000000667 | 0.000000080 |
| 100 000 | 0.000252500 | 0.000000101 | 0.000065369 | 0.000000104 |
| 1 000 000 | 0.001560663 | 0.000000106 | 0.000578962 | 0.000000121 |

La tabla muestra que los algoritmos logarítmicos permanecen prácticamente
constantes en este rango, mientras que los lineales crecen con `n`. Los valores
menores a unas pocas centenas de nanosegundos deben interpretarse con cautela,
porque se aproximan a la resolución práctica del reloj y al coste de la llamada.

### Merge Sort y matrices

| `n` | Merge Sort (s) | Matriz clásica `n×n` (s) | Strassen `n×n` (s) |
|---:|---:|---:|---:|
| 1 000 | 0.000025884 | — | — |
| 10 000 | 0.000294363 | — | — |
| 100 000 | 0.002870122 | — | — |
| 1 000 000 | 0.022914665 | — | — |
| 16 | — | 0.000002485 | 0.000002181 |
| 32 | — | 0.000009020 | 0.000007864 |
| 64 | — | 0.000075335 | 0.000058887 |
| 128 | — | 0.000663760 | 0.000419859 |

En esta ejecución Strassen fue competitivo desde `16×16`, pero esto no debe
generalizarse: la implementación, el compilador y la arquitectura del equipo
pueden cambiar el punto de cruce. Para un estudio más sólido conviene repetir
el experimento con tamaños mayores y varios umbrales híbridos.

Los datos completos están en `results/benchmark_reference.csv`. Para producir
una nueva medición, se reemplaza ese archivo ejecutando el comando de benchmark
indicado anteriormente.

## Decisiones de implementación

### Exponenciación y Fibonacci

La exponenciación binaria reduce el exponente dividiendo entre dos en cada
iteración. Fibonacci usa las identidades de fast doubling para calcular
simultáneamente `F(n)` y `F(n+1)`.

### Matrices

`Matrix` almacena los elementos en un bloque contiguo de memoria en orden por
filas. Las operaciones validan dimensiones y controlan overflow al calcular la
cantidad de elementos. Strassen admite matrices rectangulares compatibles
rellenándolas internamente con ceros hasta una potencia de dos.

Para evitar que el resultado se corrompa cuando comparte almacenamiento con una
entrada, las multiplicaciones calculan primero una matriz temporal y la asignan
al resultado al finalizar.

### Merge Sort

La implementación reserva un único buffer auxiliar, mantiene la estabilidad al
preferir el elemento izquierdo en empates y libera el buffer al terminar.
Devuelve `0` si no puede reservar memoria o recibe una entrada inválida.

## Limitaciones numéricas

Las funciones numéricas usan `uint64_t` solo para exponentes, posiciones
matemáticas y semillas no negativas. Los elementos de las matrices y los
arreglos usan `int64_t`, porque pueden contener valores negativos. Los
resultados que exceden la capacidad del tipo no representan aritmética de
precisión arbitraria; el laboratorio prioriza comparar algoritmos y
complejidades. Para resultados matemáticos exactos con entradas grandes se
necesitaría una biblioteca de enteros de precisión múltiple.
Las pruebas de matrices usan valores que mantienen las operaciones dentro del
rango de `int64_t`; para entradas mayores se debe añadir detección explícita de
overflow.

Los tiempos tampoco son resultados matemáticos exactos. Son observaciones
empíricas y deben acompañarse del entorno de ejecución y de la cantidad de
repeticiones utilizadas.

## API principal

```c
uint64_t power_linear(uint64_t base, uint64_t exponent);
uint64_t power_logarithmic(uint64_t base, uint64_t exponent);

uint64_t fibonacci_linear(uint64_t n);
uint64_t fibonacci_logarithmic(uint64_t n);

int merge_sort(int64_t *values, size_t length);

int matrix_multiply_classic(const Matrix *a, const Matrix *b, Matrix *result);
int matrix_multiply_strassen(const Matrix *a, const Matrix *b, Matrix *result);
```

Las funciones que gestionan matrices devuelven `1` si completan la operación y
`0` si las dimensiones son incompatibles o falla una reserva de memoria.

## Extensiones sugeridas

- Separar las pruebas de corrección de los benchmarks de rendimiento.
- Añadir mediciones para tamaños crecientes y exportarlas como CSV.
- Comparar distintos umbrales híbridos para Strassen.
- Incorporar pruebas automatizadas con AddressSanitizer y UndefinedBehaviorSanitizer.
