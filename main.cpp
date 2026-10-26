/*
 * Ultrasonic Parking Sensor (Arduino Uno + HC-SR04)
 *
 * Measures distance with an HC-SR04, filters the readings with a median of
 * several samples, and reports proximity with three LEDs and a buzzer whose
 * beep rate increases as an obstacle gets closer.
 *
 * Zones:
 *   > 100 cm   green LED, silent
 *   50-100 cm  yellow LED, slow beeps
 *   20-50 cm   yellow LED, beeps get faster as distance shrinks
 *   < 20 cm    red LED, continuous tone
 */
#include <Arduino.h>

// ---- Pin assignment -------------------------------------------------------
constexpr uint8_t TRIG_PIN   = 9;
constexpr uint8_t ECHO_PIN   = 10;
constexpr uint8_t BUZZER_PIN = 8;
constexpr uint8_t LED_GREEN  = 4;
constexpr uint8_t LED_YELLOW = 5;
constexpr uint8_t LED_RED    = 6;

// ---- Measurement settings -------------------------------------------------
constexpr float         MAX_RANGE_CM    = 200.0f;
constexpr unsigned long ECHO_TIMEOUT_US = 12000UL;  // ~205 cm round trip
constexpr uint8_t       SAMPLES         = 5;        // median filter size
constexpr uint16_t      BEEP_FREQ_HZ    = 2000;
constexpr uint16_t      BEEP_LENGTH_MS  = 60;

// Single distance measurement in cm; returns MAX_RANGE_CM on timeout.
float readDistanceOnce() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long echoUs = pulseIn(ECHO_PIN, HIGH, ECHO_TIMEOUT_US);
  if (echoUs == 0) {
    return MAX_RANGE_CM;
  }
  // Speed of sound ~0.0343 cm/us, divide by 2 for the round trip.
  float cm = echoUs * 0.0343f / 2.0f;
  return cm > MAX_RANGE_CM ? MAX_RANGE_CM : cm;
}

// Median of SAMPLES measurements rejects single-reading spikes.
float readDistanceMedian() {
  float v[SAMPLES];
  for (uint8_t i = 0; i < SAMPLES; i++) {
    v[i] = readDistanceOnce();
    delay(12);  // let echoes die out between pings
  }
  // Insertion sort (tiny array).
  for (uint8_t i = 1; i < SAMPLES; i++) {
    float key = v[i];
    int8_t j = i - 1;
    while (j >= 0 && v[j] > key) {
      v[j + 1] = v[j];
      j--;
    }
    v[j + 1] = key;
  }
  return v[SAMPLES / 2];
}

// Returns beep period in ms: 0 = silent, 1 = continuous tone.
unsigned long beepPeriodFor(float cm) {
  if (cm > 100.0f) return 0;
  if (cm > 50.0f)  return 600;
  if (cm > 20.0f)  return (unsigned long)(150.0f + (cm - 20.0f) * (450.0f / 30.0f));
  return 1;
}

void setLeds(bool green, bool yellow, bool red) {
  digitalWrite(LED_GREEN, green);
  digitalWrite(LED_YELLOW, yellow);
  digitalWrite(LED_RED, red);
}

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  Serial.begin(115200);
  Serial.println(F("Ultrasonic parking sensor ready"));
}

void loop() {
  static unsigned long lastBeep = 0;

  float cm = readDistanceMedian();
  unsigned long period = beepPeriodFor(cm);

  if (cm > 100.0f)      setLeds(true, false, false);
  else if (cm >= 20.0f) setLeds(false, true, false);
  else                  setLeds(false, false, true);

  if (period == 0) {
    noTone(BUZZER_PIN);
  } else if (period == 1) {
    tone(BUZZER_PIN, BEEP_FREQ_HZ);
  } else if (millis() - lastBeep >= period) {
    tone(BUZZER_PIN, BEEP_FREQ_HZ, BEEP_LENGTH_MS);
    lastBeep = millis();
  }

  Serial.print(F("distance_cm="));
  Serial.println(cm, 1);
}
