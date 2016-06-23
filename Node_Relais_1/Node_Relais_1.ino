///////////////////////////////////////////////////////////////////////////////////
// 09.01.2015
// SSR and Relais Node

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
//  INT1   ~D3: 
//  (5V)    D4: Relais0
//         ~D5: 
//  (SSR)  ~D6: 
//  (SSR)   D7: 
//          D8: SS SPI Flash
//         ~D9:
//  SS    ~D10: NSS RFM69HW
//  MOSI  ~D11: MOSI RFM69HW
//  MISO   D12: MISO RFM69HW
//  SCK    D13: SCK RFM69HW

#include <NodeProtocol.h>
#include <Data.h>
#include <Node.h>
#include <DigitalOutput.h>
#include <DigitalInput.h>

#define ADDRESS 3

DigitalOutput relais0;

Node node(ADDRESS);

void setup()
{
  relais0.setPin(3, false, true);

  if (!node.init())
    while (1)
    {}

  node.addFunction(setRelais, SET_RELAIS);
  node.addFunction(getRelais, GET_RELAIS);

  node.hello();
}

void loop()
{
  node.check();
}

uint8_t setRelais(byte num, long data)
{
  switch (num)
  {
    case 0:
      if (relais0.set(data))
      {
        //node.send(RELAIS, 0, relais0.get());
        return OK;
      }
      else
        return DATA_ERROR;

    case ALL_NUM:
      if (relais0.set(data))
      {
        //node.send(RELAIS, 0, relais0.get());
        return OK;
      }
      else
        return DATA_ERROR;

    default:
      return NUM_ERROR;
  }
}

uint8_t getRelais(byte num, long data)
{
  switch (num)
  {
    case 0:
      node.send(RELAIS, 0, relais0.get());
      return OK;

    case ALL_NUM:
      node.send(RELAIS, 0, relais0.get());
      return OK;

    default:
      return NUM_ERROR;
  }
}

