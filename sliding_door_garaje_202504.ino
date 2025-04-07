/*********************************************************************
   Automatic sliding door for garage
   Control de puerta corredera de garage
   La apertura se realiza con mando RF es una placa aparte y utilizo la salida de contacto seco del relé.
   El cierre se realiza automaticamente según el tiempo indicado por defecto 15 segundos.
   Por seguridad tiene una célula fotoeléctrica que se activa en caso de interponerse en la barrera y evitar atrapamiento.
   Dispone de un rele seco manejado por el controlador Zigbee, entonces la apertura del garage a través del tablero del vehículo con android auto y home assistant.
   Se puede obviar el montaje de pantalla, no es necesario para el funcionamiento.
   Puede elegir entre dos tipos de pantalla, la SSD1306 de 4 pines (https://s.click.aliexpress.com/e/_oB2gdBL) y la sh1106 de 7 pines (https://s.click.aliexpress.com/e/_oofWsrf)
   La placa optoacoplada de tres reles que he utilizado es esta https://s.click.aliexpress.com/e/_oFawcp3
   La fuente de alimentación 220v AC a 5v DC es esta https://s.click.aliexpress.com/e/_oFyz5GN
   He utilizado un conector de 3 pines y el resto son de 2 pines, los conectores son estos https://s.click.aliexpress.com/e/_oph4j1X
*********************************************************************/
//   Librerias
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h> //para pantalla oled de 4 pines
#include <Adafruit_SH1106.h> //para pantalla oled de 7 pines
//#include <Adafruit_SH110X.h>//Ojo NO utlilizar esta librería no va bien con el display sh1106 comprado en Ali (spi de 5 pines más alimentación).

// Definiendo pins
const int FOTOCELULA = 2;
const int FCABI = 3;
const int MandoRF = 4;
const int RELAY_LIGHT = 5;  //Relay 1
const int MOTOR_ABRIR = 6;  //Relay 2
const int FCCER = 7;
//const int MandoZigbee = 4; //utilizo la misma entrada que RF
const int MOTOR_CERRAR = 12;  //Relay   3
const int MandoRPerson = A3;  //D17 ó A3 (por implementar) cuando reciba señal por aquí, la puerta se abrirá parcialmente para el acceso de personas.


// Definiendo pins para pantalla SH1106 de 7 pines:
//OLED GND
//OLE VCC +5V
#define OLED_RESET 8
#define OLED_DC 9
#define OLED_CS 10
#define OLED_MOSI 11
#define OLED_CLK 13
// Para pantalla SSD1306 de 4 pines utilizar:
// Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
// Para pantalla SH1106 de 7 pines utilizar:
Adafruit_SH1106 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


// Definiendo variables globales
unsigned long TIEMPO_PUERTA_ABIERTA = 30000;  // Tiempo de puerta abierta en milisegundos para entrada o salida de vehículo
unsigned long TIEMPO_MAX_RECORRIDO = 600000;   // Tiempo máximo de apertura o cierre para marcar averia (60000 10 minutos)
unsigned long BLINK_LUZ = 1000;        // Ciclo en milisegundos RELAY_LIGHT señalizacion
unsigned long BLINK_LUZ_AVERIA = 500;  // Ciclo RELAY_LIGHT señalizacion
int ledState = LOW;                 // ledState used to set the LED
unsigned long currentMillis = millis();
unsigned long previousMillisblink = millis();   // Tracks the time since last event fired
unsigned long previousMillisaveria = millis();   // Tracks the time since last event fired
unsigned long previousMillispausa = millis();   // Tracks the time since last event fired
boolean AVERIA = false;         //Excedido el TIEMPO de recorrer sin detectar finales de carrera
boolean PuertaAbierta = false;  //true abierta, false cerrada

