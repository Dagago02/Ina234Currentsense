#include <Wire.h>
#include <WiFi.h>
#include <esp_sleep.h>

// Datos de red Wi-Fi
const char* ssid = "nombre_de_la_red_wifi";
const char* password = "contraseña_de_la_red_wifi";

// Dirección del sensor de voltaje y corriente I2C
const int i2cAddress = 0x10;

// Variables para almacenar los valores de voltaje y corriente
float voltage = 0.0;
float current = 0.0;

// Intervalo de tiempo (en segundos) entre mediciones
const int measurementInterval = 1800; // 30 minutos

// Configuración del temporizador de despertador
RTC_DATA_ATTR int bootCount = 0;
const int sleepTimeSeconds = measurementInterval;

// Inicialización de la conexión Wi-Fi
void setupWiFi() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a la red Wi-Fi...");
  }

  Serial.println("Conectado a la red Wi-Fi");
  Serial.print("Dirección IP asignada: ");
  Serial.println(WiFi.localIP());
}

// Función para leer los datos del sensor de voltaje y corriente
void readSensorData() {
  Wire.beginTransmission(i2cAddress);
  Wire.write(0x00); // Dirección de lectura de los datos en el sensor
  Wire.endTransmission();

  Wire.requestFrom(i2cAddress, 8); // Se solicitan 8 bytes de datos (4 bytes para voltaje y 4 bytes para corriente)

  // Lectura del voltaje (4 bytes)
  uint32_t voltageData = 0;
  for (int i = 0; i < 4; i++) {
    voltageData |= (Wire.read() << (8 * i));
  }
  voltage = ((float)&voltageData); // Conversión de los bytes a un valor de punto flotante

  // Lectura de la corriente (4 bytes)
  uint32_t currentData = 0;
  for (int i = 0; i < 4; i++) {
    currentData |= (Wire.read() << (8 * i));
  }
  current = ((float)&currentData); // Conversión de los bytes a un valor de punto flotante
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  setupWiFi();

  // Incrementar el contador de inicio y mostrar el número de reinicios
  bootCount++;
  Serial.println("Número de reinicios: " + String(bootCount));

  // Configurar el temporizador de despertador
  esp_sleep_enable_timer_wakeup(sleepTimeSeconds * 1000000);
}

void loop() {
  readSensorData();

  Serial.print("Voltaje: ");
  Serial.print(voltage);
  Serial.print(" V, Corriente: ");
  Serial.print(current);
  Serial.println(" A");

  // Realizar las operaciones necesarias para transmitir los datos al celular
  // ...

  // Volver al modo de sueño profundo
  Serial.println("Entrando en modo de sueño profundo");
  esp_deep_sleep_start();
}
