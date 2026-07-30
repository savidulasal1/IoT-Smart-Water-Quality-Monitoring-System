#include <WiFi.h>
#include <FirebaseESP32.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/************ 1. NETWORK & FIREBASE CONFIG ************/
#define WIFI_SSID "UOC_Staff"
#define WIFI_PASSWORD "admin106"
#define FIREBASE_HOST "aquariummonitor-7da3e-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "VmInuMP66sk3zSnBnl8XI14uJeIJYwYqbciCwLHS"

/************ 2. PIN DEFINITIONS ************/
#define PH_SENSOR_PIN      7
#define TDS_PIN            6
#define TURBIDITY_PIN      5
#define ONE_WIRE_BUS       4

#define ADC_MAX 4095.0
#define VREF 3.3

/************ 3. OBJECTS ************/
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

FirebaseData fbdo;
FirebaseAuth fb_auth;
FirebaseConfig fb_config;

/************ 4. SENSOR FUNCTIONS (With Your Original Sorting Logic) ************/

#define PH_SAMPLES 25
float readPHVoltage() {
  int buffer[PH_SAMPLES];
  for (int i = 0; i < PH_SAMPLES; i++) {
    buffer[i] = analogRead(PH_SENSOR_PIN);
    delay(10);
  }
  // Your Original Sort
  for (int i = 0; i < PH_SAMPLES - 1; i++) {
    for (int j = i + 1; j < PH_SAMPLES; j++) {
      if (buffer[i] > buffer[j]) {
        int temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
      }
    }
  }
  long sum = 0;
  for (int i = 5; i < PH_SAMPLES - 5; i++) {
    sum += buffer[i];
  }
  float avgADC = sum / (float)(PH_SAMPLES - 10);
  return avgADC * VREF / ADC_MAX;
}

#define TDS_SAMPLES 20
int getTDSFilteredADC() {
  int samples[TDS_SAMPLES];
  for(int i=0; i<TDS_SAMPLES; i++){
    samples[i] = analogRead(TDS_PIN);
    delay(10);
  }
  for(int i=0;i<TDS_SAMPLES-1;i++){
    for(int j=i+1;j<TDS_SAMPLES;j++){
      if(samples[j] < samples[i]){
        int temp = samples[i];
        samples[i] = samples[j];
        samples[j] = temp;
      }
    }
  }
  long sum = 0;
  for(int i=4;i<TDS_SAMPLES-4;i++) sum += samples[i];
  return sum / (TDS_SAMPLES-8);
}

#define TURB_SAMPLES 15
int getTurbidityFilteredADC() {
  int samples[TURB_SAMPLES];
  for(int i=0;i<TURB_SAMPLES;i++){
    samples[i] = analogRead(TURBIDITY_PIN);
    delay(5);
  }
  for(int i=0;i<TURB_SAMPLES-1;i++){
    for(int j=i+1;j<TURB_SAMPLES;j++){
      if(samples[j] < samples[i]){
        int temp = samples[i];
        samples[i] = samples[j];
        samples[j] = temp;
      }
    }
  }
  long sum = 0;
  for(int i=2;i<TURB_SAMPLES-2;i++) sum += samples[i];
  return sum / (TURB_SAMPLES-4);
}

float readAverageTemp() {
  float sum = 0;
  int validSamples = 0;
  for (int i = 0; i < 5; i++) {
    sensors.requestTemperatures();
    float t = sensors.getTempCByIndex(0);
    if (t != DEVICE_DISCONNECTED_C && t > -55 && t < 125) {
      sum += t;
      validSamples++;
    }
    delay(200);
  }
  return (validSamples == 0) ? DEVICE_DISCONNECTED_C : (sum / validSamples);
}

/************ 5. SETUP ************/
void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  sensors.begin();
  sensors.setResolution(9);

  // WiFi Connection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");

  // Firebase Config
  fb_config.host = FIREBASE_HOST;
  fb_config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&fb_config, &fb_auth);
  Firebase.reconnectWiFi(true);

  Serial.println("SMART FISH TANK MONITOR STARTED");
}

/************ 6. LOOP ************/
void loop() {
  // pH Calculation
  float volt = readPHVoltage();
  float ph = (2.9323 * volt * volt) + (-24.8880 * volt) + 45.0972;
  if (volt >= 2.5 && volt <= 2.7) ph = 6.7 + (volt - 2.5) * (0.7 / 0.2);
  ph = constrain(ph, 0.0, 14.0);

  // TDS Calculation
  int tdsADC = getTDSFilteredADC();
  float tdsVolt = round((analogRead(TDS_PIN) * (3.3 / 4095.0)) * 1000) / 1000.0;
  float tds = (-31.74 * tdsVolt * tdsVolt) + (447.3016 * tdsVolt) + 20;

  // Temp Calculation
  float tempC = readAverageTemp();

  // Turbidity Calculation
  int turbADC = getTurbidityFilteredADC();
  float turbVolt = round((turbADC * (VREF / ADC_MAX)) * 1000.0) / 1000.0;
  float turbidity = (106.8 * sq(turbVolt)) - (653.9 * turbVolt) + 768;
  if(turbidity < 0) turbidity = 0;

  // Local Output
  Serial.println("=========== WATER QUALITY ===========");
  Serial.printf("pH: %.2f | TDS: %.1f | Temp: %.1f | Turb: %.1f\n", ph, tds, tempC, turbidity);
  Serial.println("====================================\n");

  // Firebase Upload
  FirebaseJson json;
  json.add("ph", ph);
  json.add("tds", tds);
  json.add("temp", tempC);
  json.add("turb", turbidity);

  if (Firebase.setJSON(fbdo, "/aquariums/Dambulla_Branch/tank_1", json)) {
    Serial.println("Firebase update successful.");
  } else {
    Serial.println("Firebase update failed: " + fbdo.errorReason());
  }

  delay(3000); 
}