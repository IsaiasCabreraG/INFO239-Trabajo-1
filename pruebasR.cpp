#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
using namespace std;

#define ID 6 

int obtenerNSecuencia(char m[]){
  char nSecuencia[8] = "";
  for (int i = 0; i < 8; i++){
      nSecuencia[i] = m[i];
  }
  int valor_nSecuencia = strtol(nSecuencia, NULL, 2);
  return valor_nSecuencia;
}


int obtenerIdEmisor(char m[]){
  char id_emisor[8] = "";
  for (int i = 0; i < 8; i++){
      id_emisor[i] = m[i+8];
  }
  int valor_id = strtol(id_emisor, NULL, 2);
  return valor_id;
}

bool verificaIdReceptor(char m[]){
  char id_receptor[8] = "";
  for (int i = 0; i < 8; i++){
      id_receptor[i] = m[i+16];
  }
  int valor_id = strtol(id_receptor, NULL, 2);
  /*for(int i = 0; i < sizeof(id_receptor); i++){
      cout << id_receptor[i];   
  }
  cout << endl;
  cout << valor_id << endl;*/
  if(valor_id == ID){
    cout << "ID correcto, paquete aceptado" << endl;
    return true;
  }
  cout << "ID incorrecto, se rechaza el paquete" << endl;
  return false;
}

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

bool verificarChecksum(int checksum, char m[]){
    char checksumBinario[9];
    intABinario(checksum, checksumBinario);
    int i = 48;
    int j = sizeof(checksumBinario) - 1;
    while(j >= 0 && checksumBinario[j] == m[i]){
        i--;
        j--;
    }
    if(j < 0){
        cout << "Checksum Correcto" << endl;
        return true;
    }
    cout << "Checksum Incorrecto" << endl;
    return false;

}

void desempaquetar(const char* paquete, char mensaje[17]) {

  int inicio_msg = 24; 
  for (int i = 0; i < 16; i++) {
      mensaje[i] = paquete[inicio_msg + i];
  }
  mensaje[16] = '\0'; // Terminar el string
}

void rellenarImagen(char** imagen, const char mensaje[], int& fila_actual,int& columna_actual) {

  
    for(int i = 0; i < 16 ; i++) {
      imagen[fila_actual][columna_actual] = (mensaje[i] != '0' ? '#' : '_');
      if (columna_actual ==31){
        fila_actual++;
        columna_actual =0;
      }else{columna_actual++;}
    }
    
}

void recibirPaquete(char m[],char** imagen, int& fila_actual,int& columna_actual ) {
  char mensaje[17];
  int nSecuencia = obtenerNSecuencia(m);
  cout << "Numero de secuencia: " << nSecuencia << endl;
  verificaIdReceptor(m);
  int id_emisor = obtenerIdEmisor(m);
  cout << "ID emisor: " << id_emisor << endl;
  if (verificarChecksum(calcularChecksum(m), m) && (verificaIdReceptor(m))){
    desempaquetar(m, mensaje);
    cout<<mensaje<<endl;
    rellenarImagen(imagen , mensaje, fila_actual, columna_actual);
  }

    
}



int main(){
  // Estructura del paquete: [8 seq][8 emisor][8 receptor][16 mensaje][8 checksum]
  const int filas = 32;
  const int columnas = 32;
    
    char** imagen = new char*[filas];    
    for(int i = 0; i < filas; i++) {
      imagen[i] = new char[columnas];
      // Inicializar con '_'
      memset(imagen[i], '_', columnas);
  }
  int fila_actual = 0, columna_actual = 0;

  char mInicial[] = "000000000000011100000110001000000010000001001101";
  char* paquetes[] = {
    "000000010000011100000110000000000000000000001110",
    "000000100000011100000110000000000000000000001111",
    "000000110000011100000110011111111111111110001110",
    "000001000000011100000110111111111111111000001110",
    "000001010000011100000110011111111111111110010000",
    "000001100000011100000110111111111111111000010000",
    "000001110000011100000110011111111111111110010010",
    "000010000000011100000110111111111111111000010010",
    "000010010000011100000110011111111111111110010100",
    "000010100000011100000110111111111111111000010100",
    "000010110000011100000110011111111111111110010110",
    "000011000000011100000110111111111111111000010110",
    "000011010000011100000110011111111111111110011000",
    "000011100000011100000110111111111111111000011000",
    "000011110000011100000110011110000000111110100011",
    "000100000000011100000110111100000001111000101011",
    "000100010000011100000110011110000000111110100101",
    "000100100000011100000110111100000001111000101101",
    "000100110000011100000110011110000000111110100111",
    "000101000000011100000110111100000001111000101111",
    "000101010000011100000110011110000000111110101001",
    "000101100000011100000110111100000001111000110001",
    "000101110000011100000110011110000000111110101011",
    "000110000000011100000110111100000001111000110011",
    "000110010000011100000110011110000000111110101101",
    "000110100000011100000110111100000001111000110101",
    "000110110000011100000110011111111111111110100110",
    "000111000000011100000110111111111111111000100110",
    "000111010000011100000110011111111111000010011001",
    "000111100000011100000110000011111111111000111000",
    "000111110000011100000110011111111111000010011011",
    "001000000000011100000110000011111111111000111010",
    "001000010000011100000110011111111111000010011101",
    "001000100000011100000110000011111111111000111100",
    "001000110000011100000110011111111000000000101111",
    "001001000000011100000110000000001111111000101111",
    "001001010000011100000110011111111000000000110001",
    "001001100000011100000110000000001111111000110001",
    "001001110000011100000110011111111000000000110011",
    "001010000000011100000110000000001111111000110011",
    "001010010000011100000110011111111000000000110101",
    "001010100000011100000110000000001111111000110101",
    "001010110000011100000110011111111000000000110111",
    "001011000000011100000110000000001111111000110111",
    "001011010000011100000110011111111000000000111001",
    "001011100000011100000110000000001111111000111001",
    "001011110000011100000110011111111000111101001010",
    "001100000000011100000110111100001111111000101011",
    "001100010000011100000110011111111000111101001100",
    "001100100000011100000110111100001111111000101101",
    "001100110000011100000110011111110000111111001110",
    "001101000000011100000110111100001111111000101111",
    "001101010000011100000110011111111111111111000000",
    "001101100000011100000110111111111111111001000000",
    "001101110000011100000110011111111111111111000010",
    "001110000000011100000110111111111111111001000010",
    "001110010000011100000110011111111111111111000100",
    "001110100000011100000110111111111111111001000100",
    "001110110000011100000110011111111111111111000110",
    "001111000000011100000110111111111111111001000110",
    "001111010000011100000110001111111111111110001000",
    "001111100000011100000110111111111111110001000110",
    "001111110000011100000110000000000000000001001100",
    "010000000000011100000110000000000000000001001101"
  };
  int num_paquetes = sizeof(paquetes)/sizeof(paquetes[0]);
  for(int i = 0; i < num_paquetes; i++) {
    recibirPaquete(paquetes[i], imagen, fila_actual, columna_actual);
  }
  for(int i = 0; i < 32 ; i++){
    for(int j = 0; j < 32 ; j++){
      cout<<imagen[i][j];
    }
    cout<<endl;
  }

    // Liberar memoria
  for(int i = 0; i < filas; i++) {
    delete[] imagen[i];
  }
  delete[] imagen;
    return 0;
}
