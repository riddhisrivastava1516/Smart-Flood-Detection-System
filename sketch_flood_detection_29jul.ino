// Guardian Node Simulation for Proteus using Arduino Uno
// ** CORRECTED VERSION WITHOUT INTERRUPTS **

// --- PIN DEFINITIONS ---
const int WATER_LEVEL_PIN = A0;
const int ATMOS_PRESSURE_PIN = A1;
const int RAIN_GAUGE_PIN = 2;
const int FAILSAFE_PROBE_PIN = 3;
const int GREEN_LED_PIN = 8;
const int RED_LED_PIN = 9;
const int SIREN_PIN = 10;

// --- VARIABLES ---
int waterLevel = 0;
int waterLevelPercent = 0;
int atmosPressure = 0;
int atmosPressurePercent = 0;
int rainTipCount = 0;
bool failsafeTripped = false;

// --- THRESHOLDS ---  // <-- This section was missing
const int WATER_WARNING_LEVEL = 75;
const int PRESSURE_WATCH_LEVEL = 20;

void setup() {
  Serial.begin(9600);
  Serial.println("Guardian Node Simulation Initialized (No Interrupts).");
  Serial.println("------------------------------------");

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(SIREN_PIN, OUTPUT);
  pinMode(RAIN_GAUGE_PIN, INPUT);
  pinMode(FAILSAFE_PROBE_PIN, INPUT);

  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(SIREN_PIN, LOW);
}

void loop() {
  readAllSensors();
  checkForAlerts();
  printStatus();
  delay(5000); 
}

void readAllSensors() {
  waterLevel = analogRead(WATER_LEVEL_PIN);
  atmosPressure = analogRead(ATMOS_PRESSURE_PIN);
  waterLevelPercent = map(waterLevel, 0, 1023, 0, 100);
  atmosPressurePercent = map(atmosPressure, 0, 1023, 0, 100);

  if (digitalRead(RAIN_GAUGE_PIN) == LOW) {
    rainTipCount++;
    delay(300); 
  }

  if (digitalRead(FAILSAFE_PROBE_PIN) == LOW) {
    failsafeTripped = true;
  } else {
    failsafeTripped = false;
  }
}

void checkForAlerts() {
  if (failsafeTripped) {
    activateCriticalAlert();
    return;
  }
  if (waterLevelPercent > WATER_WARNING_LEVEL) {
    activateWarningAlert();
  } else if (atmosPressurePercent < PRESSURE_WATCH_LEVEL) {
    activateWatchAlert();
  } else {
    setSystemNormal();
  }
}

void printStatus() {
  Serial.println("\n--- Sensor Status Report ---");
  Serial.print("Water Level: ");
  Serial.print(waterLevelPercent);
  Serial.println("%");
  Serial.print("Atmospheric Pressure: ");
  Serial.print(atmosPressurePercent);
  Serial.println("%");
  Serial.print("Rain Tip Count: ");
  Serial.println(rainTipCount);
  Serial.print("Failsafe Tripped: ");
  Serial.println(failsafeTripped ? "YES" : "NO");
  Serial.println("--- End of Report ---");
}

void setSystemNormal() {
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(SIREN_PIN, LOW);
}

void activateWatchAlert() {
  Serial.println("ALERT: Level 1 (WATCH) - Pressure dropping.");
  digitalWrite(GREEN_LED_PIN, !digitalRead(GREEN_LED_PIN)); 
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(SIREN_PIN, LOW);
}

void activateWarningAlert() {
  Serial.println("ALERT: Level 2 (WARNING) - High water level detected.");
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(SIREN_PIN, LOW);
}

void activateCriticalAlert() {
  Serial.println("!!! ALERT: Level 3 (CRITICAL) - FAIL-SAFE PROBE TRIPPED. EVACUATE! !!!");
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, !digitalRead(RED_LED_PIN)); 
  digitalWrite(SIREN_PIN, HIGH);; 
}