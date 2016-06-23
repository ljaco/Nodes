///////////////////////////////////////////////////////////////////////////////////
// 29.07.2015
// Motion Node

//      Pinmap:
//          A0: voltage divider to measure Vin
//          A1:
//          A2:
//          A3:
//  SDA     A4:
//  SCL     A5:
//          A6: not connected
//          A7: not connected
//  RX      D0: FTDI TX for programming only
//  TX      D1: FTDI RX for programming only
//  INT0    D2: DIO0 RFM69HW
//  INT1   ~D3: motion0
//  (5V)    D4:
//         ~D5:
//  (SSR)  ~D6:
//  (SSR)   D7:
//          D8: SS SPI Flash
//         ~D9: LED9
//  SS    ~D10: NSS RFM69HW
//  MOSI  ~D11: MOSI RFM69HW
//  MISO   D12: MISO RFM69HW
//  SCK    D13: SCK RFM69HW

#include <NodeProtocol.h>
#include <Data.h>
#include <Node.h>
#include <LowPower.h>
#include <DigitalOutput.h>
#include <DigitalInput.h>

#define ADDRESS 102

DigitalInput motion0;
DigitalOutput led9;

Node node(ADDRESS, A0);

void wakeUp(void)
{
  // interrupt handler
}

void setup()
{
  motion0.setPin(3);
  led9.setPin(9);

  led9.on();
  if (!node.init())
    while (1)
    {};
  led9.off();

  node.hello();
}

void loop()
{
  node.sleep();
  attachInterrupt(1, wakeUp, CHANGE);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

  // wakeup
  detachInterrupt(1);
  node.send(MOTION, 0, motion0.get());
  //led9.on();
  //delay(4);
  //led9.off();
  node.lowVoltage();
}
