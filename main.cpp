#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib> 
#include <ctime>   
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>

using namespace std;

struct Actividad {
    int id;
    string nombre;
    int tiempo_ms;
    vector<int> dependencias;
    int pipe_fd[2];
    bool completada = false;
    bool en_proceso = false;
    pid_t pid = 0;
};

vector<pid_t> pids_activos;

void llegada_seremi(int senal) {
    cout << "\n\n[!] Llego la Seremi de Salud (SIGINT)! Cancelando el asado...\n";
    for (size_t i = 0; i < pids_activos.size(); i++) {
        if (pids_activos[i] > 0) {
            kill(pids_activos[i], SIGKILL);
        }
    }
    cout << "[!] Todos los procesos hijos han sido terminados.\n";
    exit(1); 
}

vector<Actividad> leer_plan(const string& nombre_archivo) {
    vector<Actividad> plan;
    ifstream archivo(nombre_archivo);
    string linea;

    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << nombre_archivo << "\n";
        exit(1);
    }
    srand(time(NULL)); 

    while (getline(archivo, linea)) {
        if (linea.empty()) continue; 
        stringstream ss(linea);
        string item;
        Actividad act;

        getline(ss, item, ':');
        act.id = stoi(item);
        getline(ss, act.nombre, ':');
        
        getline(ss, item, ':');
        bool tiene_tiempo = false;
        for (char c : item) {
            if (isdigit(c)) tiene_tiempo = true;
        }
        if (tiene_tiempo) {
            act.tiempo_ms = stoi(item);
        } else {
            act.tiempo_ms = rand() % 4901 + 100;
        }

        if (getline(ss, item, ':')) {
            stringstream ss_deps(item);
            string dep;
            while (getline(ss_deps, dep, ',')) {
                bool es_numero = false;
                for (char c : dep) {
                    if (isdigit(c)) es_numero = true;
                }
                if (es_numero) {
                    act.dependencias.push_back(stoi(dep));
                }
            }
        }
        plan.push_back(act);
    }
    return plan;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " plan.txt K\n";
        return 1;
    }

    signal(SIGINT, llegada_seremi);

    int k_max = stoi(argv[2]);
    vector<Actividad> plan = leer_plan(argv[1]);
    int activas = 0;
    int listas = 0;
    int total = plan.size();

    while (listas < total) {
        for (size_t i = 0; i < plan.size(); i++) {
            if (!plan[i].completada && !plan[i].en_proceso && activas < k_max) {
                bool puede_partir = true;
                for (int d : plan[i].dependencias) {
                    bool dep_ok = false;
                    for (size_t j = 0; j < plan.size(); j++) {
                        if (plan[j].id == d && plan[j].completada) {
                            dep_ok = true;
                            break;
                        }
                    }
                    if (!dep_ok) {
                        puede_partir = false;
                        break;
                    }
                }

                if (puede_partir) {
                    pipe(plan[i].pipe_fd);
                    pid_t p = fork();
                    
                    if (p == 0) { 
                        signal(SIGINT, SIG_DFL);
                        close(plan[i].pipe_fd[0]);
                        usleep(plan[i].tiempo_ms * 1000);
                        
                        string msj = "listo " + plan[i].nombre;
                        write(plan[i].pipe_fd[1], msj.c_str(), msj.length());
                        close(plan[i].pipe_fd[1]);
                        exit(0);
                    } else { 
                        close(plan[i].pipe_fd[1]);
                        plan[i].pid = p;
                        plan[i].en_proceso = true;
                        pids_activos.push_back(p); 
                        activas++;
                    }
                }
            }
        }

        if (activas > 0) {
            int status;
            pid_t termino = wait(&status);
            activas--;
            
            for (size_t k = 0; k < pids_activos.size(); k++) {
                if (pids_activos[k] == termino) {
                    pids_activos[k] = 0;
                    break;
                }
            }
            
            for (size_t i = 0; i < plan.size(); i++) {
                if (plan[i].pid == termino) {
                    plan[i].completada = true;
                    plan[i].en_proceso = false;
                    listas++;
                    
                    char buffer[100];
                    int bytes = read(plan[i].pipe_fd[0], buffer, sizeof(buffer));
                    if (bytes > 0) {
                        buffer[bytes] = '\0';
                        cout << "Actividad completada: " << buffer << "\n";
                    }
                    close(plan[i].pipe_fd[0]);
                    break;
                }
            }
        }
    }
    cout << "\nAsado finalizado con exito!\n";
    return 0;
}