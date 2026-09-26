#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    // El programa exige ejecutarse como: ./planificador plan.txt K
    if (argc != 3) {
        cerr << "Error. Uso correcto: " << argv[0] << " <archivo_plan> <K_concurrencia>\n";
        return 1;
    }

    cout << "Planificador Dieciochero iniciado.\n";
    cout << "Archivo a leer: " << argv[1] << "\n";
    cout << "Limite de concurrencia (K): " << argv[2] << "\n";
    
    // Aquí agregaremos la lectura del archivo en el siguiente paso
    
    return 0;
}