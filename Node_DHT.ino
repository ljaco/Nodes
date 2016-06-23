///////////////////////////////////////////////////////////////////////////////////
// 27.08.2015
// DHT Node

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
//  INT1   ~D3:
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

#define ADDRESS 104

#include <NodeProtocol.h>
#include <Data.h>
#include <Node.h>
#include <LowPower.h>
#include <DigitalOutput.h>
#include <DHT.h>

DigitalOutput led9;

//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

#define DHTPIN A1

DHT dht(DHTPIN, DHTTYPE);

Node node(ADDRESS, A0);

void setup()
{
  led9.setPin(9);

  led9.on();
  if (!node.init())
    while (1)
    {};
  led9.off();

  dht.begin();
  delay(500);
  node.hello();
}

void loop()
{
  node.sleep();
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
  delay(1);
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();
  if (!isnan(hum) && !isnan(temp))
  {
    int temperature = int(temp * 10);
    int humidity = int(hum * 10);
    node.send(TEMP_HUM, 0, node.twoToOne(temperature, humidity));
  }
  node.lowVoltage();

  led9.on();
  delay(4);
  led9.off();
}
