#include <Button.h>
#include <ESP8266WiFi.h>  // https://github.com/esp8266/Arduino
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
// define los pines de los segmentos del display
#define segA 16                     // D0 
#define segB 5                      // D1
#define segC 4                      // D2
#define segD 0                      // D3
#define segE 2                      // D4
#define segF 14                     // D5
#define segG 12                     // D6
#define dispDecenas 13              // D7
#define dispUnidades 15             // D8
#define dispSeries 10               // SD3
#define sensorMgn 9                 // SD2

Button sensor(sensorMgn);
DNSServer dnsServer;
ESP8266WebServer server(80);

// Define the digit patterns for numbers 0 to 9 a to f
byte digitos[10][7] = {
  {0, 0, 0, 0, 0, 0, 1},  // Digit 0
  {1, 0, 0, 1, 1, 1, 1},  // Digit 1
  {0, 0, 1, 0, 0, 1, 0},  // Digit 2
  {0, 0, 0, 0, 1, 1, 0},  // Digit 3
  {1, 0, 0, 1, 1, 0, 0},  // Digit 4
  {0, 1, 0, 0, 1, 0, 0},  // Digit 5
  {0, 1, 0, 0, 0, 0, 0},  // Digit 6
  {0, 0, 0, 1, 1, 1, 1},  // Digit 7
  {0, 0, 0, 0, 0, 0, 0},  // Digit 8
  {0, 0, 0, 0, 1, 0, 0},  // Digit 9

};

byte pattern[8]; // patron para dibujar en el display

unsigned long millisPulsacion = 0;       // Variable timestamp
unsigned long intervaloReset = 3500;     // 3.5 segundos
unsigned long intervaloSeries = 1500;    // 1.5 segundos

int piletas = 0;                    // Variable to increment
int series = 0;                     // Variable to increment
int displayLED = 0;                 // displays | 0 = unidades | 1 = decenas | 2 = series |

void setup() {
  // para darle consumo al chip y que la bateria no se apague sola
  IPAddress IP(10, 1, 1, 1);
  IPAddress gateway(10, 1, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.mode(WIFI_AP); 
  WiFi.persistent(true);
  WiFi.disconnect();
  WiFi.softAPConfig(IP, gateway, subnet);
  WiFi.softAP("Piletazo");
  dnsServer.start(53, "*", IP);
  server.begin();  // Inicia el Servidor

  // Set segment and digit pins as outputs
  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT);
  pinMode(segG, OUTPUT);
  pinMode(dispDecenas, OUTPUT);
  pinMode(dispUnidades, OUTPUT);
  pinMode(dispSeries, OUTPUT);

  sensor.begin(); // inicializa boton

  digitalWrite(dispDecenas, LOW);   // apaga los displays
  digitalWrite(dispUnidades, LOW);
  digitalWrite(dispSeries, LOW);
}

void loop() {
  unsigned long currentMillis = millis();

  if(sensor.pressed()) {
    millisPulsacion = millis();
    piletas++;
  }
  
  if(sensor.released()) {
    if (currentMillis - millisPulsacion >= intervaloReset) {
      piletas = 0;
      series = 0;
    } else if (currentMillis - millisPulsacion >= intervaloSeries) {
      series++;
      piletas = 0;
    }
  }

  imprimirEnPantalla(piletas, series, displayLED);   
  displayLED = (displayLED + 1) % 3;  // Increment dispLed and wrap around to 0 after reaching 2
  delay(5);
}

void imprimirEnPantalla(int piletas, int series, int display) {
  // apaga los displays
  digitalWrite(dispDecenas, LOW);
  digitalWrite(dispUnidades, LOW);
  digitalWrite(dispSeries, LOW);
 
 int valor = 0;
  switch (display) {
    case 0:
      digitalWrite(dispUnidades, HIGH);
      valor = piletas % 10; // valor de las unidades
      for (int i = 0; i < 8; i++) {
        pattern[i] = digitos[valor][i];
      }
      break;
    case 1:
      digitalWrite(dispDecenas, HIGH);  
      valor = (piletas / 10) % 10; // valor de las decenas
      for (int i = 0; i < 8; i++) {
        pattern[i] = digitos[valor][i];
      }
      break;
    case 2:
      digitalWrite(dispSeries, HIGH);
      for (int i = 0; i < 8; i++) {
        pattern[i] = digitos[series][i];
      }
      break;
  }
  // Display the segments for the current digit
  digitalWrite(segA, pattern[0]);
  digitalWrite(segB, pattern[1]);
  digitalWrite(segC, pattern[2]);
  digitalWrite(segD, pattern[3]);
  digitalWrite(segE, pattern[4]);
  digitalWrite(segF, pattern[5]);
  digitalWrite(segG, pattern[6]);
}