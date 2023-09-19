

#include "Arduino.h"
#include "LoRaWan_APP.h"
//agregadas nuevas para la prueba
#include <Wire.h>  
#include "HT_SSD1306Wire.h"
//#include "images.h"


//Configuración de Parametros
#define RF_FREQUENCY                                915000000 // Hz

#define TX_OUTPUT_POWER                             14        // dBm

#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false


#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 30 // Define the payload size here

char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];

static RadioEvents_t RadioEvents;

int16_t txNumber;

int16_t rssi,rxSize;

bool lora_idle = true;

//agrego constructor
SSD1306Wire  factory_display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED); // addr , freq , i2c group , resolution , rst

//logo 




void setup() {
  Serial.begin(115200);
  Mcu.begin();
  factory_display.init(); // Inicializa la pantalla OLED
  factory_display.flipScreenVertically(); // Voltea la pantalla OLED (opcional)
  factory_display.setFont(ArialMT_Plain_16); // Establece la fuente de texto
  factory_display.drawString(0, 0, "Esperando dato"); // Muestra el mensaje "Hola mundo!" en la pantalla OLED
  factory_display.display(); // Muestra el contenido de la pantalla OLED
    
    txNumber=0;
    rssi=0;
  
    RadioEvents.RxDone = OnRxDone;
    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                               LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                               LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                               0, true, 0, 0, LORA_IQ_INVERSION_ON, true );

}

void loop() {
  if(lora_idle)
  {
    lora_idle = false;
    Serial.println("into RX mode");
    Radio.Rx(0);
  }
  Radio.IrqProcess( );

}



void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
  
rssi=rssi;
//rssi = "rssi: ";
//rssi += String(rssi,DEC);
rxSize=size;
memcpy(rxpacket, payload, size );
rxpacket[size]='\0';
Radio.Sleep( );
Serial.printf("\r\nPaquete Recibido \"%s\" con rssi %d , longitud %d\r\n",rxpacket,rssi,rxSize);
//lora_idle = true;

factory_display.clear(); // Limpia la pantalla OLED
//factory_display.drawString(0, 0, "Paquete Recibido:");
//factory_display.drawString(0, 12, rxpacket);
factory_display.drawString(0, 0, "RSSI:");
factory_display.drawString(0, 12, String(rssi));
factory_display.display(); // Muestra el contenido de la pantalla OLED
delay(2000);
factory_display.clear();


    

//char rxpacket[] = " ACX123@ACY456@ACZ678"; 

int acxValue = 0;
int acyValue = 0;
int aczValue = 0;

// Busca "ACX" y extrae su valor
char* acxStart = strstr(rxpacket, "ACX");
if (acxStart != NULL) {
  sscanf(acxStart, "ACX%d", &acxValue);
}

// Busca "ACY" y extrae su valor
char* acyStart = strstr(rxpacket, "ACY");
if (acyStart != NULL) {
  sscanf(acyStart, "ACY%d", &acyValue);
}
//}

// Busca "ACZ" y extrae su valor
char* aczStart = strstr(rxpacket, "ACZ");
if (aczStart != NULL){
  sscanf(aczStart, "ACZ%d", &aczValue);
}

Serial.print("ACX: ");
Serial.println(acxValue);
Serial.print("ACY: ");
Serial.println(acyValue);
Serial.print("ACZ: ");
Serial.println(aczValue);








factory_display.clear();
factory_display.drawString(0, 0, "ACX:");
factory_display.drawString(40, 0, String(acxValue)); // 0,12
factory_display.drawString(0, 24, "ACY:");
factory_display.drawString(46, 24, String(acyValue)); // 0.36
factory_display.drawString(0, 48, "ACZ:");
factory_display.drawString(48,48, String(aczValue));
factory_display.display();
delay(3000);
factory_display.clear();


lora_idle = true;

}
//}
