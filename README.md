# Actividad9

Ejercicios de concurrencia implementados en C++ (comentarios en español).

Archivos principales (cada uno ejecutable por separado):

- `BARBERO DORMILON.cpp`  — Simulación del Barbero Dormilón.
- `CENA DE LOS FILOSOFOS.cpp` — Simulación de la Cena de los Filósofos (evita deadlock con un portero).
- `PRODUCTOR - CONSUMIDOR.cpp` (hay también `productor_consumidor.cpp`) — Productores y consumidores con búfer acotado.

Instrucciones rápidas (PowerShell en Windows):

Compilar y ejecutar el Barbero:

```powershell
g++ -std=c++17 "BARBERO DORMILON.cpp" -o barbero.exe -pthread
.\barbero.exe 10 3
```

Filósofos (versión corregida):

```powershell
g++ -std=c++17 "CENA DE LOS FILOSOFOS.cpp" -o filosofos.exe -pthread
.\filosofos.exe 3
```

Productor-Consumidor:

```powershell
g++ -std=c++17 "productor_consumidor.cpp" -o prodcons.exe -pthread
.\prodcons.exe 2 2 5 10
```

Notas:
- Si tu compilador MinGW en Windows no acepta `-pthread`, prueba a quitarla.
- Si prefieres que sobreescriba o renombre archivos, dímelo y lo ajusto.

Para detalles y opciones adicionales consulta `README_INSTRUCCIONES.md`.
# Actividad9