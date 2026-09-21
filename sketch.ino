#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
// RFID pins
#define SS_PIN 10
#define RST_PIN 9

// Sensors
#define PIR_PIN 2
#define BUZZER_PIN 8
#define LED_PIN 7

MFRC522 rfid(SS_PIN, RST_PIN);

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Student details
String studentName = "Sneha";
String studentID = "101";

// Registered RFID card UID
String registeredUID = "A1 B2 C3 D4";

bool attendanceMarked = false;

void setup() {

  Serial.begin(9600);

  SPI.begin();

  rfid.PCD_Init();

  lcd.init();
  lcd.backlight();

  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print("SMART ATTENDANCE");

  lcd.setCursor(0, 1);
  lcd.print("Scan Your ID");

  delay(2000);

  lcd.clear();
}

void loop() {

  // Check person presence
  int presence = digitalRead(PIR_PIN);

  if (presence == HIGH) {

    lcd.setCursor(0, 0);
    lcd.print("Person Detected ");

    lcd.setCursor(0, 1);
    lcd.print("Scan ID Card    ");

  } else {

    lcd.setCursor(0, 0);
    lcd.print("No Person       ");

    lcd.setCursor(0, 1);
    lcd.print("Waiting...      ");
  }

  // Check RFID card
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  String scannedUID = "";

  for (byte i = 0; i < rfid.uid.size; i++) {

    if (rfid.uid.uidByte[i] < 0x10)
      scannedUID += "0";

    scannedUID += String(rfid.uid.uidByte[i], HEX);

    if (i < rfid.uid.size - 1)
      scannedUID += " ";
  }

  scannedUID.toUpperCase();

  Serial.print("Scanned UID: ");
  Serial.println(scannedUID);

  lcd.clear();

  // Check presence
  if (presence == LOW) {

    lcd.setCursor(0, 0);
    lcd.print("No Person");

    lcd.setCursor(0, 1);
    lcd.print("Attendance Denied");

    beep();

    delay(2500);

    lcd.clear();

    rfid.PICC_HaltA();

    return;
  }

  // Check registered student
  if (scannedUID == registeredUID) {

    if (attendanceMarked == true) {

      lcd.setCursor(0, 0);
      lcd.print("Already Marked");

      lcd.setCursor(0, 1);
      lcd.print("Try Tomorrow");

      beep();

    } else {

      attendanceMarked = true;

      lcd.setCursor(0, 0);
      lcd.print("Attendance");

      lcd.setCursor(0, 1);
      lcd.print("PRESENT");

      digitalWrite(LED_PIN, HIGH);

      tone(BUZZER_PIN, 1000, 300);

      Serial.println("Attendance Marked");
      Serial.println("Student: Sneha");
      Serial.println("ID: 101");
      Serial.println("Status: PRESENT");

    }

  } else {

    // Proxy / unregistered card
    lcd.setCursor(0, 0);
    lcd.print("PROXY DETECTED");

    lcd.setCursor(0, 1);
    lcd.print("Access Denied");

    digitalWrite(LED_PIN, LOW);

    beep();

    Serial.println("Proxy Attempt Detected");

  }

  delay(3000);

  lcd.clear();

  digitalWrite(LED_PIN, LOW);

  rfid.PICC_HaltA();
}

void beep() {

  tone(BUZZER_PIN, 500, 500);

  delay(500);

}