#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
using namespace std;

#define IDEMISOR 7
#define IDRECEPTOR 6

// PAQUETE INICIAL: "[0][ID_EMISOR][ID_RECEPTOR][N][M][CHECKSUM]"
// PAQUETE PIXELES: "[NRO SECUENCIA][ID_EMISOR][ID_RECEPTOR][16 PIXELES][CHECKSUM]"

int calcularChecksum(char m[]){
    char byte[9] = "";
    int suma = 0;
    for(int i = 0; i < 40; i+=8){
        for(int j = i; j < i+8; j++){
            byte[j-i] = m[j];
        }
        int valor_byte = strtol(byte, NULL, 2);
        suma += valor_byte;
    }
    return suma;
}

void intABinario(int valor, char binario[9]) {
  for (int i = 7; i >= 0; i--) {
    binario[7 - i] = (valor & (1 << i)) ? '1' : '0';
  }
  binario[8] = '\0';
}

void paqueteInicial(char m[], char id_emisor[], char id_receptor[], int n_pixeles, int m_pixeles) {
    cout << m << endl;
    strcat(m, "00000000");
    strcat(m, id_emisor);
    strcat(m, id_receptor);

    char nBinario[9];
    intABinario(n_pixeles, nBinario);
    strcat(m, nBinario);

    char mBinario[9];
    intABinario(m_pixeles, mBinario);
    strcat(m, mBinario);

    char checksumBinario[9];
    int checksum = calcularChecksum(m);
    intABinario(checksum, checksumBinario);
    strcat(m, checksumBinario);
}

void paquetePixeles(char m[], int n_secuencia, char id_emisor[], char id_receptor[], char pixeles[]) {
    char nSecuenciaBinario[9];
    intABinario(n_secuencia, nSecuenciaBinario);
    strcat(m, nSecuenciaBinario);
    strcat(m, id_emisor);
    strcat(m, id_receptor);
    strcat(m, pixeles);

    char checksumBinario[9];
    int checksum = calcularChecksum(m);
    intABinario(checksum, checksumBinario);
    strcat(m, checksumBinario);
}

char** packager(char* bits, int n) {
  int numPaquetes = (n / 16) + (n % 16 != 0 ? 1 : 0);
  char** paquetes = new char*[numPaquetes]; 

  for (int i = 0; i < numPaquetes; i++) {
      paquetes[i] = new char[16];
      memset(paquetes[i], '0', 16); 
  }
  
  for (int i = 0; i < n; i++) {
      int paqueteActual = i / 16;
      int posEnPaquete = i % 16;
      paquetes[paqueteActual][posEnPaquete] = bits[i];
  }
  return paquetes;
}

void print(char array[]) {
  int len = strlen(array);
  for (int i = 0; i < len; i++) {
      cout << array[i];
      if ((i + 1) % 8 == 0) { 
          cout << ' ';  
      }
  }
  cout << endl; 
}

void enviarPaquete(char m[]) {
  cout << "Enviando paquete: ";
  print(m);
  // Vaciar el buffer m
  m[0] = '\0';
}

int main(){
// formato de paquetes:[secuencia] [id emisor] [id receptor] [mensaje] [mensaje] [checksum]
    char* imagen = "0000000000000000000000000000000001111111111111111111111111111110011111111111111111111111111111100111111111111111111111111111111001111111111111111111111111111110011111111111111111111111111111100111111111111111111111111111111001111000000011111111000000011110011110000000111111110000000111100111100000001111111100000001111001111000000011111111000000011110011110000000111111110000000111100111100000001111111100000001111001111111111111111111111111111110011111111111000000001111111111100111111111110000000011111111111001111111111100000000111111111110011111111000000000000000111111100111111110000000000000001111111001111111100000000000000011111110011111111000000000000000111111100111111110000000000000001111111001111111100000000000000011111110011111111000111111110000111111100111111110001111111100001111111001111111000011111111000011111110011111111111111111111111111111100111111111111111111111111111111001111111111111111111111111111110011111111111111111111111111111100011111111111111111111111111110000000000000000000000000000000000";
    char m[49] = "";
    char id_emisor[9];
    char id_receptor[9];
    char pixeles[] =  "000000010000000000000000";
    intABinario(IDEMISOR, id_emisor);
    intABinario(IDRECEPTOR, id_receptor);

    
    paqueteInicial(m, id_emisor, id_receptor, 32, 32);
    cout << "Paquete inicial: " << endl;
    enviarPaquete(m);

    char** paquetes = packager(imagen, strlen(imagen));
    int n = strlen(imagen);
    int numPaquetes = (n / 16) + (n % 16 != 0 ? 1 : 0);
    
    for(int i = 0; i < numPaquetes; i++){
      paquetePixeles(m, i+1, id_emisor, id_receptor, paquetes[i]);
      enviarPaquete(m);
    }
    
    cout << "ID emisor: " << id_emisor << endl;
    cout << "ID receptor: " << id_receptor << endl;
    

    return 0;
}

