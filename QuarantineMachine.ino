#include <LedControl.h>          //Dot matrix

#include <Servo.h>               //Servo motor

#include <DHT.h>                 //Digital temp & humidity
#include <DHT_U.h>               
                                 
#include <LiquidCrystal_I2C.h>   //Tlcd

#include <Key.h>                 //Keypad
#include <Keypad.h>


  ///////////////////////////////////////////////////////////////////////////////////////////////////////////


  //Key matrix 설정
  const byte ROWS = 4;
  const byte COLS = 4;

  char keyMatrix[ROWS][COLS] =
   {{'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}};

  byte rowPins[ROWS] = {2, 3, 4, 5};
  byte colPins[COLS] = {6, 7, 8, 9};

  Keypad myKeypad = Keypad(makeKeymap(keyMatrix), rowPins, colPins, ROWS, COLS);


  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  
  //Dot matrix 설정
  LedControl myDm = LedControl(12, 11, 10, 1);
  
  byte human[] = {B00111100, 
                  B00111100, 
                  B00011000, 
                  B11111111,
                  B00011000, 
                  B00011000, 
                  B00100100, 
                  B01000010};

  byte pet[] = {B00000111, 
                B00000111, 
                B00000111, 
                B11111000,
                B11111000,
                B11011000, 
                B11011000, 
                B00000000};

  byte ghost[] = {B00011000, 
                  B00011000, 
                  B00011000, 
                  B00011000,
                  B00011000, 
                  B00000000, 
                  B00011000, 
                  B00011000};

  void Human()
  {
    for(int i = 0; i < 8; i++)
    {
      myDm.setRow(0, i, human[i]);
      delay(100);
    }
  }

  void Pet()
  {
    for(int i = 0; i < 8; i++)
    {
      myDm.setRow(0, i, pet[i]);
      delay(100);
    }
  }
  
  void Ghost()
  {
    for(int i = 0; i < 8; i++)
    {
      myDm.setRow(0, i, ghost[i]);
      delay(100);
    }
  }
///////////////////////////////////////////////////////////////////////////////////////////////////////

float Distance_cm(int a)
{
  float dcm = ((a/2.9)/2)*0.1;
  return dcm;
}

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////


  LiquidCrystal_I2C myTlcd(0x3F, 20, 4);
  
  DHT myDht(22, DHT11);

  Servo myServo;

  char pw[4] = {'1', '2', '3', '4'};
  int count = 0;
  int thru = 0;


  ///////////////////////////////////////////////////////////////////////////////////////////////////////////


void setup() {
  Serial.begin(9600);

  myTlcd.init();
  myTlcd.backlight();
  
  myDht.begin();

  myServo.attach(50);
  myServo.write(0);

  myDm.shutdown(0, false);
  myDm.setIntensity(0, 5);
  myDm.clearDisplay(0);

  pinMode(A0, INPUT); //IR

  pinMode(A1, INPUT); //Flame

  pinMode(A2, INPUT); //Gas

  pinMode(33, INPUT); //Echo
  pinMode(31, OUTPUT);  //Trigger
  
  pinMode(35, OUTPUT); //Piezo Buzzer
  
  delay(2000);
}

int location;


  ///////////////////////////////////////////////////////////////////////////////////////////////////////////


void loop() {
  float temp = myDht.readTemperature();
  int hum = myDht.readHumidity();

  myTlcd.setCursor(0, 0);
  myTlcd.print("T : ");
  myTlcd.print(temp);
  myTlcd.print("C");
  myTlcd.setCursor(13, 0);
  myTlcd.print("H : ");
  myTlcd.print(hum);
  myTlcd.print("%");

  //IR 거리 처리값
  float volts = analogRead(A0) * 0.0048828125;
  float distance = 65 * pow(volts, -1.10);

  //ultrasonic 처리값
  digitalWrite(31, LOW);
  delayMicroseconds(2);
  digitalWrite(31, HIGH);
  delayMicroseconds(10);
  digitalWrite(31, LOW);

  location = pulseIn(33, HIGH);
  float cm = Distance_cm(location);

  //Human, Pet, Ghost 인식
  if(distance < 30 && cm < 30)
  {
    Human();
  }

  else if(cm < 30)
  {
    Pet();
  }

  else if(distance < 30)
  {
    Ghost();
  } 

  else
  {
    myDm.clearDisplay(0);
  }

  //Flame & Gas
  //Flame 500 이하 또는 Gas 400 이상이면 경보

 // Serial.print(cm);
 // Serial.println("                 ");
 // Serial.println(analogRead(A2));
  if(analogRead(A1) <= 500 || analogRead(A2) >= 450)
  {
    if(analogRead(A1) <= 500)
    {
      myServo.write(179);
      digitalWrite(35, HIGH);
      myTlcd.setCursor(0, 2);
      myTlcd.print("Fire, fire, fire!!");
      myTlcd.print(analogRead(A1));
    }

    else if(analogRead(A2) >= 400)
    {
      myServo.write(179);
      digitalWrite(35, HIGH);
      myTlcd.setCursor(0, 3);
      myTlcd.print("Gas, gas, gas!!");
      myTlcd.print(analogRead(A2));
    }
  }

  else
  {
    myServo.write(0);
    myTlcd.clear();
    digitalWrite(35, LOW);
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  char key = myKeypad.getKey();
  if(key)
  {
    Serial.println(key);
   
    if(key == pw[count])
    {
     count++;
     thru++;
    }

    else if(key != pw[count])
    {
      count++;
    }

    if(count == 4)
    {
      if(thru == 4)
      {
       myServo.write(179);
       delay(5000);
       myServo.write(0);
      }
      else
      {
        myServo.write(0);
      }
      thru = 0;
      count = 0;
    }
  }
}
  
