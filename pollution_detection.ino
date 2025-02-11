#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Pin Definitions
#define MQ_PIN 34  // MQ-3 Alcohol Sensor
#define SOIL_SENSOR_PIN 35  // Soil Moisture Sensor
#define ONE_WIRE_BUS 4  // DS18B20 Temperature Sensor

// Initialize LCD (16x2) with I2C Address 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Set up OneWire instance and DallasTemperature library
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  // Initialize sensors
  sensors.begin();

  lcd.setCursor(0, 0);
  lcd.print("Sensors Ready!");
  delay(2000);
  lcd.clear();
}

void loop() {
  displayAlcoholSensor();
  delay(2000);
  displaySoilMoisture();
  delay(2000);
  displayTemperature();
  delay(2000);
}

void displayAlcoholSensor() {
  int sensorValue = analogRead(MQ_PIN);
  float ppm = map(sensorValue, 0, 4095, 0, 220);
  String airQuality = classifyAirQuality(ppm);

  Serial.print("Alcohol PPM: ");
  Serial.println(ppm);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AQI: ");
  lcd.print(ppm);
  lcd.setCursor(0, 1);
  lcd.print(airQuality);
}

void displaySoilMoisture() {
  int sensorValue = analogRead(SOIL_SENSOR_PIN);
  float moisturePercent = map(sensorValue, 4095, 0, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);
  String soilCondition = getSoilCondition(moisturePercent);

  Serial.print("Soil Moisture: ");
  Serial.print(moisturePercent);
  Serial.println("%");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moisture: ");
  lcd.print(moisturePercent);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print(soilCondition);
}

void displayTemperature() {
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  if (temperatureC == DEVICE_DISCONNECTED_C) {
    Serial.println("Error: DS18B20 not found!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error!");
    return;
  }
  
  float temperatureF = temperatureC * 9.0 / 5.0 + 32.0;

  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.print("C / ");
  Serial.print(temperatureF);
  Serial.println("F");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperatureC, 1);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temperatureF, 1);
  lcd.print((char)223);
  lcd.print("F");
}

String classifyAirQuality(float ppm) {
  if (ppm < 50) return "Good";
  else if (ppm < 100) return "Moderate";
  else if (ppm < 150) return "Unhealthy";
  else if (ppm < 200) return "Very Unhealthy";
  else return "Hazardous";
}

String getSoilCondition(float moisture) {
  if (moisture > 80) return "Very Wet";
  else if (moisture > 60) return "Moist";
  else if (moisture > 40) return "Normal";
  else if (moisture > 20) return "Dry";
  else return "Very Dry";
}
