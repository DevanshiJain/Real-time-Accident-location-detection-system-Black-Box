//accelerometer, gps and gsm
#include <TinyGPS.h>
#include <SoftwareSerial.h>
#include<LiquidCrystal.h>

LiquidCrystal lcd(13,12,11,10,9,8);   
TinyGPS gps;  //Creates a new instance of the TinyGPS object
SoftwareSerial mySerial(9, A5);

int Flame = 7;
int Rain = A4;

int xsample=0;
int ysample=0;
int zsample=0;

#define samples 10
#define minVal -50   // Min and max value of acc coordinates to detect accident
#define MaxVal 50 

void setup()
{
  for(int i=0;i<samples;i++)                    //run loop for 10 values(instants)
  {
    xsample+=analogRead(A1);                     //xsample = xsamples + the value from the specified analog pin
    ysample+=analogRead(A2);
    zsample+=analogRead(A3);
  }
  xsample/=samples;                            //Taking average of all sample values
  ysample/=samples;
  zsample/=samples;
  
  Serial.begin(9600);  
  mySerial.begin(9600);   // Setting the baud rate of GSM Module  
  Serial.println("Testing GPS");
  pinMode(Flame,INPUT_PULLUP);
  lcd.begin(20,4);
  
}


void loop()
{
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial.available())
    {
      char c = Serial.read();
      //Serial.print(c);
      if (gps.encode(c)) 
        newData = true;  
    }
  }
    
  int value1=analogRead(A1);                  //Read x co-ordinate of Accelerometer & store it in value1
  int value2=analogRead(A2);
  int value3=analogRead(A3);

  int xValue=xsample-value1;                 //xValue=xsample value read before - value 1 read above
  int yValue=ysample-value2;
  int zValue=zsample-value3;
  
  if(xValue < minVal || xValue > MaxVal  || yValue < minVal || yValue > MaxVal  || zValue < minVal || zValue > MaxVal)
  {
    SendMessage();
    display_lcd();

  }
  else{
    mySerial.println("no accident detected");
  }
}


 void SendMessage()
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(100);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+91xxxxxxxxxx\"\r"); // Replace x with mobile number
  delay(100);
  
  float flat, flon;
  unsigned long age;
  gps.f_get_position(&flat, &flon, &age);
  mySerial.print("Latitude = ");
  mySerial.println(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
  delay(100);
  mySerial.print(" Longitude = ");
  mySerial.println(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
  delay(100);
  
}

void display_lcd()
{
  lcd.setCursor(0,0);lcd.print("Accident Detected ");
  if(digitalRead(Rain) == HIGH)
  {
    lcd.setCursor(0,1);lcd.print("Rain Detected ");
  }
  if(digitalRead(Rain) == LOW)
  {
    lcd.setCursor(0,1);lcd.print("Rain NOT Detected ");
  }
  if(digitalRead(Flame) == HIGH)
  {
    lcd.setCursor(0,2);lcd.print("Flame Detected ");
  }
  if(digitalRead(Flame) == LOW)
  {
    lcd.setCursor(0,2);lcd.print("Flame NOT Detected ");
  }
  
}
 void RecieveMessage()
{
  mySerial.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
  delay(1000);
 }
