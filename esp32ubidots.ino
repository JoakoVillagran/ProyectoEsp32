#include <UbiConstants.h>
#include <UbiTypes.h>
#include <UbidotsEsp32Mqtt.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

const char *UBIDOTS_TOKEN = "BBUS-SPupuFhC6nBfigumjWpgANguDI2ccG";
const char *WIFI_SSID = "Villagran's";      
const char *WIFI_PASS = "papitobonito1234";     
const char *DEVICE_LABEL = "bme280";
const char *VARIABLE_LABEL_1 = "temperatura";  
const char *VARIABLE_LABEL_2 = "luz"; // Para la luz
const int PUBLISH_FREQUENCY = 10000;
const int PHOTORESISTOR_PIN = 34; // GPIO 34
unsigned long timer;



Ubidots ubidots(UBIDOTS_TOKEN);


void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  
  Serial.begin(115200);     
  Serial.println("Medición inciada");
  Serial.println("BME280 Encendido");

 
  if (!bme.begin(0x77)) { 
    Serial.println("No se encuentra el BME280");
    while (1);
  }
  
  
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  
  ubidots.setCallback(callback);
  
  ubidots.setup();
  ubidots.reconnect();
  timer = millis();
}


void loop() {
  
  if (!ubidots.connected()) {
    ubidots.reconnect();
  }

  
  if ((millis() - timer) > PUBLISH_FREQUENCY) {
    
    float t = bme.readTemperature();  // Leer temperatura

    int lightValue = analogRead(PHOTORESISTOR_PIN);
    
    float lightStatus = lightValue > 2000 ? 1 : 0; // 1 si la luz está encendida, 0 si está apagada

    ubidots.add(VARIABLE_LABEL_1, t);
    ubidots.add(VARIABLE_LABEL_2, lightStatus);
    
    ubidots.publish(DEVICE_LABEL);
    
    Serial.println("Enviando los datos a Ubidots: ");
    Serial.println("Temperatura: " + String(t));
    Serial.println("Luz: " + String(lightStatus ? "Encendida" : "Apagada"));
    Serial.println("-----------------------------------------");
    
    timer = millis();
  }

  delay(10000); 
  ubidots.loop();
}

