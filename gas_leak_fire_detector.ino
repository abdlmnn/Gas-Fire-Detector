#define BLYNK_TEMPLATE_ID "TMPL67Jn0uSBm"
#define BLYNK_TEMPLATE_NAME "Fire"
#define BLYNK_AUTH_TOKEN "NsvQ1BhFiWn730rXZ1LlQ_n3Hg_lCp5Z"  

#include <Servo.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "Sandra";         
char pass[] = "Sandra_7102005"; 

#define MQ2_PIN A0
#define FLAME_SENSOR_PIN D1
#define LED_PIN D2
#define BUZZER_PIN D5
#define SERVO_PIN D6

Servo myServo;

int doorValue = 0;
int ledValue = 0;

void setup() {
  Serial.begin(9600);
  pinMode(FLAME_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(0);  

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Connecting to Wi-Fi and Blynk...");
}


BLYNK_WRITE(V4) {
  doorValue = param.asInt();
  if (doorValue == 1) {
    Serial.println("Manual: Opening Door");
    myServo.write(180);
  } else {
    Serial.println("Manual: Closing Door");
    myServo.write(0);
  }
}

BLYNK_WRITE(V2) {
  ledValue = param.asInt();
  if (ledValue == 1) {
    Serial.println("Manual: LED ON");
    digitalWrite(LED_PIN, HIGH);
  } else {
    Serial.println("Manual: LED OFF");
    digitalWrite(LED_PIN, LOW);
  }
}

bool fireAlertSent = false;

void loop() {
  Blynk.run();
  
  int flameDetected = digitalRead(FLAME_SENSOR_PIN);
  int gasValue = analogRead(MQ2_PIN);
  int gasThreshold = 600;

  bool isGas = gasValue > gasThreshold;
  bool isFire = flameDetected == LOW;

  Serial.print("Gas Level: ");
  Serial.println(gasValue);
  Blynk.virtualWrite(V1, gasValue);

  if (isGas && isFire) {
    if (!fireAlertSent) {
      Serial.println("Gas and Fire Detected! Door Opened.");
      Blynk.logEvent("alert", "Gas and Fire Detected! Door Opened.");
      Blynk.virtualWrite(V0, "Gas Detected!");
      Blynk.virtualWrite(V3, "Fire Detected! Door Opened");
      fireAlertSent = true;
    }
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 1000);
    myServo.write(180);
  }

  else if (isGas) {
    Serial.println("Gas Detected!");
    Blynk.logEvent("alert", "Gas Detected!");
    Blynk.virtualWrite(V0, "Gas Detected!");
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 1000);
  }

  else if (isFire) {
    if (!fireAlertSent) {
      Serial.println("Fire Detected! Door opened.");
      Blynk.logEvent("alert", "Fire Detected! Door Opened.");
      Blynk.virtualWrite(V3, "Fire Detected! Door Opened");
      fireAlertSent = true;
    }
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 1000);
    myServo.write(180);
  }

  else {
    if (fireAlertSent) {
      Serial.println("All Safe.");
      Blynk.logEvent("alert", "All Safe.");
      Blynk.virtualWrite(V3, "No Fire");
      fireAlertSent = false;
    }
    noTone(BUZZER_PIN);
    Blynk.virtualWrite(V0, "No Gas Leak");
    if (ledValue != 1) {
      digitalWrite(LED_PIN, LOW);
    }
    if (doorValue != 1) {
      myServo.write(0);
    }
  }
  delay(500);
}












  // if (flameDetected == LOW) {

  //   Serial.println("Fire detected! Opening door.");
  //   digitalWrite(LED_PIN, HIGH);
  //   tone(BUZZER_PIN, 1000);
  //   myServo.write(180);
  //   Blynk.logEvent("fire_detected");
  // } else {
  //   Serial.println("Safe. No fire.");
  //   Blynk.logEvent("No fire detected");
  //   noTone(BUZZER_PIN);

  //   if (ledValue != 1) {
  //     digitalWrite(LED_PIN, LOW);  
  //   }

  //   if (doorValue != 1) {
  //     myServo.write(0);  
  //   }

  // }
