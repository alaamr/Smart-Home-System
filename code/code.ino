#define BLYNK_TEMPLATE_ID "TMPL2VFS4tNI9"
#define BLYNK_TEMPLATE_NAME "Smart Home"
#define BLYNK_AUTH_TOKEN "0WstJ1JR7fzFs_GGz0-uIGZx9fAahoD5"

#include <HardwareSerial.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <DHT.h>
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#define DHTTYPE DHT22
Servo myservo;

LiquidCrystal_I2C lcd(0x27, 16, 2);

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "realme 8 5G";
char pass[] = "joemo2003";
const char* correct_password = "Betreek"; // Set the correct password here
bool accessGranted = false; // Flag to check access status

const int DHTPIN = 32; // Connect DHT sensor to GPIO 35
const int ledPin1 = 33; // Connect LED 1 to GPIO 23 (ensure this pin supports PWM on your ESP32 model)
const int ledPin2 = 25; // Connect LED 2 to GPIO 22
const int ledPin3 = 2; // Connect LED 3 to GPIO 21
const int gasSensorPin = 34; // Gas sensor pin (assuming analog output)
const int buzzerPin = 23; // Buzzer connected to GPIO 26
const int motorPin1 = 16; // Pin connected to input 1 of the H-bridge
const int motorPin2 = 17; // Pin connected to input 2 of the H-bridge
const int soilMoisturePin = 35; // GPIO pin connected to the sensor output
const int servopin =19;
const int IRSensorPin = 27;  // IR sensor connected to GPIO 26
const int ledPin4 = 26;      // Additional LED connected to GPIO 27
const int lightSensorPin =18 ;

DHT dht(DHTPIN, DHTTYPE);

// Servo servo; // Example if you plan to control a servo
float temperature = 0.0;
float temp =0.0;
int gasValue =0;
int soilMoistureValue = 0; // Variable to store the sensor value
int incorrectPasswordAttempts = 0; 

void setup() {
    Serial.begin(9600);
    Blynk.begin(auth, ssid, pass);
    pinMode(ledPin1, OUTPUT);
    pinMode(ledPin2, OUTPUT);
    pinMode(ledPin3, OUTPUT);
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    pinMode(soilMoisturePin, INPUT);
    pinMode(lightSensorPin, INPUT);
    pinMode(ledPin4, OUTPUT);  // Set the new LED as an output
    pinMode(IRSensorPin, INPUT);  // Set the IR sensor pin as an input
    digitalWrite(buzzerPin, LOW);
    dht.begin();
    myservo.attach(servopin);
    myservo.write(90);
    lcd.init();
    lcd.backlight();
    lcd.print("System Start");
}

void resetDevices() {
    analogWrite(ledPin1, 0);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, LOW);
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
}

void defult_display(){
  lcd.clear();
  lcd.print("temp: ");
  lcd.print(temp);
  lcd.print(" C ");
  lcd.setCursor(0,1);
  lcd.print("gas: ");
  lcd.print(gasValue);
}

BLYNK_WRITE(V3) { // Function to check password
    String password = param.asStr();
    if (password == correct_password) {
        accessGranted = true;
        incorrectPasswordAttempts = 0;
        lcd.clear(); // Reset the counter on correct password
        lcd.print("Access Granted");
        delay(1000);
        defult_display();
        WidgetLED LED1(V13);
        LED1.on();
        delay(1000);
        LED1.off();
        digitalWrite(buzzerPin, LOW);
        myservo.write(180);
        delay(3000);
        myservo.write(90);
    } else {
        accessGranted = false;
        incorrectPasswordAttempts++;
        lcd.clear(); // Increment on incorrect password
        lcd.print("Access Denied");
        delay(1000);
        defult_display();
        resetDevices();

        if (incorrectPasswordAttempts >= 3) {
            digitalWrite(buzzerPin, HIGH);
            lcd.clear();
            lcd.print("wrong password 3 ");
            delay(1000);
            defult_display();  // Activate the buzzer
            incorrectPasswordAttempts = 0;  // Reset the counter after action is taken
        }
    }
}