void setup() {
  Serial.begin(9600);
  // Inicializando los pines
  pinMode(MandoRPerson, INPUT_PULLUP);
  pinMode(FOTOCELULA, INPUT_PULLUP);
  pinMode(MandoRF, INPUT_PULLUP);
  pinMode(FCCER, INPUT_PULLUP);  //FC HIGH=NO DETECTADO Y LOW=DETECTADO
  pinMode(FCABI, INPUT_PULLUP);  //FC HIGH=NO DETECTADO Y LOW=DETECTADO
  pinMode(RELAY_LIGHT, OUTPUT);
  pinMode(MOTOR_ABRIR, OUTPUT);
  pinMode(MOTOR_CERRAR, OUTPUT);
  // Apagar motores de puerta y luz
  digitalWrite(MOTOR_ABRIR, LOW);   //high == start      low==stop
  digitalWrite(MOTOR_CERRAR, LOW);  //high == start      low==stop
  digitalWrite(RELAY_LIGHT, LOW);   //high == start      low==stop
  // Inicializa sensores
  digitalWrite(FCCER, HIGH);       //FC HIGH=NO DETECTADO Y LOW=DETECTADO
  digitalWrite(FCABI, HIGH);       //FC HIGH=NO DETECTADO Y LOW=DETECTADO
  digitalWrite(MandoRF, HIGH);     //FC HIGH=NO DETECTADO Y LOW=DETECTADO
  digitalWrite(FOTOCELULA, HIGH);  //FC HIGH=NO DETECTADO Y LOW=DETECTADO
  
  // Inicializar el display
  // Para SH1106 descomentar utiliza:
  display.begin(SH1106_SWITCHCAPVCC);  // Inicia el display OLED (Carga Buffer)
  // Para SSD1306 descomentar utiliza: 
  //display.begin(SSD1306_SWITCHCAPVCC);
  display.display();  // Muestra pantalla inicial de Adafruit
  delay(1000);
  display.clearDisplay();
  display.display();
  // prepara el display para texto
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

// ==============( Void Loop  ) =========
void loop() {
  mandoDistancia();
  LuzAveria();
  currentMillis = millis();
  if ( ((unsigned long)(currentMillis - previousMillispausa) > TIEMPO_PUERTA_ABIERTA) && (PuertaAbierta == true) ) {
    previousMillispausa = currentMillis;
    cierraPuerta();
  }
  //inicio blink normal
    currentMillis = millis();
    if ( ((unsigned long)(currentMillis - previousMillisblink) > (BLINK_LUZ/2)) && (PuertaAbierta == true) ){
      previousMillisblink = currentMillis;
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      digitalWrite(RELAY_LIGHT, ledState);
    }//fin blink normal

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("LOOP");
  //Serial.println("LOOP");
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print("S.Photo: ");
  display.println(digitalRead(FOTOCELULA));
  display.print("FCCER: ");
  display.println(digitalRead(FCCER));
  display.print("FCABI: ");
  display.println(digitalRead(FCABI));
  display.print("Mando: ");
  display.println(digitalRead(MandoRF));
  display.print("Averia: ");
  display.println(AVERIA);
  display.display();
}
// ========{ Mando distancia }==================
void mandoDistancia() {
  if ((digitalRead(MandoRF) == LOW) && (PuertaAbierta == false)) {
    //Serial.println("Mando abre");
    abrePuerta();
    delay(2000);
  }
  if ((digitalRead(MandoRF) == LOW) && (PuertaAbierta == true)) {
    //Serial.println("Mando cierra");
    cierraPuerta();
    delay(2000);
  }
}
// ========{ abre Puerta }==================
void abrePuerta() {
  PuertaAbierta = true;
  digitalWrite(MOTOR_ABRIR, HIGH);
  delay(1000);
  while ((digitalRead(FCABI) == HIGH)) {  // LOW DETECTA EL FINAL DE CARRERA DE APERTURA
    digitalWrite(MOTOR_ABRIR, HIGH);
    //inicio blink
    currentMillis = millis();
    if ( ((unsigned long)(currentMillis - previousMillisblink) > BLINK_LUZ)  ){
      previousMillisblink = currentMillis;
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      digitalWrite(RELAY_LIGHT, ledState);
    }//fin blink 
  }
  digitalWrite(MOTOR_ABRIR, LOW);  //stop motor
  delay(500);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("ABRE");
  //Serial.println("ABRE");
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print("S.Photo: ");
  display.println(digitalRead(FOTOCELULA));
  display.print("FCCER: ");
  display.println(digitalRead(FCCER));
  display.print("FCABI: ");
  display.println(digitalRead(FCABI));
  display.print("Mando: ");
  display.println(digitalRead(MandoRF));
  display.print("Averia: ");
  display.println(AVERIA);
  display.display();
  //inicio avería, si el tiempo excede del marcado indica averia, para el motor y hará el parpadeo de avería.
  if ((unsigned long)(currentMillis - previousMillisaveria) > TIEMPO_MAX_RECORRIDO){ 
    previousMillisaveria = currentMillis;
    AVERIA = true;
    LuzAveria();
    } 
}
// ========{ cierra Puerta  }==================
void cierraPuerta() {
  digitalWrite(MOTOR_CERRAR, HIGH);  // Activa motor de cierre
  delay(1000);
  while ((digitalRead(FCCER) == HIGH)) {  // LOW DETECTA EL FINAL DE CARRERA DE APERTURA
    digitalWrite(MOTOR_CERRAR, HIGH);     // Activa motor de cierre
    //inicio blink
    currentMillis = millis();
    if ( ((unsigned long)(currentMillis - previousMillisblink) > BLINK_LUZ)  ){
      previousMillisblink = currentMillis;
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      digitalWrite(RELAY_LIGHT, ledState);
    }//fin blink 
    if ((digitalRead(FOTOCELULA) == HIGH) || (digitalRead(MandoRF) == LOW)) { //detecta obstaculo o pulso de mando RF
      digitalWrite(MOTOR_CERRAR, LOW);
      delay(1000);
      PuertaAbierta = true;
      abrePuerta();
      goto exit;
    }
  }
  delay(350);                       // Retrasa el cierre unos milisegundos OJO, esto es opcional, en mi caso lo tengo así para que quede bien cerrada.
  digitalWrite(MOTOR_CERRAR, LOW);  //stop motor
  digitalWrite(RELAY_LIGHT, LOW);
  PuertaAbierta = false;
  delay(500);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("CIERRA");
  //Serial.println("CIERRA");
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print("S.Photo: ");
  display.println(digitalRead(FOTOCELULA));
  display.print("FCCER: ");
  display.println(digitalRead(FCCER));
  display.print("FCABI: ");
  display.println(digitalRead(FCABI));
  display.print("Mando: ");
  display.println(digitalRead(MandoRF));
  display.print("Averia: ");
  display.println(AVERIA);
  display.display();
  //inicio avería, si el tiempo excede del marcado indica averia, para el motor y hará el parpadeo de avería.
  if ((unsigned long)(currentMillis - previousMillisaveria) > TIEMPO_MAX_RECORRIDO){ 
    previousMillisaveria = currentMillis;
    AVERIA = true;
    LuzAveria();
    } 
  //final averia
exit:
  loop();
}


void LuzAveria() { //blink, parpadeo en la luz de avería 
  while (AVERIA == true) {
    digitalWrite(MOTOR_CERRAR, LOW);
    digitalWrite(MOTOR_ABRIR, LOW);
    currentMillis = millis();
    if ((unsigned long)(currentMillis - previousMillisaveria) > BLINK_LUZ_AVERIA) {
      previousMillisaveria = currentMillis;
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      digitalWrite(RELAY_LIGHT, ledState);
    }
  }
}




