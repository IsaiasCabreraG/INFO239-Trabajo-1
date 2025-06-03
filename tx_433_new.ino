#include <VirtualWire.h>
#include <stdint.h>

#define IDEMISOR 7
#define IDRECEPTOR 6
#define TX_PIN 2  // Arduino pin for VirtualWire transmitter

// Function prototypes
uint8_t calcularChecksum(uint8_t* m, size_t length);
void intABinario(uint8_t valor, uint8_t* binario);
void paqueteInicial(uint8_t* m, uint8_t* id_emisor, uint8_t* id_receptor, uint8_t n_pixeles, uint8_t m_pixeles);
void paquetePixeles(uint8_t* m, uint8_t n_secuencia, uint8_t* id_emisor, uint8_t* id_receptor, uint8_t* pixeles, size_t pixeles_length);
void enviarPaquete(uint8_t* m, size_t length);
void packager(uint8_t* bits, size_t n, uint8_t** paquetes, size_t* numPaquetes);

void setup() {
  Serial.begin(9600);
  vw_set_ptt_inverted(true);
  vw_set_tx_pin(TX_PIN);
  vw_setup(2000); // Bits per sec
}

void loop() {
  Serial.print("hola");
  // Binary representation of a 32x32 image (1024 bits)
  uint8_t imagen[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
   
  };
  
  size_t imagen_length = sizeof(imagen)/sizeof(imagen[0]);
  
  // Convert IDs to binary representation
  uint8_t id_emisor[8];
  uint8_t id_receptor[8];
  intABinario(IDEMISOR, id_emisor);
  intABinario(IDRECEPTOR, id_receptor);
  
  // Create initial packet
  uint8_t m_inicial[40] = {0}; // 5 bytes (40 bits) for initial packet
  paqueteInicial(m_inicial, id_emisor, id_receptor, 32, 32);
  enviarPaquete(m_inicial, sizeof(m_inicial));
  
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
  }
  free(paquetes);
  
  delay(5000); // Wait 5 seconds before sending again

}


uint8_t calcularChecksum(uint8_t* m, size_t length) {
  uint8_t suma = 0;
  for(size_t i = 0; i < length; i += 8) {
    uint8_t byte = 0;
    for(size_t j = 0; j < 8 && (i+j) < length; j++) {
      byte |= (m[i+j] << (7-j));
    }
    suma += byte;
  }
  return suma;
}

void intABinario(uint8_t valor, uint8_t* binario) {
  for(int i = 7; i >= 0; i--) {
    binario[7 - i] = (valor & (1 << i)) ? 1 : 0;
  }
}

void paqueteInicial(uint8_t* m, uint8_t* id_emisor, uint8_t* id_receptor, uint8_t n_pixeles, uint8_t m_pixeles) {
  // Sequence number (0 for initial packet)
  for(int i = 0; i < 8; i++) m[i] = 0;
  
  // Emitter ID (8 bits)
  for(int i = 0; i < 8; i++) m[8+i] = id_emisor[i];
  
  // Receiver ID (8 bits)
  for(int i = 0; i < 8; i++) m[16+i] = id_receptor[i];
  
  // N pixels (8 bits)
  uint8_t nBinario[8];
  intABinario(n_pixeles, nBinario);
  for(int i = 0; i < 8; i++) m[24+i] = nBinario[i];
  
  // M pixels (8 bits)
  uint8_t mBinario[8];
  intABinario(m_pixeles, mBinario);
  for(int i = 0; i < 8; i++) m[32+i] = mBinario[i];
  
  // Checksum (8 bits)
  uint8_t checksum = calcularChecksum(m, 40);
  uint8_t checksumBinario[8];
  intABinario(checksum, checksumBinario);
  for(int i = 0; i < 8; i++) m[40+i] = checksumBinario[i];
}

void paquetePixeles(uint8_t* m, uint8_t n_secuencia, uint8_t* id_emisor, uint8_t* id_receptor, uint8_t* pixeles, size_t pixeles_length) {
  // Sequence number (8 bits)
  uint8_t nSecuenciaBinario[8];
  intABinario(n_secuencia, nSecuenciaBinario);
  for(int i = 0; i < 8; i++) m[i] = nSecuenciaBinario[i];
  
  // Emitter ID (8 bits)
  for(int i = 0; i < 8; i++) m[8+i] = id_emisor[i];
  
  // Receiver ID (8 bits)
  for(int i = 0; i < 8; i++) m[16+i] = id_receptor[i];
  
  // Pixels (16 bits)
  for(size_t i = 0; i < pixeles_length && i < 16; i++) {
    m[24+i] = pixeles[i];
  }
  
  // Fill remaining bits if pixeles_length < 16
  for(size_t i = pixeles_length; i < 16; i++) {
    m[24+i] = 0;
  }
  
  // Checksum (8 bits)
  uint8_t checksum = calcularChecksum(m, 40);
  uint8_t checksumBinario[8];
  intABinario(checksum, checksumBinario);
  for(int i = 0; i < 8; i++) m[40+i] = checksumBinario[i];
}

void enviarPaquete(uint8_t* m, size_t length) {
  // Convert bit array to bytes for transmission
  size_t byte_length = (length + 7) / 8;
  uint8_t tx_bytes[byte_length];
  memset(tx_bytes, 0, byte_length);
  
  for(size_t i = 0; i < length; i++) {
    if(m[i]) {
      tx_bytes[i/8] |= (1 << (7 - (i % 8)));
    }
  }
  
  // Send with VirtualWire
  vw_send(tx_bytes, byte_length);
  vw_wait_tx(); // Wait until the whole message is gone
  
  // Print in binary format
  Serial.print("Sent packet: ");
  for(size_t i = 0; i < length; i++) {
    Serial.print(m[i] ? "1" : "0");
    if((i + 1) % 8 == 0 && (i + 1) < length) {
      Serial.print(" "); // Add space between bytes
    }
  }
  Serial.println();
}

void packager(uint8_t* bits, size_t n, uint8_t** paquetes, size_t* numPaquetes) {
  *numPaquetes = (n / 16) + (n % 16 != 0 ? 1 : 0);
  
  for(size_t i = 0; i < n; i++) {
    size_t paqueteActual = i / 16;
    size_t posEnPaquete = i % 16;
    paquetes[paqueteActual][posEnPaquete] = bits[i];
  }
  
  // Fill remaining bits in last packet with zeros if needed
  if(n % 16 != 0) {
    for(size_t i = n % 16; i < 16; i++) {
      paquetes[*numPaquetes - 1][i] = 0;
    }
  }
}
