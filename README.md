# ELM327
A library to communicate with an ELM327 device via an arduino's serial connection

```
#include "SoftwareSerial.h"
#include "PID.h"

SoftwareSerial serial(8, 7); // RX, TX
serial.begin(9600);

ELM327 obd(serial);
obd.get(PID_RPM);
```

```
#include "SoftwareSerial.h"
#include "PID.h"

// this might not work, just an example of how to mock
//  the required serial connection interface
class ArrayMockSerial extends MockSerial {
  int data[6] = {1, 2, 3, 4, 5, 6};
  int total = 6;

  void println(String payload) {
    // nothing
  }
  
  int available() {
    return total;
  }
  
  int read() {
    total--;
    return data[total];
  }
}

ArrayMockSerial serial;

ELM327 obd(serial);
obd.get(PID_RPM);
```


