#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// הגדרות פינים
#define FLAMEPIN A0        
#define BUZZER 8           
#define RED_PIN 5          
#define GREEN_PIN 6        
#define BLUE_PIN 7         
#define TRIG_PIN 9         
#define ECHO_PIN 10        

// מצבי מחוות
enum GestureState { IDLE, HAND_DETECTED };
GestureState gestureState = IDLE;

// משתנים למצב המערכת
bool systemActive = true;
unsigned long lastGestureTime = 0;

// מסך LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);

  pinMode(FLAMEPIN, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Fire Detection");
  lcd.setCursor(0, 1);
  lcd.print("System Ready");
  delay(2000);
  lcd.clear();
}

void loop() {
  handleGesture();
  
  if (systemActive) {
    handleFlameSensor();
    delay(100);
  }
}

void handleGesture() {
  float distance = measureDistance();
  unsigned long currentTime = millis();

  if (currentTime - lastGestureTime < 100) {
    return; // מחכים לפני זיהוי מחווה חדשה
  }

  if (gestureState == IDLE && distance > 5 && distance < 30) {
    gestureState = HAND_DETECTED;
    lastGestureTime = currentTime;
    Serial.println("Hand detected!");
  }
  
  if (gestureState == HAND_DETECTED) {
    toggleSystemState();
    gestureState = IDLE;
    lastGestureTime = currentTime;
  }
}

float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  return (duration * 0.0343) / 2;
}

void toggleSystemState() {
  systemActive = !systemActive;
  Serial.println(systemActive ? "System Resumed" : "System OFF");
  lcd.setCursor(0, 0);
  lcd.print(systemActive ? "System Active   " : "System OFF      ");
  lcd.setCursor(0, 1);
  lcd.print(systemActive ? "All Systems Go  " : "Wave to restart ");
}

void handleFlameSensor() {
  int flame = analogRead(FLAMEPIN);
  Serial.print("Flame: ");
  Serial.println(flame);

  lcd.setCursor(0, 0);
  lcd.print("Flame Level:     ");
  lcd.setCursor(13, 0);
  lcd.print(flame);

  if (flame > 5) {
    setRGB(255, 0, 0);
    lcd.setCursor(0, 1);
    lcd.print("ALERT! FIRE!   ");
    Serial.println("FIRE DETECTED");
    activateBuzzer();
  } else if (flame > 2) {
    setRGB(255, 165, 0);
    lcd.setCursor(0, 1);
    lcd.print("Warning!       ");
    Serial.println("Warning: Flame detected");
    deactivateBuzzer();
  } else {
    setRGB(0, 255, 0);
    lcd.setCursor(0, 1);
    lcd.print("All Good       ");
    Serial.println("System Normal");
    deactivateBuzzer();
  }
}

void setRGB(int red, int green, int blue) {
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}

void activateBuzzer() {
  digitalWrite(BUZZER, HIGH);
}

void deactivateBuzzer() {
  digitalWrite(BUZZER, LOW);
}
