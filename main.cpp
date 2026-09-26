#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib> 
#include <ctime>   

using namespace std;


struct Actividad {
    int id;
    string nombre;
    int tiempo_ms;
    vector<int> dependencias;
    int pipe_fd[2]; 
};


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
        cerr << "Error. Uso correcto: " << argv[0] << " <archivo_plan> <K_concurrencia>\n";
        return 1;
    }

    cout << "Plan del WAtON loyola\n";
    cout << "Archivo a leer: " << argv[1] << "\n";
    cout << "Limite de concurrencia (K): " << argv[2] << "\n";

    
    vector<Actividad> actividades = leer_plan(argv[1]);

    
    cout << "\n--- DAG Cargado ---\n";
    for (const auto& act : actividades) {
        cout << "ID: " << act.id << " | " << act.nombre 
             << " | Tiempo: " << act.tiempo_ms << " ms | Dependencias: ";
        if (act.dependencias.empty()) {
            cout << "Ninguna";
        } else {
            for (int d : act.dependencias) cout << d << " ";
        }
        cout << "\n";
    }

    return 0;
}