#include <VirtualWire.h>
#include <stdint.h>

#define RX_PIN 2  // Pin del receptor

void setup() {
  Serial.begin(9600);
  Serial.println("Receptor listo");
  
  vw_set_rx_pin(RX_PIN);
  vw_setup(2000); // Misma velocidad que el transmisor (2000 bps)
  vw_rx_start();  // Iniciar el receptor
}

void loop() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen)) { // Si hay un mensaje disponible
    Serial.print("Paquete recibido ("); 
    Serial.print(buflen); 
    Serial.println(" bytes):");
    
    // Imprimir cada byte en binario
    for (unsigned int i = 0; i < buflen; i++) {
      printByteBinary(buf[i]);
      Serial.print(" "); // Separador entre bytes
      
      // Nueva línea cada 6 bytes para mejor legibilidad
      if ((i + 1) % 6 == 0) Serial.println();
    }
    Serial.println("\n-----------------------");
  }
}

// Función para imprimir un byte en formato binario (8 bits)
void printByteBinary(uint8_t byte) {
  for (int i = 7; i >= 0; i--) {
    Serial.print(bitRead(byte, i));
  }
}
