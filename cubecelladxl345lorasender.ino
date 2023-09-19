#include <Wire.h>
#include "HT_SH1107Wire.h"
#include "LoRaWan_APP.h"
#include "Arduino.h"


//int valor = 5;



//Se deben definir los pines segun pinout
int SDApin = GPIO9;
int SCLpin = GPIO8;
//Direccion del dispositivo
const int DEVICE_ADDRESS = (0x53);

//Contructor de la pantalla Oled
extern SH1107Wire display;

byte _buff[6];

//Direcciones de los registros del ADXL345
char POWER_CTL = 0x2D;
char DATA_FORMAT = 0x31;
char DATAX0 = 0x32;  //X-Axis Data 0
char DATAX1 = 0x33;  //X-Axis Data 1
char DATAY0 = 0x34;  //Y-Axis Data 0
char DATAY1 = 0x35;  //Y-Axis Data 1
char DATAZ0 = 0x36;  //Z-Axis Data 0
char DATAZ1 = 0x37;  //Z-Axis Data 1


//lora configuacion

#ifndef LoraWan_RGB
#define LoraWan_RGB 0
#endif

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

float txNumber;
bool lora_idle=true;


void setup()
{
  Serial.begin(57000);
  Serial.print("Iniciado");
  //display.init();
   txNumber=0;


    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
   
  //display.setFont(ArialMT_Plain_16);
  
  //display.drawString(0, 0, "Iniciando...");
  //display.drawString(46, 24, String(status));
  //display.display();
  
  //Wire.begin();
  Wire.begin(SDApin, SCLpin);
  writeTo(DEVICE_ADDRESS, DATA_FORMAT, 0x01); //Poner ADXL345 en +- 4G
  writeTo(DEVICE_ADDRESS, POWER_CTL, 0x08);  //Poner el ADXL345 
}

void loop()
{
  readAccel(); //Leer aceleracion x, y, z
  delay(3000);
  //display.init();
  //display.init();
//display.clear(); 
//display.setFont(ArialMT_Plain_16);
//display.setCursor(0, 0);
//display.drawString(0, 0, "Enviando.. ");
//display.display();
}

void readAccel() {
  //Leer los datos
  if(lora_idle==true){
  uint8_t numBytesToRead = 6;
  readFrom(DEVICE_ADDRESS, DATAX0, numBytesToRead, _buff);
  //if(lora_idle == true){
  //Leer los valores del registro y convertir a int (Cada eje tiene 10 bits, en 2 Bytes LSB)
  int x = (((int)_buff[1]) << 8) | _buff[0];   
  int y = (((int)_buff[3]) << 8) | _buff[2];
  int z = (((int)_buff[5]) << 8) | _buff[4];
  //if(lora_idle == true){

  
  Serial.print("x: ");
  Serial.print( x );
  Serial.print(" y: ");
  Serial.print( y );
  Serial.print(" z: ");
  Serial.println( z );
  //envio paquete lora
//Hmd@%0.2f@Tmp@%0.2f
//"Hello world number %0.2f",txNumber

sprintf(txpacket," ACX%0i@ACY%0i@ACZ%0i",x,y,z); //comienza envio de paquete en prueba 3 datos

//sprintf(txpacket," ACX%0.2i@ACY@%0.2i",x,y); //comienza envio de paquete 2 datos probado funciona
Serial.printf("\r\nEnviando paquete \"%s\" , longitud %d\r\n",txpacket, strlen(txpacket));

turnOnRGB(COLOR_SEND,0); //cambio color rgb
Radio.Send( (uint8_t *)txpacket, strlen(txpacket) ); //send the package out 
lora_idle = false;
  }

Radio.IrqProcess();




  //display.init();
  //display.clear(); 
  //display.setFont(ArialMT_Plain_16);
  //display.drawString(0, 0, "x: ");
  //display.drawString(40, 0, String(x));
  //display.drawString(0, 24, "y: ");
  //display.drawString(46, 24, String(y));
  //display.drawString(0, 48, "z: ");
  //display.drawString(48, 48, String(z));
  //display.display();
  //delay(3000);
  //display.clear();
}

//Funcion auxiliar de escritura
void writeTo(int device, byte address, byte val) {
  Wire.beginTransmission(device);
  Wire.write(address);
  Wire.write(val);
  Wire.endTransmission(); 
}

//Funcion auxiliar de lectura
void readFrom(int device, byte address, int num, byte _buff[]) {
  Wire.beginTransmission(device);
  Wire.write(address);
  Wire.endTransmission();

  Wire.beginTransmission(device);
  Wire.requestFrom(device, num);

  int i = 0;
  while(Wire.available())
  { 
    _buff[i] = Wire.read();
    i++;
  }
  Wire.endTransmission();
}



void OnTxDone( void )
{
  turnOffRGB();
  Serial.println("TX done......");
  lora_idle = true;
}

void OnTxTimeout( void )
{
  turnOffRGB();
  Radio.Sleep( );
  Serial.println("TX Timeout......");
  lora_idle = true;
}