BLYNK_WRITE(V5) { // Function to control LED brightness
    if (accessGranted) {
        int choice = param.asInt();
        lcd.clear();
        lcd.print("Room 1 Brightness");
        switch (choice) {
            case 2: // 100% brightness
                analogWrite(ledPin1, 255);
                lcd.setCursor(0,1);
                lcd.print("100%");
                break;
            case 1: // 50% brightness
                analogWrite(ledPin1, 128);
                lcd.setCursor(0,1);
                lcd.print("50%");
                break;
            case 0: // LED Off
                analogWrite(ledPin1, 0);
                lcd.setCursor(0,1);
                lcd.print("OFF");
                break;
        }
        delay(1000);
        defult_display();
    } else {
        analogWrite(ledPin1, 0);
    }
}

BLYNK_WRITE(V7) { // Control for LED 2
    if (accessGranted) {
        digitalWrite(ledPin2, param.asInt());
        lcd.clear();
        lcd.print("Room 2 light  :");
        lcd.setCursor(0,1);
        lcd.print(param.asInt() ? "ON" : "OFF");
        delay(1000);
        defult_display();
    } else {
        digitalWrite(ledPin2, LOW);
    }
}

BLYNK_WRITE(V11) { // Soil Moisture sensor
     if (accessGranted) {
        soilMoistureValue = analogRead(soilMoisturePin); // Read the analog value from the sensor
        soilMoistureValue = map(soilMoistureValue, 0, 1024, 0, 100);
        soilMoistureValue = (soilMoistureValue - 100) * -1;
        Blynk.virtualWrite(V11, soilMoistureValue);
    }
}

BLYNK_WRITE(V8) { // Control for LED 3
    if (accessGranted) {
        digitalWrite(ledPin3, param.asInt());
        lcd.clear();
        lcd.print("Room 3 light ");
        lcd.setCursor(0,1);
        lcd.print(param.asInt() ? "ON" : "OFF");
        delay(1000);
        defult_display();
    } else {
        digitalWrite(ledPin3, LOW);
    }
}

BLYNK_WRITE(V9) { // Control for motor
    if (accessGranted) {
        digitalWrite(motorPin1, param.asInt());
        digitalWrite(motorPin2, LOW);
        lcd.clear();
        lcd.print("Fan state :");
        lcd.setCursor(0,1);
        lcd.print(param.asInt() ? "Running" : "Stopped");
        delay(1000);
        defult_display();
    } else {
        digitalWrite(motorPin1, LOW);
        digitalWrite(motorPin2, LOW);
    }
}

void checkGasLevels() { // Function to handle gas sensor readings and alerts
    gasValue = analogRead(gasSensorPin);
    gasValue = map(gasValue, 0, 4095, 0, 100);
    Serial.println("gasValue : ");
    Serial.println(gasValue);
    if (gasValue > 80) {
        digitalWrite(buzzerPin, HIGH);
        Blynk.logEvent("hare2aaaa");
        lcd.clear();
        lcd.print("haree2aaaa");

        WidgetLED LED(V12);
        LED.on();
        // Blynk.notify("Warning: High gas level detected!");
    } else {
        digitalWrite(buzzerPin, LOW);
        WidgetLED LED(V12);
        LED.off();
    }
    Blynk.virtualWrite(V10, gasValue);
}

void sendTemperatureToBlynk() {
  if (accessGranted) {
    temperature= dht.readTemperature(); // Read temperature in Celsius
    // Blynk.virtualWrite(V11, soilMoistureValue);
    Blynk.virtualWrite(V6, temperature); // Send temperature to Blynk app
  }
}

void loop() {
    Blynk.run();
    
    temp = dht.readTemperature();
    Blynk.virtualWrite(V6, temp);
    soilMoistureValue = analogRead(soilMoisturePin); // Read the analog value from the sensor
    soilMoistureValue = map(soilMoistureValue, 0, 1024, 0, 300);
    //soilMoistureValue = (soilMoistureValue - 100) * -1;
    Blynk.virtualWrite(V11, soilMoistureValue);
    defult_display();
    int lightValue = digitalRead(lightSensorPin);
    int IRValue = digitalRead(IRSensorPin);

    if (lightValue == HIGH) {
    if (IRValue == LOW) {
      digitalWrite(ledPin4, HIGH);
      lcd.clear();
      lcd.print("Door Led On");
      delay(1000);
      defult_display();
  
  } else {
    digitalWrite(ledPin4, LOW);
    lcd.clear();
    lcd.print("Door Led Off");
    delay(1000);
    defult_display();
  
  }
  }else{
    digitalWrite(ledPin4, LOW);
  }

    // sendTemperatureToBlynk();
    checkGasLevels();
}