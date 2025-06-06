#include <VirtualWire.h>
#include <stdint.h>

#define IDEMISOR 7
#define IDRECEPTOR 6
#define TX_PIN 2  // Arduino pin for VirtualWire transmitter
#define NUMBYTES 2
#define KEY 5

// Function prototypes

void packager(uint8_t* paquete, uint8_t secuencia, uint8_t id_emisor, uint8_t id_receptor, uint8_t* datos);
void enviarPaquete(uint8_t* paquete, size_t length);
void data_extractor(uint8_t* imagen, size_t n, uint8_t* datos, size_t numBytes, int secuencia);
uint8_t calcularCRC8(const uint8_t* datos, size_t longitud, uint8_t polinomio , uint8_t inicial);
void cifrar(uint8_t datos, size_t longitud, int clave);
void printImagen(uint8_t imagen[], int n, int m);





void setup() {
  Serial.begin(9600);
  vw_set_ptt_inverted(true);
  vw_set_tx_pin(TX_PIN);
  vw_setup(2000); // Bits per sec
}

void loop() {
  // Binary representation of a 32x32 image (1024 bits)
  //uint8_t imagen[1024] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  uint8_t imagen[128] = {0, 0, 0, 0, 127, 255, 255, 254, 127, 255, 255, 254, 127, 255, 255, 254, 127, 255, 255, 254, 127, 255, 255, 254, 127, 255, 255, 254, 120, 15, 240, 30, 120, 15, 240, 30, 120, 15, 240, 30, 120, 15, 240, 30, 120, 15, 240, 30, 120, 15, 240, 30, 127, 255, 255, 254, 127, 240, 15, 254, 127, 240, 15, 254, 127, 240, 15, 254, 127, 128, 0, 254, 127, 128, 0, 254, 127, 128, 0, 254, 127, 128, 0, 254, 127, 128, 0, 254, 127, 128, 0, 254, 127, 143, 240, 254, 127, 143, 240, 254, 127, 15, 240, 254, 127, 255, 255, 254, 127, 255, 255, 254, 127, 255, 255, 254, 127, 255, 255, 254, 63, 255, 255, 252, 0, 0, 0, 0};
  printImagen(imagen, 32, 32);

  size_t imagen_length = sizeof(imagen)/sizeof(imagen[0]);
  
  // Convert IDs to binary representation
  uint8_t id_emisor = IDEMISOR;
  uint8_t id_receptor = IDRECEPTOR;
  
  // Create initial packet
  uint8_t paquete[6] = {0};
  uint8_t datos[NUMBYTES] = {32,32};
  packager(paquete, 0, id_emisor, id_receptor, datos);
  enviarPaquete(paquete, sizeof(paquete));
  size_t numPaquetes = (imagen_length / 2) + (imagen_length % 2 != 0 ? 1 : 0);

  // Send pixel packets
  for (size_t i = 1; i <= numPaquetes; i++){
    data_extractor(imagen, imagen_length, datos, NUMBYTES, i);
    cifrar(datos, NUMBYTES, KEY);
    packager(paquete, i, id_emisor, id_receptor, datos);
    enviarPaquete(paquete, sizeof(paquete));
  }

  /*
   // Package the image into 16-bit chunks
  size_t numPaquetes = (imagen_length / 16) + (imagen_length % 16 != 0 ? 1 : 0);
  uint8_t** paquetes = (uint8_t**)malloc(numPaquetes * sizeof(uint8_t*));

  for(size_t i = 0; i < numPaquetes; i++) {
    paquetes[i] = (uint8_t*)malloc(16 * sizeof(uint8_t));
  }
  
  packager(imagen, imagen_length, paquetes, &numPaquetes);
  
  // Send pixel packets
  for(size_t i = 0; i < numPaquetes; i++) {
    uint8_t m_pixel[48] = {0}; // 6 bytes (48 bits) for pixel packet
    paquetePixeles(m_pixel, i+1, id_emisor, id_receptor, paquetes[i], 16);
    enviarPaquete(m_pixel, sizeof(m_pixel));
    free(paquetes[i]); // Free memory after sending
    delay(100);
  }
  free(paquetes);
  */
  
  //delay(5000); // Wait 5 seconds before sending again
}

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

void packager(uint8_t* paquete, uint8_t secuencia, uint8_t id_emisor, uint8_t id_receptor, uint8_t* datos) {
  // Sequence number (0 for initial packet)
  paquete[0] = secuencia;
  // Emitter ID (8 bits)
  paquete[1] = id_emisor;
  
  // Receiver ID (8 bits)
  paquete[2] = id_receptor;
  
  // datos
  paquete[3] = datos[0];
  paquete[4] = datos[1];
  
  // Checksum (8 bits)
  uint8_t CRC8 = calcularCRC8(paquete, 5);
  paquete[5] = CRC8;
}


void enviarPaquete(uint8_t* paquete, size_t length) {

  // Send with VirtualWire
  vw_send(paquete, length);
  vw_wait_tx(); // Wait until the whole message is gone
  
  // Print in binary format
  Serial.print("Sent packet: ");
  for(size_t i = 0; i < length; i++) {
    Serial.print(paquete[i], BIN);
    Serial.print(" ");
  }
  Serial.println();
}

void data_extractor(uint8_t* imagen, size_t n, uint8_t* datos, size_t numBytes, int secuencia) {
  int inicio = (secuencia - 1) *(int)numBytes;
  
  for(size_t i = 0; i < numBytes; i++) {
    if (inicio+i < n){
      datos[i]= imagen[inicio+i];
    }else{
      datos[i]=0; 
    }
    
  }
}

void cifrar(uint8_t* datos, size_t longitud, int clave) {
  for (size_t i = 0; i < longitud; i++) {
    datos[i] = (datos[i] + (clave*2));
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
