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
//         ~D5: Relais1
//  (SSR)  ~D6: Relais2
//  (SSR)   D7: Relais3
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

#define ADDRESS 4

DigitalOutput relais0;
DigitalOutput relais1;
DigitalOutput relais2;
DigitalOutput relais3;

Node node(ADDRESS);

void setup()
{
  relais0.setPin(4, false, true);
  relais1.setPin(5, false, true);
  relais2.setPin(6, false, true);
  relais3.setPin(7, false, true);

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

    case 1:
      if (relais1.set(data))
      {
        //node.send(RELAIS, 1, relais1.get());
        return OK;
      }
      else
        return DATA_ERROR;

    case 2:
      if (relais2.set(data))
      {
        //node.send(RELAIS, 2, relais2.get());
        return OK;
      }
      else
        return DATA_ERROR;

    case 3:
      if (relais3.set(data))
      {
        //node.send(RELAIS, 3, relais3.get());
        return OK;
      }
      else
        return DATA_ERROR;

    case ALL_NUM:
      if (relais0.set(data) && relais1.set(data) && relais2.set(data) && relais3.set(data))
      {
        node.send(RELAIS, 0, relais0.get());
        node.send(RELAIS, 1, relais1.get());
        node.send(RELAIS, 2, relais2.get());
        node.send(RELAIS, 3, relais3.get());
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

    case 1:
      node.send(RELAIS, 1, relais1.get());
      return OK;

    case 2:
      node.send(RELAIS, 2, relais2.get());
      return OK;

    case 3:
      node.send(RELAIS, 3, relais3.get());
      return OK;

    case ALL_NUM:
      node.send(RELAIS, 0, relais0.get());
      node.send(RELAIS, 1, relais1.get());
      node.send(RELAIS, 2, relais2.get());
      node.send(RELAIS, 3, relais3.get());
      return OK;

    default:
      return NUM_ERROR;
  }
}
