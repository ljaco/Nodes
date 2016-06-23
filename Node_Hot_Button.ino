///////////////////////////////////////////////////////////////////////////////////
// 27.08.2015
// Hot Button with Rotary encoder Node

//      Pinmap:
//          A0: voltage divider to measure Vin
//          A1: Rotary Encoder CLK
//          A2: Rotary Encoder DT
//          A3: Rotary Encoder Button
//  SDA     A4:
//  SCL     A5:
//          A6: not connected
//          A7: not connected
//  RX      D0: FTDI TX for programming only
//  TX      D1: FTDI RX for programming only
//  INT0    D2: DIO0 RFM69HW
//  INT1   ~D3: interrupt wakeup  D3 -->|-- A3, D3 -->|-- A4, D3 -->|-- A5,
//  (5V)    D4: Red Button
//         ~D5: Silver Button
//  (SSR)  ~D6: Buzzer
//  (SSR)   D7:
//          D8: SS SPI Flash
//         ~D9: LED9 or LED0
//  SS    ~D10: NSS RFM69HW
//  MOSI  ~D11: MOSI RFM69HW
//  MISO   D12: MISO RFM69HW
//  SCK    D13: SCK RFM69HW

/*
  1-VCC (3.3v)
  2-GND
  3-RX
  4-TX
  5-DTR
  6-Vcharge (5v)


  status leds next to plug:
  red: charge
  green: power

*/

#include <NodeProtocol.h>
#include <Node.h>
#include <Data.h>
#include <LowPower.h>
#include <DigitalOutput.h>
#include <DigitalInput.h>
#include <Rotary.h>

#define ADDRESS 100

Node node(ADDRESS, A0);

DigitalOutput led9;
DigitalOutput buzzer;

DigitalInput red;
DigitalInput silver;
DigitalInput rotaryButton;

const unsigned int wakeUpPin = 3;
const unsigned int rotaryEncoderPin1 = A1;
const unsigned int rotaryEncoderPin2 = A2;
const unsigned int debounceTime = 5;

unsigned char rotaryResult = 0;

bool firstPressed = false;

unsigned long nextSleep = 0;
long timeUntilSleep = 3000;

Rotary rotary = Rotary(rotaryEncoderPin1, rotaryEncoderPin2);

void wakeUp()
{
  // interrupt handler
}

void setup()
{
  //Serial.begin(115200);
  red.setPin(4, INPUT_PULLUP, true, debounceTime);
  silver.setPin(5, INPUT_PULLUP, true, debounceTime);
  rotaryButton.setPin(A3, INPUT_PULLUP, true, debounceTime); // TODO: increase

  buzzer.setPin(6);

  led9.setPin(9);
  pinMode(wakeUpPin, INPUT_PULLUP);

  led9.on();
  if (!node.init())
    while (1)
    {}
  led9.off();

  node.send(HELLO);
}

void loop()
{
  node.sleep();
  digitalWrite(rotaryEncoderPin1, LOW);
  digitalWrite(rotaryEncoderPin2, LOW);
  firstPressed = false;
  led9Blink();
  //Serial.println("sleep");

  delay(5);
  attachInterrupt(1, wakeUp, CHANGE);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

  // wakeup
  //Serial.println("wakeup");
  detachInterrupt(1);

  newSleepTime();
  node.lowVoltage();
  digitalWrite(rotaryEncoderPin1, HIGH);
  digitalWrite(rotaryEncoderPin2, HIGH);
  while (nextSleep > millis())
  {
    checkInputs();
  }
}

void checkInputs(void)
{
  if (node.receive())
    newSleepTime();

  if (red.changed())
  {
    newSleepTime();
    //Serial.println("red changed");
    if (red.pressed())
    {
      led9Blink();
      //Serial.println("red on");
      node.send(BUTTON, 0, ON);
    }
    if (red.released())
    {
      //Serial.println("red off");
      node.send(BUTTON, 0, OFF);
    }
  }

  if (silver.changed())
  {
    newSleepTime();
    //Serial.println("silver changed");
    if (silver.pressed())
    {
      led9Blink();
      //Serial.println("silver on");
      node.send(BUTTON, 1, ON);
    }
    if (silver.released())
    {
      //Serial.println("silver off");
      node.send(BUTTON, 1, OFF);
    }
  }

  if (rotaryButton.changed())
  {
    newSleepTime();
    //Serial.println("rotary changed");
    if (rotaryButton.pressed())
    {
      led9Blink();
      //Serial.println("button on");
      node.send(BUTTON, 2, ON);
    }
    if (rotaryButton.released())
    {
      //Serial.println("button off");
      node.send(BUTTON, 2, OFF);
    }
  }

  rotaryResult = rotary.process();
  if (rotaryResult)
  {
    if (rotaryResult == DIR_CW)
    {
      newSleepTime();
      led9Blink();
      //Serial.println("l");
      if (node.send(CONTACT, 3, OFF))
        beep();
    }
    else
    {
      newSleepTime();
      led9Blink();
      //Serial.println("r");
      if (node.send(CONTACT, 3, ON))
        beep();
    }
  }
}

void newSleepTime(void)
{
  nextSleep = millis() + timeUntilSleep;
}

void led9Blink(void)
{
  led9.on();
  delay(1);
  led9.off();
}

void beep (void)
{
  buzzer.on();
  delay(2);
  buzzer.off();
}

void beep (int time)
{
  buzzer.on();
  delay(time);
  buzzer.off();
}

