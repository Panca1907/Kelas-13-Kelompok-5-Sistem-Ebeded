#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Pin definisi
#define TDS_PIN A0         // Pin sensor TDS
#define RELAY_OUT_PIN 7    // Pin relay untuk pompa keluar (air kotor)
#define RELAY_IN_PIN 5     // Pin relay untuk pompa masuk (air bersih)
#define LED_PIN 8          // Pin LED indikator
#define ONE_WIRE_BUS 3     // Pin data untuk sensor suhu DS18B20

// Inisialisasi objek
LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat I2C untuk LCD 16x2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);

// Variabel
float tdsValue = 0;          // Nilai TDS
float temperature = 0;       // Nilai suhu
const int TDS_THRESHOLD = 500;  // Ambang batas TDS (ppm)
const int TEMP_THRESHOLD = 30;  // Ambang batas suhu (Celsius)

// State Pompa
bool pumpState = false; // false = Off, true = On

void setup() {
  // Inisialisasi LCD, sensor, dan pin
  lcd.begin(16, 2);
  lcd.backlight();
  tempSensor.begin();
  pinMode(RELAY_OUT_PIN, OUTPUT);
  pinMode(RELAY_IN_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Matikan semua output di awal
  digitalWrite(RELAY_OUT_PIN, LOW);
  digitalWrite(RELAY_IN_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  lcd.setCursor(0, 0);
  lcd.print("Sistem Mulai");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Membaca nilai suhu
  tempSensor.requestTemperatures();
  temperature = tempSensor.getTempCByIndex(0);

  // Map suhu dari 0 ke 50 (asumsi rentang sensor DS18B20)
  temperature = map(temperature, 0, 50, 0, 50);

  // Membaca nilai TDS
  int tdsAnalog = analogRead(TDS_PIN);
  tdsValue = map(tdsAnalog, 0, 1023, 0, 1000); // Konversi ke ppm

  // Tampilkan data pada LCD
  lcd.setCursor(0, 0);
  lcd.print("TDS: ");
  lcd.print(tdsValue);
  lcd.print(" ppm");

  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");

  // Logika kontrol kualitas air
  if (tdsValue < TDS_THRESHOLD && temperature < TEMP_THRESHOLD) {
    pumpState = false; // Baik
  } else {
    pumpState = true;  // Buruk
  }

  // Kontrol pompa berdasarkan state
  if (pumpState) {
    digitalWrite(LED_PIN, HIGH);      // Hidupkan LED peringatan
    
    digitalWrite(RELAY_OUT_PIN, HIGH); // Hidupkan pompa keluar
    delay(5000);                      // Tunggu 5 detik
    digitalWrite(RELAY_OUT_PIN, LOW); // Matikan pompa keluar

    digitalWrite(RELAY_IN_PIN, HIGH); // Hidupkan pompa masuk
    delay(5000);                      // Tunggu 5 detik
    digitalWrite(RELAY_IN_PIN, LOW);  // Matikan pompa masuk

  } else {
    digitalWrite(LED_PIN, LOW);       // Matikan LED jika kualitas air aman
  }

  // Tampilkan status pompa di Serial Monitor
  Serial.print("Pompa State: ");
  Serial.println(pumpState ? "On" : "Off");

  delay(1000); // Tunggu 1 detik sebelum membaca kembali
}
