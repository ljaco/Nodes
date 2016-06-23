///////////////////////////////////////////////////////////////////////////////////
// 11.03.2016
// Node <--> Node-RED connection over serial

//      Pinmap:
//          A0:
//          A1:
//          A2:
//          A3:
//  SDA     A4:
//  SCL     A5:
//          A6:
//          A7:
//  RX      D0: FTDI TX for programming and RPi UART TX
//  TX      D1: FTDI RX for programming and RPi UART RX
//  INT0    D2: DIO0 RFM69HW
//  INT1   ~D3: (IR receiver)
//          D4: (Shock sensor)
//         ~D5: (Buzzer)
//         ~D6:
//          D7:
//          D8:
//         ~D9: LED9
//  SS    ~D10: NSS RFM69HW
//  MOSI  ~D11: MOSI RFM69HW
//  MISO   D12: MISO RFM69HW
//  SCK    D13: SCK RFM69HW

// ok added retx while sending
// ok changed letters: e -> c, f -> d, c -> e, d -> f
// ok changed report numbers
// ok added j,rssi,temperature; under r,3,0;

#include <NodeProtocol.h>
#include <Data.h>
#include <Node.h>
#include <DigitalOutput.h>
#include <avr/wdt.h>

#define ADDRESS 0

Node node(ADDRESS);

DigitalOutput led9;

#define NODES 20

unsigned int goodTx[NODES] = {0};
unsigned int badTx[NODES] = {0};

unsigned int rxHigh[NODES] = {0};
unsigned int rxLow[NODES] = {0};

unsigned int totalGoodTx = 0;
unsigned int totalBadTx = 0;
unsigned int totalTx = 0;
unsigned int totalRx = 0;

void setup()
{
  led9.setPin(9);
  Serial.begin(115200);
  Serial.setTimeout(5);
  if (!node.init())
    while (1)
    {};

  Serial.print(F("z,0;"));
}

void loop()
{
  serialToNode();
  nodeToSerial();
}

void nodeToSerial(void)
{
  if (node.packetAvailable())
  {
    byte type;
    byte num;
    long data;
    byte from;
    unsigned int volt;
    int temp;
    int rssi;
    if (node.receive(&type, &num, &data, &from, &volt, &temp, &rssi))
    {
      led9.on();
      ++totalRx;
      char serialBuffer[40];
      if (from >= 100)
      {
        rxHigh[from - 100]++;
        sprintf(serialBuffer, "a,%u,%u,%u,%ld,%u,%d,%d,%u,%u;", from, type, num, data, volt, temp, rssi, rxHigh[from - 100], totalRx);
      }
      else
      {
        rxLow[from]++;
        sprintf(serialBuffer, "a,%u,%u,%u,%ld,%u,%d,%d,%u,%u;", from, type, num, data, volt, temp, rssi, rxLow[from], totalRx);
      }
      Serial.print(serialBuffer);
      led9.off();
    }
  }
  else if (node.arrayAvailable())
  {
    return; // TODO
  }
}

void serialToNode(void)
{
  if (Serial.available() > 0)
  {
    led9.on();
    char inChar = Serial.read();
    if (inChar == 'a')
    { // a,to,type,num,data;
      byte to = Serial.parseInt();
      byte type = Serial.parseInt();
      byte num = Serial.parseInt();
      long data = Serial.parseInt();
      led9.off();
      if (Serial.read() == ';')
      {
        if (to == ADDRESS)
        {
          //nodeFunction(type, num, data);
          return;
        }
        else
        {
          ++totalTx;
          char serialBuffer[20];
          if (node.send(type, num, data, to))
          {
            ++totalGoodTx;
            if (to >= 100)
            {
              sprintf(serialBuffer, "c,%u,%u,%lu,%u;", to, totalGoodTx, node.retransmissions(), totalTx); // should not happen, node is sleeping
            }
            else
            {
              goodTx[to]++;
              sprintf(serialBuffer, "d,%u,%u,%u,%lu,%u;", to, goodTx[to], totalGoodTx, node.retransmissions(), totalTx);
            }
          }
          else
          {
            ++totalBadTx;
            if (to >= 100)
            {
              sprintf(serialBuffer, "e,%u,%u,%lu,%u;", to, totalBadTx, node.retransmissions(), totalTx); // sent to sleeping node, error
            }
            else
            {
              badTx[to]++;
              sprintf(serialBuffer, "f,%u,%u,%u,%lu,%u;", to, badTx[to], totalBadTx, node.retransmissions(), totalTx);
            }
          }
          Serial.print(serialBuffer);
        }
      }
    }
    else if (inChar == 'r')
    { // r,type,data;
      byte type = Serial.parseInt();
      byte data = Serial.parseInt();
      led9.off();
      if (Serial.read() == ';')
        answerRequest(type, data);
    }
    else
      led9.off();
  }
}

