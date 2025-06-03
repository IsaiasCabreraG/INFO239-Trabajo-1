
#include <cstring> 
#include <iostream>
using namespace std;

void print(char** paquetes, int numPaquetes) {
    for (int i = 0; i < numPaquetes; i++) {
        for (int j = 0; j < 16; j++) {
            cout << paquetes[i][j];
        }
        cout << endl;
    }
}


char** getStringArray(char* bits, int n) {
    int numPaquetes = (n / 16) + (n % 16 != 0 ? 1 : 0);
    char** paquetes = new char*[numPaquetes]; 
    
    // Inicializar todos los paquetes con '0'
    for (int i = 0; i < numPaquetes; i++) {
        paquetes[i] = new char[16];
        memset(paquetes[i], '0', 16); 
    }
    
    // Llenar los paquetes con los bits
    for (int i = 0; i < n; i++) {
        int paqueteActual = i / 16;
        int posEnPaquete = i % 16;
        paquetes[paqueteActual][posEnPaquete] = bits[i];
    }
    
    print(paquetes, numPaquetes);
    return paquetes;
}
int main() {
    char bits[] = "0101010010100101001110100101010010";
    int n = strlen(bits); // Longitud del arreglo
    
    // Llamamos a la función
    char** paquetes = getStringArray(bits, n);

    
    
    return 0;
}