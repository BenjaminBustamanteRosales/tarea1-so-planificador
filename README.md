# tarea1-so-planificador


 (Simulador de Asado)

Integrantes: Catalina Molina y Benjamin Bustamante

## 1. Cómo compilar
Para compilar el código fuente utilizando los estándares exigidos, ejecuta el siguiente comando en la terminal de Linux (WSL):
```bash
g++ -Wall -Wextra -std=c++17 main.cpp -o planificador


## 2. Cómo ejecutar

El programa recibe dos parámetros obligatorios: el archivo de texto con el DAG y el límite máximo de procesos activos al mismo tiempo (K).

Ejemplo de ejecución con K = 3:

./planificador plan.txt 3






## 3. Decisiones de Diseño y Lógica Implementada
Creación de procesos y comunicación: Tal como pedía el enunciado, no se usaron hilos. Se utilizó fork() para levantar procesos pesados. Para que los hijos notifiquen al padre, usé pipe(). El hijo escribe un mensaje de texto indicando los milisegundos que tomó la ejecución y luego muere con exit(0).

Control de concurrencia (Límite K): Para evitar que el sistema colapse y no hacer busy-waiting (espera activa que gasta CPU), utilizo una variable activas. Si esta variable llega al límite de K, el ciclo de creación de procesos se detiene y llama a wait(&status). Esto deja al padre bloqueado durmiendo hasta que algún hijo termine y libere un cupo.

Manejo de la Seremi (SIGINT): Utilicé un vector global llamado pids_activos para almacenar los PIDs de los hijos que están en ejecución. La justificación de hacerlo global es que la función que captura la señal Ctrl+C es gatillada de forma asíncrona por el sistema operativo, por lo que necesita tener los PIDs a la mano para mandarles la señal SIGKILL. A los hijos les asigné signal(SIGINT, SIG_DFL) apenas nacen para que vuelvan al comportamiento por defecto y dejen que solo el proceso padre controle el cierre del programa.

Simulación de Errores y Efecto Dominó: Agregué un 15% de probabilidad de que una actividad falle, terminando con exit(1). El padre detecta si hubo un error gracias a la macro WEXITSTATUS(status) != 0 al momento de hacer el wait. Si hay fallo, marco la actividad en el struct con un flag fallida = true. Antes de lanzar cualquier actividad nueva en el bucle, el programa revisa el vector de dependencias previas; si alguna dependencia tiene el flag de falla, la actividad actual se aborta automáticamente sin necesidad de botar las demás ramas del simulador.