void answerRequest(byte type, byte data)
{
  switch (type)
  {
    case 0:
      reportErrors();
      break;

    case 1:
      resetErrors();
      reportErrors();
      break;

    case 2:
      wdt_enable(WDTO_15MS);
      delay(1000);
      break;

    case 3:
      reportRssiAndTemp();
      break;

    default:
      break; // add temperature, rssi, data on request
  }
}

void reportErrors(void)
{
  for (int i = 0; i < NODES; ++i)
  {
    Serial.print(F("h,"));
    Serial.print(i);
    Serial.print(F(","));
    Serial.print(goodTx[i]);
    Serial.print(F(","));
    Serial.print(badTx[i]);
    Serial.print(F(","));
    Serial.print(rxLow[i]);
    Serial.print(F(";"));
  }
  for (int i = 0; i < NODES; ++i)
  {
    Serial.print(F("i,"));
    Serial.print(i + 100);
    Serial.print(F(","));
    Serial.print(rxHigh[i]);
    Serial.print(F(";"));
  }
  char serialBuffer [30];
  sprintf(serialBuffer, "g,%u,%u,%u,%lu,%u;", totalTx, totalGoodTx, totalBadTx, node.retransmissions(), totalRx);
  Serial.print(serialBuffer);
}

void resetErrors(void)
{
  for (int i = 0; i < NODES; ++i)
  {
    goodTx[i] = 0;
    badTx[i] = 0;
    rxLow[i] = 0;
  }
  for (int i = 0; i < NODES; ++i)
  {
    rxHigh[i] = 0;
  }
  totalGoodTx = 0;
  totalBadTx = 0;
  totalTx = 0;
  node.resetRetransmissions();
  totalRx = 0;
}

void reportRssiAndTemp(void)
{
  Serial.print(F("j,"));
  Serial.print(node.rssi());
  Serial.print(F(","));
  Serial.print(node.temperature());
  Serial.print(F(";"));

}
/*
  void nodeFunction(byte type, byte num, long data)
  {
  char serialBuffer [35];
  switch (type)
  {
    case RESET:
      wdt_enable(WDTO_15MS);
      delay(1000);
      break;

    case GET_GOOD:
      sprintf(serialBuffer, "s,%d,%d,%d,%ld,%d,%d,%d;", 0, GOOD, 0, goodTx, 0, node.temperature(), 0);
      break;

    case GET_BAD:
      sprintf(serialBuffer, "s,%d,%d,%d,%ld,%d,%d,%d;", 0, BAD, 0, badTx, 0, node.temperature(), 0);
      break;

    case GET_RETX:
      sprintf(serialBuffer, "s,%d,%d,%d,%ld,%d,%d,%d;", 0, RETX, 0, node.retransmissions(), 0, node.temperature(), 0);
      break;

    case GET_TOTAL:
      sprintf(serialBuffer, "s,%d,%d,%d,%ld,%d,%d,%d;", 0, TOTAL, 0, totalTx, 0, node.temperature(), 0);
      break;

    case GET_PING:
      sprintf(serialBuffer, "s,%d,%d,%d,%ld,%d,%d,%d;", 0, PING_ANSWER, 0, 0, 0, node.temperature(), 0);
      break;

    case GET_MILLIS:
      sprintf(serialBuffer, "s,%d,%d,%d,%ld,%d,%d,%d;", 0, MILLIS, 0, millis(), 0, node.temperature(), 0);
      break;

    case SET_TXPOWER:
      node.setNewTxpower(data);
      //sprintf(serialBuffer, "s,%d,%d,%d,%ld,%d,%d,%d;", 0, TXPOWER, 0, node.getTxpower(), 0, node.temperature(), 0);
      break;

    case GET_TXPOWER:
      sprintf(serialBuffer, "s,%d,%d,%d,%ld,%d,%d,%d;", 0, TXPOWER, 0, node.getTxpower(), 0, node.temperature(), 0);
      break;

    case SET_RETRIES:
      node.setNewRetries(data);
      //sprintf(serialBuffer, "s,%d,%d,%d,%ld,%d,%d,%d;", 0, RETRIES, 0, node.getRetries(), 0, node.temperature(), 0);
      break;

    case GET_RETRIES:
      sprintf(serialBuffer, "s,%d,%d,%d,%ld,%d,%d,%d;", 0, RETRIES, 0, node.getRetries(), 0, node.temperature(), 0);
      break;

    case SET_TIMEOUT:
      node.setNewTimeout(data);
      //sprintf(serialBuffer, "s,%d,%d,%d,%ld,%d,%d,%d;", 0, TIMEOUT, 0, node.getTimeout(), 0, node.temperature(), 0);
      break;

    case GET_TIMEOUT:
      sprintf(serialBuffer, "s,%d,%d,%d,%ld,%d,%d,%d;", 0, TIMEOUT, 0, node.getTimeout(), 0, node.temperature(), 0);
      break;

    default:
      sprintf(serialBuffer, "s,%d,%d,%d,%ld,%d,%d,%d;", 0, TYPE_ERROR, 0, data, 0, node.temperature(), 0);
      break;
  }
  Serial.print(serialBuffer);
  }
*/
