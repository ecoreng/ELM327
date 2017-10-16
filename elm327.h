/*
  ELM327.h - Library communicating with an ELM327 device over serial
  Created by Evan d'Entremont, March 2016

  TODO: handle max/min and throw out bad reads.
  TODO: Don't use delay for processing incoming data.. Wait for > character.
  TODO: Handle PID's supported - 00, 20, 30, 40, 50, 60, A0, C0
  TODO: Mode 09 02 (Read VIN)
  TODO: Bitmap / text string of manufacturer based on VIN 09 02 (bitmaps optional by define.)
  TODO: Mode 09 0A (Get ECU name)
  TODO: Handle Trouble Codes.
  TODO: Turn headers off  (AT H0)
  TODO: Turn Echo off (perhaps add a debug mode with optional serial parameter?) (AT E0)
  TODO: investigate AT CS (detects hardware errors)
  TODO: A nicer hex to dec wrapper
*/
#ifndef ELM327_h
#define ELM327_h

#include "Arduino.h"
#include "HardwareSerial.h"
#include "SoftwareSerial.h"


#ifndef DELAYLENGTH
  #define DELAYLENGTH 500
#endif

#ifndef SERIALTIMEOUT
  #define SERIALTIMEOUT 500
#endif

#ifndef DEBUG_ELM
  #define DEBUG_ELM 0
#endif



struct Measurement {
  int raw;
  String value;
  String unit;
};


class MockSerial
{
  public:
    virtual void println(String payload) = 0;
    virtual int available() = 0;
    virtual int read() = 0;
};


template <class T>
class ELM327
{
  public:
    ELM327(T *serial);
    void init();
    Measurement get(String command);
    String query(String command);
  private:
    T *serial;
    Measurement process(String command, String result);
};

#endif
