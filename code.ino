#include <LiquidCrystal.h>
#include <Wire.h>
#include <MPU6050.h>


// Pin definitions for sensors and actuators
#define FLAME_SENSOR_PIN A1  // Pin connected to the flame sensor
#define GAS_SENSOR_PIN A0  // Pin connected to the analog output of the gas sensor
#define LED_PIN 12         // Pin for LED indicator
#define LED2 13            //pin for 2nd LED
#define BUZZER_PIN 8       // Pin for the buzzer
float prevx = 0;            //stores previous value of x
float prevy = 0;            //stores prev value of y
float prevz = 0;            //stores prev value of z


MPU6050 mpu;

void setup() {
  pinMode(GAS_SENSOR_PIN, INPUT);
  pinMode(FLAME_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(9600);
  Wire.begin();
  Serial.println("Initialize MPU6050");
  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)) {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    // delay(500);
  }
  mpu.setThreshold(3);

  // Check settings
  checkSettings();
}

void checkSettings() {
  Serial.println();
  Serial.print(" * Sleep Mode:        ");
  Serial.println(mpu.getSleepEnabled() ? "Enabled" : "Disabled");
  Serial.print(" * Clock Source:      ");
  switch (mpu.getClockSource()) {
    case MPU6050_CLOCK_KEEP_RESET:     Serial.println("Stops the clock and keeps the timing generator in reset"); break;
    case MPU6050_CLOCK_EXTERNAL_19MHZ: Serial.println("PLL with external 19.2MHz reference"); break;
    case MPU6050_CLOCK_EXTERNAL_32KHZ: Serial.println("PLL with external 32.768kHz reference"); break;
    case MPU6050_CLOCK_PLL_ZGYRO:      Serial.println("PLL with Z axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_YGYRO:      Serial.println("PLL with Y axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_XGYRO:      Serial.println("PLL with X axis gyroscope reference"); break;
    case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
  }
  Serial.print(" * Gyroscope:         ");
  switch (mpu.getScale()) {
    case MPU6050_SCALE_2000DPS:        Serial.println("2000 dps"); break;
    case MPU6050_SCALE_1000DPS:        Serial.println("1000 dps"); break;
    case MPU6050_SCALE_500DPS:         Serial.println("500 dps"); break;
    case MPU6050_SCALE_250DPS:         Serial.println("250 dps"); break;
  }
  Serial.print(" * Gyroscope offsets: ");
  Serial.print(mpu.getGyroOffsetX());
  Serial.print(" / ");
 Serial.print(mpu.getGyroOffsetY());
  Serial.print(" / ");
  Serial.println(mpu.getGyroOffsetZ());
  Serial.println();
}

void loop() {
  int gasSensorValue = analogRead(GAS_SENSOR_PIN);
  int flameSensorValue = analogRead(FLAME_SENSOR_PIN);
  bool check=false;
  // Check flame sensor reading against a threshold
  if (flameSensorValue < 400) {  // Adjust this threshold based on sensor sensitivity
    Serial.println("Flame detected!");
    check=true;
    digitalWrite(LED2, HIGH);
    digitalWrite(LED_PIN, HIGH);    // Turn on the LED
    tone(BUZZER_PIN, 1000);         // Activate the buzzer
  } 

  // Check gas sensor reading against a threshold
  if (gasSensorValue > 800) {  // Adjust this threshold based on sensor sensitivity
    Serial.println("Gas detected!");
    Serial.print("gassensorvalue: ");
    Serial.println(gasSensorValue);
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED2, HIGH);
    check=true;    // Turn on the LED
    tone(BUZZER_PIN, 1000);         // Activate the buzzer
  }
    Serial.print("gassensorvalue: ");
    Serial.println(gasSensorValue);

  Vector rawGyro = mpu.readRawGyro();
  Vector normGyro = mpu.readNormalizeGyro();
  Serial.print(" Xraw = ");
  Serial.print(rawGyro.XAxis);
  Serial.print(" Yraw = ");
  Serial.print(rawGyro.YAxis);
  Serial.print(" Zraw = ");
  Serial.println(rawGyro.ZAxis);
  float changex = rawGyro.XAxis;
  changex-=prevx;
  float changey = rawGyro.YAxis;
  changey-=prevy;
  float changez = rawGyro.ZAxis;
  changez-=prevz;

  if ((changex > 100 || changex < -100) || (changey > 200 || changey<-200) || (changez >100 || changez<-100) ) {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED2, HIGH);
    tone(BUZZER_PIN, 1000);  
    check=true;
    Serial.println("**EarthQuake**");
    delay(2000);
  }
 Serial.print(" changeX = ");
  Serial.print(changex);
  Serial.print(" changeY = ");
  Serial.print(changey);
  Serial.print(" changeZ = ");
  Serial.println(changez);

  Serial.print(" Xnorm = ");
  Serial.print(normGyro.XAxis);
  Serial.print(" Ynorm = ");
  Serial.print(normGyro.YAxis);
  Serial.print(" Znorm = ");
  Serial.println(normGyro.ZAxis);


  if(check==false){
    digitalWrite(LED_PIN, LOW);
    digitalWrite(LED2, LOW);     // Turn off the LED
   noTone(BUZZER_PIN); 
  }
  prevx=rawGyro.XAxis;
  prevy=rawGyro.YAxis;
  prevz=rawGyro.ZAxis;

  delay(1000);  // Adjust delay based on your needs
}



// #include <SoftwareSerial.h>

// SoftwareSerial SIM900A(10,11);

// void setup()
// {
//   SIM900A.begin(9600);   // Setting the baud rate of GSM Module  
//   Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
//   Serial.println ("SIM900A Ready");
//   delay(100);
//   Serial.println ("Type s to send message or r to receive message");
// }


// void loop()
// {
//   if (Serial.available()>0)
//    switch(Serial.read())
//   {
//     case 's':
//       SendMessage();
//       break;

//     case 'r':
//       RecieveMessage();
//       break;
//   }

//  if (SIM900A.available()>0)
//    Serial.write(SIM900A.read());
// }


//  void SendMessage()
// {
//   Serial.println ("Sending Message");
//   SIM900A.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
//   delay(1000);
//   Serial.println ("Set SMS Number");
//   SIM900A.println("AT+CMGS=\"+919149014514\"\r"); //Mobile phone number to send message
//   delay(1000);
//   Serial.println ("Set SMS Content");
//   SIM900A.println("Disaster Detected at place : ");// Messsage content
//   delay(100);
//   Serial.println ("Finish");
//   SIM900A.println((char)26);// ASCII code of CTRL+Z
//   delay(1000);
//   Serial.println ("Message has been sent ->SMS Selesai dikirim");
// }


//  void RecieveMessage()
// {
//   Serial.println ("SIM900A Membaca SMS");
//   delay (1000);
//   SIM900A.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
//   delay(1000);
//   Serial.write ("Unread Message done");
//  }
