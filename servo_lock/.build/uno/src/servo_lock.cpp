#include <Arduino.h>
void setup();
void enableRFID();
void getRFIDTag();
boolean isValid();
void disableRFID();
boolean IsDoorOpen();
void waitToLock();
void Lock();
void flashLED(int pin);
void Unlock();
void clearCode();
void loop();
#line 1 "src/servo_lock.ino"
#include <Servo.h>
#include <EEPROM.h>

#define CODE_LEN 10
#define START_BYTE 0x0A
#define STOP_BYTE 0x0D
#define VALIDATE_TAG 1
#define VALIDATE_LENGTH  200
#define ITERATION_LENGTH 2

Servo myServo;

int lockedPos = 40;
int unlockedPos = 130;
int buttonPin = 2;
int doorSense = 6;
int rfidPin = 4;
int serv = 9;
int led = 13;
int ledG = 11;
int ledB = 7;
int buttonState = 0;
char tag[CODE_LEN];
char andrew[CODE_LEN] = {'3', 'D', '0', '0', '7', '3', 'A', 'E', '0', '9'};
char troy[CODE_LEN] = {'3', 'D', '0', '0', '1', 'D', '6', '1', 'F', 'D'};


void setup()
{
  myServo.attach(serv);
  Serial.begin(2400);
  pinMode(rfidPin, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(doorSense, INPUT);
  digitalWrite(doorSense, HIGH);
  digitalWrite(ledB, HIGH);
  digitalWrite(ledG, LOW);
  digitalWrite(led, LOW);
  Unlock();
}


void enableRFID()
{
  digitalWrite(rfidPin, LOW);
}


void getRFIDTag()
{
  int val;
  int bytesread;
  if(Serial.available() > 0)
  {          // if data available from reader
    if((val = Serial.read()) == 10)
	 {   // check for header
      bytesread = 0;
      while(bytesread<10)
		{              // read 10 digit code
        if( Serial.available() > 0)
		  {
          val = Serial.read();
          if((val == 10)||(val == 13))
			 { // if header or stop bytes before the 10 digit reading
            break;                       // stop reading
          }
          tag[bytesread] = val;         // add the digit
          bytesread++;                   // ready to read next digit
        }
      }

      if(bytesread == 10)
		{
        Serial.print("TAG code is: ");   // possibly a good TAG
        Serial.println(tag);
      }
      bytesread = 0;
    }
  }
}


boolean isValid()
{
  int t = 0;
  int a = 0;

  for(int x = 0; x < CODE_LEN; x++)
  {
    if(tag[x] == troy[x])
	 {
      t++;
    }
  }

  for(int y = 0; y < CODE_LEN; y++)
  {
    if(tag[y] == andrew[y])
	 {
      a++;
    }
  }

  if(t == 10)
  {
    Serial.println("TAG ID: Troy");
    return true;
  }

  if(a == 10)
  {
    Serial.println("TAG ID: Andrew");
    return true;
  }

  return false;
}


void disableRFID()
{
  digitalWrite(rfidPin, HIGH);
}


boolean IsDoorOpen()
{
  Serial.println(digitalRead(doorSense));
  if (digitalRead(doorSense) == 1) { return 1; }
  if (digitalRead(doorSense) == 0) { return 0; }
}


void waitToLock()
{
  BeginWaitingToLock:
    while (IsDoorOpen() == 1) {}
    delay(5000);

   if (IsDoorOpen() == 0)
	{
      Lock();
   }

   if (IsDoorOpen() == 1)
	{
     goto BeginWaitingToLock;
   }
}


void Lock()
{
  digitalWrite(ledG, LOW);
  flashLED(led);
  digitalWrite(led, HIGH);
  myServo.write(lockedPos);
  delay(5000);
  digitalWrite(led, LOW);
}


void flashLED(int pin)
{
  digitalWrite(pin, HIGH);
  delay(100);
  digitalWrite(pin, LOW);
  delay(100);
  digitalWrite(pin, HIGH);
  delay(100);
  digitalWrite(pin, LOW);
  delay(100);
  digitalWrite(pin, HIGH);
  delay(100);
  digitalWrite(pin, LOW);
}


void Unlock()
{
  digitalWrite(led, LOW);
  flashLED(ledG);
  digitalWrite(ledG, HIGH);
  myServo.write(unlockedPos);
  waitToLock();
}


void clearCode()
{
  for(int i=0; i<CODE_LEN; i++)
  {
    tag[i] = 0;
  }
}


void loop()
{
  enableRFID();
  getRFIDTag();
  boolean valid = isValid();
  if(valid)
  {
    valid = false;
    disableRFID();
    clearCode();
    Serial.flush();
    Unlock();
  }
}
