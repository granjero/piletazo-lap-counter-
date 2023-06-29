#include <Button.h>

#include <ESP8266WiFi.h>  // https://github.com/esp8266/Arduino
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

DNSServer dnsServer;          // Inicia Servidor DNS (no?)
ESP8266WebServer server(80);  // Inicia Servidor web en el puerto 80


// define los pines de los segmentos
#define segA 16         // D0
#define segB 5          // D1
#define segC 4          // D2
#define segD 0          // D3
#define segE 2          // D4
#define segF 14         // D5
#define segG 12         // D6
// define los pins de los displays
#define dispDecenas 13  // D7
#define dispUnidades 15 // D8
#define dispSeries 10   // SD3
// define el pin del boton
#define boton 9 // boton
Button botonIncremento(boton); 

// Define the digit patterns for numbers 0 to 9 a to f
byte digitos[16][8] = {
  {0, 0, 0, 0, 0, 0, 1, 1},  // Digit 0
  {1, 0, 0, 1, 1, 1, 1, 1},  // Digit 1
  {0, 0, 1, 0, 0, 1, 0, 1},  // Digit 2
  {0, 0, 0, 0, 1, 1, 0, 1},  // Digit 3
  {1, 0, 0, 1, 1, 0, 0, 1},  // Digit 4
  {0, 1, 0, 0, 1, 0, 0, 1},  // Digit 5
  {0, 1, 0, 0, 0, 0, 0, 1},  // Digit 6
  {0, 0, 0, 1, 1, 1, 1, 1},  // Digit 7
  {0, 0, 0, 0, 0, 0, 0, 1},  // Digit 8
  {0, 0, 0, 0, 1, 0, 0, 1},   // Digit 9
  {0, 1, 1, 1, 1, 1, 1, 1},  // Letter A
  {1, 1, 1, 0, 0, 0, 0, 1},  // Letter B
  {0, 1, 1, 0, 0, 1, 1, 1},  // Letter C
  {1, 1, 0, 0, 0, 1, 1, 1},  // Letter D
  {0, 1, 1, 0, 0, 0, 1, 1},  // Letter E
  {0, 1, 1, 1, 0, 0, 1, 1}   // Letter F
};

byte pattern[8]; // patron para dibujar en el display

unsigned long millisPulsacion = 0;       // Variable timestamp
unsigned long intervaloReset = 3500;     // 3.5 segundos
unsigned long intervaloSeries = 1500;    // 1.5 segundos

int piletas_series[2] = {0, 0};     // {piletas, series}
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
  //Serial.println(F("Wifi Iniciado."));
  dnsServer.start(53, "*", IP);
  server.begin();  // Inicia el Servidor

  // inicializa boton
  botonIncremento.begin();
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
  // apaga los leds
  digitalWrite(dispDecenas, LOW);
  digitalWrite(dispUnidades, LOW);
  digitalWrite(dispSeries, LOW);
}

void loop() {
  unsigned long currentMillis = millis();

  if(botonIncremento.pressed())
  {
    millisPulsacion = millis();
    piletas++;
    piletas_series[0] = piletas;
  }
  
  if(botonIncremento.released())
  {
    if (currentMillis - millisPulsacion >= intervaloReset) {
      piletas = 0;
      series = 0;
      piletas_series[0] = piletas;
      piletas_series[1] = series;      

    }
    else if (currentMillis - millisPulsacion >= intervaloSeries) {
      series++;
      piletas = 0;
      piletas_series[0] = piletas;
      piletas_series[1] = series;      

    }
  }
  imprimirEnPantalla(piletas_series, displayLED);   

  displayLED = (displayLED + 1) % 3;  // Increment dispLed and wrap around to 0 after reaching 2

  delay(5);
}

void imprimirEnPantalla(int datos[], int display)
{
  // apaga los displays
  digitalWrite(dispDecenas, LOW);
  digitalWrite(dispUnidades, LOW);
  digitalWrite(dispSeries, LOW);
 
 int valor = 0;
  switch (display) {
    case 0:
      digitalWrite(dispUnidades, HIGH);
      valor = datos[0] % 10;
      for (int i = 0; i < 8; i++) {
        pattern[i] = digitos[valor][i];
      }
      break;
    case 1:
      digitalWrite(dispDecenas, HIGH);  
      valor = (datos[0] / 10) % 10;
      for (int i = 0; i < 8; i++) {
        pattern[i] = digitos[valor][i];
      }
      break;
    case 2:
      digitalWrite(dispSeries, HIGH);
      valor = datos[1];
      for (int i = 0; i < 8; i++) {
        pattern[i] = digitos[valor][i];
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