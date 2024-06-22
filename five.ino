#include <OneWire.h>
#include <DallasTemperature.h>
#include <Arduino.h>

// Define sensor pins (adjust based on your setup)
const int tdsSensorPin = A0;
const int turbiditySensorPin = A1;
const int temperatureSensorPin = 4;
const int phSensorPin = A2;
const int waterFlowSensorPin = 2; // Assuming water flow sensor uses pin 2

// Variables for sensor readings
float voltage, tdsValue;
int sensorValue;
int turbidity;
float celsius, fahrenheit;
float phValue;
volatile long pulse = 0; // Variable for water flow sensor pulse count
float volume;

// Create objects for DS18B20 sensor communication
OneWire oneWire(temperatureSensorPin);
DallasTemperature sensors(&oneWire);

// Calibration value for pH sensor (adjust based on your sensor datasheet)
float calibration_value = 21.34 + 0.7;

void setup() {
  Serial.begin(9600);

  // Initialize TDS and turbidity sensor readings
  Serial.println("Starting TDS sensor reading...");
  Serial.println("Starting turbidity sensor reading...");



  // Initialize pH sensor pin as input
  pinMode(phSensorPin, INPUT);

  // Initialize water flow sensor pin as input with interrupt
  pinMode(waterFlowSensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(waterFlowSensorPin), increase, RISING);

  Serial.println("Welcome To Multi-Sensor Monitoring");

}

void loop() {
  // Read TDS sensor
  voltage = analogRead(tdsSensorPin) * 5.0 / 1024.0; // Convert analog reading to voltage (assuming 5V reference)
  // You might need to adjust the conversion factor (5.0 / 1024.0) based on your specific sensor and ADC resolution

  // **Optional: Apply a calibration factor if needed**
  // tdsValue = voltage * calibrationFactor;

  tdsValue = (voltage * 146); // Example conversion formula, might need adjustment based on your sensor datasheet
  Serial.print("TDS Value (ppm): ");
  Serial.println(tdsValue);

  // Read turbidity sensor
  sensorValue = analogRead(turbiditySensorPin);
  turbidity = map(sensorValue, 0, 750, 100, 0); // Adjust thresholds based on your sensor datasheet

  Serial.print("Turbidity: ");
  Serial.print(turbidity);

  if (turbidity < 20) {
    Serial.println(" - Water is Clear");
  } else if (turbidity >= 20 && turbidity < 50) {
    Serial.println(" - Water is Cloudy");
  } else {
    Serial.println(" - Water is Dirty");
  }

  // Read temperature sensor (DS18B20)
  sensors.requestTemperatures();
  celsius = sensors.getTempCByIndex(0);
  fahrenheit = sensors.toFahrenheit(celsius);

  Serial.print("Temperature: ");
  Serial.print(celsius);
  Serial.print(" °C  ");
  Serial.print(fahrenheit);
  Serial.println(" °F");

  // Read pH sensor
  unsigned long int avgValue = 0;
  int buf[10], temp;

  for (int i = 0; i < 10; i++) {
    buf[i] = analogRead(phSensorPin);
    delay(10);
  }

  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }

  for (int i = 2; i < 8; i++) {
    avgValue += buf[i];
  }

  phValue = (float)avgValue * 5.0 / 1024 / 6; // Convert analog value into millivolt
  phValue = -5.70 * phValue + calibration_value; // Convert millivolt into pH value


  Serial.print("pH Value: ");
  Serial.println(phValue, 2);  // Print pH value with 2 decimal places



  volume = 2.663 * pulse;
  Serial.print(volume);
  Serial.println(" mL");
 

  delay(3000);
  Serial.print("\n\n");

}


void increase() {
  pulse++;
}