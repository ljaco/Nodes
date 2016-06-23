///////////////////////////////////////////////////////////////////////////////////
// 27.08.2015
// IR receiver Node

//      Pinmap:
//          A0: voltage divider to measure Vin
//          A1: DHT
//          A2:
//          A3:
//  SDA     A4:
//  SCL     A5:
//          A6: not connected
//          A7: not connected
//  RX      D0: FTDI TX for programming only
//  TX      D1: FTDI RX for programming only
//  INT0    D2: DIO0 RFM69HW
//  INT1   ~D3: IR receiver
//  (5V)    D4:
//         ~D5:
//  (SSR)  ~D6:
//  (SSR)   D7:
//          D8: SS SPI Flash
//         ~D9: LED9 or LED0
//  SS    ~D10: NSS RFM69HW
//  MOSI  ~D11: MOSI RFM69HW
//  MISO   D12: MISO RFM69HW
//  SCK    D13: SCK RFM69HW

#define ADDRESS 103

#include <NodeProtocol.h>
#include <Data.h>
#include <Node.h>
#include <LowPower.h>
#include <DigitalOutput.h>
#include <IRremote.h>
#include <IRCodes.h>
#include <DHT.h>

//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTPIN A1

decode_results results;
long int IRCode = 0;
const int IRreceiverPin = 3;

volatile bool gotInterrupt = false;

DigitalOutput led9;
DHT dht(DHTPIN, DHTTYPE);
IRrecv irReceiver(IRreceiverPin);
Node node(ADDRESS, A0);

void wakeUp()
{
  gotInterrupt = true;
}

void setup()
{
  irReceiver.enableIRIn();
  pinMode(IRreceiverPin, INPUT);
  led9.setPin(9);

  led9.on();
  if (!node.init())
    while (1);
  led9.off();

  dht.begin();

  node.hello();
  //sendTempHum();
}

void loop()
{
  node.sleep();
  attachInterrupt(1, wakeUp, LOW);
  //LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);

  // wakeup
  if (gotInterrupt)
  {
    gotInterrupt = false;
    detachInterrupt(1);
    getIR();
  }
  else
    sendTempHum();
  node.lowVoltage();
  node.sleep();
  delay(5);
  attachInterrupt(1, wakeUp, LOW);
}

void getIR(void)
{
  unsigned long nextSleepTime = millis() + 200;
  while (millis() < nextSleepTime)
  {
    if (irReceiver.decode(&results))
    {
      IRCode = results.value;
      byte code = determineIR(IRCode);
      if (code != INVALID_CODE)
        node.send(IRCODE_RECEIVED, 0, code);
      irReceiver.resume();
      return;  // right?
    }
  }
}

byte determineIR(long int IRCode)
{
  switch (IRCode)
  {
    case 2011254980:
      return UP;
      break;

    case 2011246788:
      return DOWN;
      break;

    case 2011271364:
      return LEFT;
      break;

    case 2011259076:
      return RIGHT;
      break;

    case 2011249348:
      return ENTER;
      break;

    case 2011283652:
      return MENU;
      break;

    case 2011298500:
      return PLAY;
      break;


    case 1090453759:
      return POWER;
      break;

    case 1090486399:
      return MUTE;
      break;

    case 1090470079:
      return DISP;
      break;

    case 1090461919:
      return CH_UP;
      break;

    case 1090474159:
      return CH_DOWN;
      break;

    case 1090478239:
      return P_DOWN;
      break;

    case 1090457839:
      return P_UP;
      break;

    case 1090510879:
      return SEL;
      break;

    case 1090490479:
      return SEEK;
      break;

    case 1090506799:
      return BAND;
      break;

    case 1090465999:
      return ONE;
      break;

    case 1090498639:
      return TWO;
      break;

    case 1090482319:
      return THREE;
      break;

    case 1090514959:
      return FOUR;
      break;

    case 1090455799:
      return FIVE;
      break;

    case 1090488439:
      return BANK;
      break;

    default:
      return INVALID_CODE;
      break;
  }
}

void sendTempHum(void)
{
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();
  if (!isnan(hum) && !isnan(temp))
  {
    int humidity = int(hum * 10);
    int temperature = int(temp * 10);
    node.send(TEMP_HUM, 0, node.twoToOne(temperature, humidity));
  }
  //led9.on();
  //delay(4);
  //led9.off();
}



