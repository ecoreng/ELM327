# ELM327
A library to communicate with an ELM327 device via an arduino's serial connection

Software Serial (Cable, Bluetooth SPP, etc)

```
#include "SoftwareSerial.h"
#include "PID.h"

SoftwareSerial serial(8, 7); // RX, TX
serial.begin(9600);

ELM327 obd(serial);
obd.get(PID_RPM);
```


Mock Serial to test without a car

```
#include "elm327.h"
#include "elm327.cpp"
#include "PID.h"

class ArrayMockSerial : public MockSerial {
  char data[20];
  int total = 20;
  int i = 0;

  public:
    void println(String payload) {
      i = 0;
      if (payload == "ATI") {
        strcpy(data, " ELM327            ");
      }
      else if (payload == PID_RPM) {
        strcpy(data, " ZZ ZZ 1F  FF      ");
      }
    }
    
    int available() {
      return total - i;
    }
    
    int read() {
      return data[i++];
    }
};

ArrayMockSerial serialMock;
ELM327<ArrayMockSerial> obd(&serialMock);


void setup() {
  delay(500);
  obd.init();
}

void loop() {
  String rpm = obd.get(PID_RPM);
  delay(1500);
}

```
