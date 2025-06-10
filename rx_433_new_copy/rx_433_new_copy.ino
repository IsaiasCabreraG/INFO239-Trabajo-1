#include <VirtualWire.h>
#include <stdint.h>

#define RX_PIN 2 // Pin del receptor
#define LED_PIN 7 // Pin del led Verde
#define ID 6
#define PXLS_POR_PAQUETE 16
#define KEY 10

void intABinario(int valor, char binario[9]) {
  for (int i = 7; i >= 0; i--) {
    binario[7 - i] = (valor & (1 << i)) ? '1' : '0';
  }
  binario[8] = '\0';
}

bool verificarID(uint8_t paquete[]){
  if(paquete[2] == ID){
    Serial.print("ID correcto - ");
    return true;
  }
  Serial.println("ID incorrecto");
  return false;
}

// Función para calcular CRC8
uint8_t calcularCRC8(const uint8_t* datos, size_t longitud, uint8_t polinomio = 0x07, uint8_t inicial = 0x00) {
    uint8_t crc = inicial;
    for (size_t i = 0; i < longitud; ++i) {
        crc ^= datos[i];
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x80)
                crc = (crc << 1) ^ polinomio;
            else
                crc <<= 1;
        }
    }
    return crc;
}

// Verifica que el último byte sea el CRC8 correcto
bool verificarCRC8(const uint8_t* datos, size_t longitudConCRC, uint8_t polinomio = 0x07, uint8_t inicial = 0x00) {
    uint8_t crcCalculado = calcularCRC8(datos, longitudConCRC - 1, polinomio, inicial);
    if(crcCalculado == datos[longitudConCRC - 1]){
      Serial.println("CRC correcto");
      return true; 
     }
    Serial.println("CRC incorrecto");
    return false;
}

bool imagenCompleta(bool paquetesRecibidos[], int numPaquetes){
  for(int i = 0; i < numPaquetes; i++){
    if(! paquetesRecibidos[i]){
      return false;
    }
  }
  return true;
}

int cantidadPaquetes(int n, int m, int pxlPorPaquete){
  return (n * m) / pxlPorPaquete;
}

void assembler(uint8_t imagen[], int sizeImagen, uint8_t datos[], int sizeDatos, int nSecuencia){
  int inicio = (nSecuencia-1) * sizeDatos;
  for(int i = 0; i < sizeDatos; i++){
    if(inicio + i < sizeImagen){
      imagen[inicio + i] = datos[i];
    }
  }    
}

void printImagen(uint8_t imagen[], int n, int m){
  for(int i = 0; i < n; i++){
    for(int j = 0; j < m / 8; j++){
      for(int k = 7; k >= 0 ; k--){
        if(bitRead(imagen[i*(m/8) + j], k) == 0){
          Serial.print("  ");
        }
        else{
          Serial.print("##");
        }
      }
    }
    Serial.println();
  }   
}

uint8_t descifrar(uint8_t dato, int clave){
  return (dato - clave) % 256;
}

bool inicialRecibido = false;
bool imagenCompletaAux = false;
bool* paquetesRecibidos;
int n;
int m;
uint8_t imagen[128];
int contador = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Receptor listo");
  
  vw_set_rx_pin(RX_PIN);
  vw_setup(2000);
  vw_rx_start();
}

void loop() {
  if(!imagenCompletaAux){
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

    if (vw_get_message(buf, &buflen)) { // Si hay un mensaje disponible
      Serial.print("Paquete recibido - "); 
      //Serial.print(buflen); 
      //Serial.println(" bytes):");

      if(verificarID(buf) && verificarCRC8(buf, buflen)){ // se verifica que el id_receptor y el checksum es correcto
        if(!inicialRecibido){
          if(buf[0] == 0){
            inicialRecibido = true;
            n = buf[3];
            m = buf[4];
            paquetesRecibidos = new bool[cantidadPaquetes(n, m, PXLS_POR_PAQUETE)];
            for(int i = 0; i < cantidadPaquetes(n, m, PXLS_POR_PAQUETE); i++){
              paquetesRecibidos[i] = false;
            }
            paquetesRecibidos[buf[0]] = true;
            Serial.println("Paquete inicial recibido");
          }
        }
        else{
          if(!paquetesRecibidos[buf[0]] && buf[0] != 0){
              contador ++;
              digitalWrite(LED_PIN, HIGH);
              paquetesRecibidos[buf[0]] = true;
              uint8_t datos[2];
              datos[0] = descifrar(buf[3], KEY);
              datos[1] = descifrar(buf[4], KEY);


              assembler(imagen, sizeof(imagen)/sizeof(imagen[0]), datos, sizeof(datos)/sizeof(datos[0]), buf[0]);
              // Imprimir cada byte en binario
              for (unsigned int i = 0; i < buflen; i++) {
                printByteBinary(buf[i]);
                Serial.print(" "); // Separador entre bytes
              }
              Serial.println("\n-----------------------");
              Serial.print("Contador: ");
              Serial.println(contador);
              if(contador > 60){
                for(int i = 0; i < (n*m/PXLS_POR_PAQUETE); i++){
                  if(paquetesRecibidos[i] == false) Serial.println(i);
                }
              }
              digitalWrite(LED_PIN, LOW);
              imagenCompletaAux = imagenCompleta(paquetesRecibidos, cantidadPaquetes(n, m, PXLS_POR_PAQUETE));
            }
        }
      }
    }
  }
  else{
    printImagen(imagen, n, m);
    delay(5000);
  }
  
}

// Función para imprimir un byte en formato binario (8 bits)
void printByteBinary(uint8_t byte) {
  for (int i = 7; i >= 0; i--) {
    Serial.print(bitRead(byte, i));
  }
}
