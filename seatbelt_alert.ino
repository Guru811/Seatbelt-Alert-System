const int SEAT_SENSOR_PIN = 2;
const int BELT_SENSOR_PIN = 3;
const int BUZZER_PIN      = 8;
const int LED_PIN         = 13;

unsigned long lastBeepTime = 0;
bool alertActive           = false;
bool lastSafeState = false;

void setup() {
  pinMode(SEAT_SENSOR_PIN, INPUT_PULLUP);
  pinMode(BELT_SENSOR_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  noTone(BUZZER_PIN);
  digitalWrite(LED_PIN, LOW);
  Serial.begin(9600);
  Serial.println("System Ready.");
}

void loop() {
  bool personSeated = (digitalRead(SEAT_SENSOR_PIN) == LOW);
  bool beltBuckled  = (digitalRead(BELT_SENSOR_PIN) == LOW);

  if (!personSeated) {
    // seat empty
    if (alertActive) {
      Serial.println("SEAT EMPTY: No one in seat.");
    }
    resetAlert();
    lastSafeState = false;  // reset safe tracking

  } else if (personSeated && !beltBuckled) {
    // seated but not buckled
    lastSafeState = false;
    if (!alertActive) {
      alertActive  = true;
      lastBeepTime = millis();
      Serial.println("ALERT: Person seated, belt not fastened!");
    }
    runAlert();

  } else if (personSeated && beltBuckled) {
    // seated AND buckled — SAFE
    if (alertActive || !lastSafeState) {
      Serial.println("SAFE: Person seated and belt fastened correctly.");
      lastSafeState = true;
    }
    resetAlert();
  }
}

void runAlert() {
  unsigned long now     = millis();
  unsigned long cyclePos = (now - lastBeepTime) % 600;

  // Level 3 beep — 100ms ON, 150ms OFF at 2800Hz
  if (cyclePos < 100) {
    tone(BUZZER_PIN, 2800);
  } else {
    noTone(BUZZER_PIN);
  }

  // Fast blinking LED
  digitalWrite(LED_PIN, (millis() / 150) % 2 == 0 ? HIGH : LOW);
}

void resetAlert() {
  alertActive = false;
  noTone(BUZZER_PIN);
  digitalWrite(LED_PIN, LOW);
}
