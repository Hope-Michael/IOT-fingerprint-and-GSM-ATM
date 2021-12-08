#include <Adafruit_Fingerprint.h>

#include <SoftwareSerial.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#include <Servo.h>

#define Password_Length 5



LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo myservo;

int pos = 0;

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)


SoftwareSerial mySerial(2, 3);

SoftwareSerial gsmSerial(14, 15);

#else

#define mySerial Serial1

#endif

int user;
int scanned = 1;
int sentOTP = 0;
int sms_sent = 0;
int amountEntered = 0;

int signalPin = 13;

char Data[Password_Length] = "";
char OTP[Password_Length] = "";
byte data_count = 0, master_count = 0;
bool Pass_is_good;
char customKey;

String randNumber;

String inputString;
long inputInt;


char number1[15] = "+254798482358"; // phone number of registered user #1
char number2[15] = "+254742357997"; // phone number of registered user #2
char number3[15] = "";// phone number of registered user #3
char number4[15] = "";// phone number of registered user #4
char number5[15] = "";// phone number of registered user #5


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);


const byte ROWS = 4;
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {12, 11, 10, 9};
byte colPins[COLS] = {8, 7, 6};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);



void setup()
{
  myservo.write(0);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);

  lcd.print("INITIALIZING.....");

  Serial.begin(9600);
  while (!Serial);
  delay(15000);
  Serial.println("\n\nAdafruit finger detect test");

  gsmSerial.begin(9600);

  delay (2000);

  finger.begin(57600);

  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
    Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }

  inputString.reserve(10);

  myservo.attach(5);

  lcd.clear();

  lcd.setCursor(5, 0);

  lcd.print("DONE");

  delay(1000);

}

void loop()
{

  if (amountEntered ==  0)

  {

    withdrawinput();

  }

  if (scanned == 0)
  {

    lcd.setCursor(2, 0);

    lcd.print("Place Finger");

    lcd.setCursor(5, 1);

    lcd.print("To scan");

    getFingerprintID();

    randNumber = random(1000, 9999);

    randNumber.toCharArray(OTP, Password_Length);

  }


  if (user == 1)

  {

    String text = "Your OTP is " + String(randNumber);

    int text_len = text.length() + 1;

    const char msg[text_len];

    text.toCharArray(msg, text_len);


    send_text(number1, msg);

    user = 0;


  }

  if (user == 2)

  {

    String text = "Your OTP is " + String(randNumber);

    int text_len = text.length() + 1;

    const char msg[text_len];

    text.toCharArray(msg, text_len);


    send_text(number2, msg);

    user = 0;


  }

  if (user == 3)

  {

    String text = "Your OTP is " + String(randNumber);

    int text_len = text.length() + 1;

    const char msg[text_len];

    text.toCharArray(msg, text_len);


    send_text(number3, msg);

    user = 0;


  }


  if (user == 4)

  {

    String text = "Your OTP is " + String(randNumber);

    int text_len = text.length() + 1;

    const char msg[text_len];

    text.toCharArray(msg, text_len);


    send_text(number4, msg);

    user = 0;


  }


  if (user == 5)

  {

    String text = "Your OTP is " + String(randNumber);

    int text_len = text.length() + 1;

    const char msg[text_len];

    text.toCharArray(msg, text_len);


    send_text(number5, msg);

    user = 0;


  }


  if (sms_sent == 1)
  {

    OTPverfication();

  }

  delay(50);

  Serial.print("Amount:" );
  Serial.println(inputInt);

  Serial.print("user no: ");
  Serial.println(user);

  Serial.print("OTP: ");
  Serial.println(randNumber);
}

void send_text(const char *num, const char *msg)
{
  gsmSerial.println("AT+CMGF=1");
  updateSerial();
  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(num);
  gsmSerial.println("\"\r");
  updateSerial();
  gsmSerial.print(msg);
  updateSerial();
  gsmSerial.write(0x1a);

  sms_sent = 1;

}

void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    gsmSerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while (gsmSerial.available())
  {
    Serial.write(gsmSerial.read());//Forward what Software Serial received to Serial Port
  }
}

void withdrawinput()

{

  lcd.setCursor(0, 0);
  lcd.print("Enter Amount: ");

  customKey = customKeypad.getKey();

  if (customKey) {

    Data[data_count] = customKey;
    lcd.setCursor(data_count, 1);
    lcd.print(Data[data_count]);
    data_count++;

    if (customKey >= '0' && customKey <= '9') {
      inputString += customKey;
    } else if (customKey == '#') {
      if (inputString.length() > 0) {
        inputInt = inputString.toInt();

        clearData();

        Data[data_count] = "";

        Data[Password_Length] = "";

        data_count = 0;

        amountEntered = 1;

        scanned = 0;

        lcd.clear();


      }
    } else if (customKey == '*') {
      inputString = "";                 // clear input
      clearData();
      lcd.clear();
    }
  }

}

void OTPverfication()
{
  lcd.setCursor(0, 0);
  lcd.print("Enter OTP:");

  customKey = customKeypad.getKey();
  if (customKey) {
    Data[data_count] = customKey;
    lcd.setCursor(data_count, 1);
    lcd.print(Data[data_count]);
    data_count++;
  }

  if (data_count == Password_Length - 1) {
    lcd.clear();

    if (!strcmp(Data, OTP)) {
      lcd.setCursor(0, 0);
      lcd.print("OTP Match");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Withdrawing...");
      lcd.setCursor(0, 1);
      lcd.print("Ksh: ");
      lcd.setCursor(5, 1);
      lcd.print(inputInt);

      myservo.write(180);
      delay(5000);
      myservo.write(0);

      randNumber = "";
      OTP[Password_Length] = "";
      inputString = "";
      amountEntered = 0;
      sms_sent = 0;
      inputInt = 0;

    }

    else

    {

      lcd.print("Incorrect");
      delay(1000);

    }

    lcd.clear();
    clearData();

  }
}

void clearData() {
  while (data_count != 0) {
    Data[data_count--] = 0;
  }
  return;
}
