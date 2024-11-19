#include <Servo.h>
#include <LiquidCrystal.h>

// Pin assignments
const int pirTrigPin = 7;         // Ultrasonic sensor trigger pin
const int pirEchoPin = 8;         // Ultrasonic sensor echo pin
const int servoPin = 9;           // Servo motor connected to pin 9
const int irPin = A0;             // IR sensor connected to analog pin A0

Servo servoMotor;
LiquidCrystal lcd(12, 11, 5, 6, 10, 13); // LCD pins: RS, E, D4, D5, D6, D7

// Variables
const int distanceThreshold = 30;    // Distance threshold in cm for detecting person near bin
const int emptyIRReading = 800;      // Approximate IR reading when bin is empty (adjust as needed)
const int fullIRReading = 300;       // Approximate IR reading when bin is full (adjust as needed)
int garbageFilled = 0;
String password = "----";            // Initialize password display

void setup() {
  pinMode(pirTrigPin, OUTPUT);
  pinMode(pirEchoPin, INPUT);
  pinMode(irPin, INPUT);

  servoMotor.attach(servoPin);
  lcd.begin(16, 2);               // Initialize 16x2 LCD
  lcd.print("Welcome!");
  delay(2000);                    // Show the welcome message for 2 seconds
  lcd.clear();

  servoMotor.write(0);            // Start with lid closed
  randomSeed(analogRead(A1));     // Seed random generator for password
}

void loop() {
  // Check if a person is detected nearby with the ultrasonic sensor
  long duration, distance;
  digitalWrite(pirTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pirTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pirTrigPin, LOW);
  
  duration = pulseIn(pirEchoPin, HIGH);
  distance = duration * 0.034 / 2;  // Convert duration to cm
  
  // If person is within the threshold, open the lid and generate a password
  if (distance > 0 && distance < distanceThreshold) {
    // Generate a random 4-digit password
    password = String(random(1000, 10000));  // Random number between 1000 and 9999
    
    // Display password on the first line of the LCD
    lcd.setCursor(0, 0);
    lcd.print("Password: " + password + "    "); // Clear extra chars with spaces

    servoMotor.write(90);         // Open lid
    delay(5000);                 // Keep lid open for 1 minute
    servoMotor.write(0);          // Close lid
  }

  // Measure the bin fill level using the IR sensor
  int irReading = analogRead(irPin);

  // Map the IR reading to a percentage of bin fullness
  if (irReading <= emptyIRReading && irReading >= fullIRReading) {
    garbageFilled = map(irReading, fullIRReading, emptyIRReading, 100, 0);
  } else if (irReading < fullIRReading) {
    garbageFilled = 100;          // Bin is full
  } else {
    garbageFilled = 0;            // Bin is empty
  }
  
  // Display the garbage fill level on the second line of the LCD
  lcd.setCursor(0, 1);
  lcd.print("Garbage: " + String(garbageFilled) + "%     "); // Clear extra chars with spaces

  delay(500);
}
