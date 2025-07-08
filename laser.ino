#include <Wire.h>
#include "Adafruit_VL53L0X.h"

// Crea una instancia del sensor
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(1); // Espera que el puerto serie esté listo

  Serial.println("Iniciando VL53L0X...");

  if (!lox.begin()) {
    Serial.println("¡Error al iniciar el sensor VL53L0X!");
    while (1);
  }

  Serial.println("Sensor VL53L0X listo.");
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;

  lox.rangingTest(&measure, false); // 'true' para mostrar detalles

  if (measure.RangeStatus != 4) { // 4 = error
    Serial.print("Distancia: ");
    Serial.print(measure.RangeMilliMeter);
    Serial.println(" mm");
  } else {
    Serial.println("Error al medir");
  }

  delay(500); // Medio segundo entre lecturas
}