/*
  ELM327.h - Library communicating with an ELM327 device over serial
  Created by Evan d'Entremont, March 2016

  ELM327 obd(Serial);
  obd.get(PID_RPM);

  TODO: Debug / test with a real car.
  TODO: handle max/min and throw out bad reads.
  TODO: throttle DELAYLENGTH until consistently getting good reads.
  TODO: Don't use delay for processing incoming data..
  TODO: Handle PID's supported - 00, 20, 30, 40, 50, 60, A0, C0
  TODO: Mode 09 02 (Read VIN)
  TODO: Bitmap / text string of manufacturer based on VIN 09 02 (bitmaps optional by define.)
  TODO: Mode 09 0A (Get ECU name)
*/
#ifndef ELM327_h
#define ELM327_h

#include "Arduino.h"
#include "HardwareSerial.h"
#include "PID.h"

#define DELAYLENGTH 500

class ELM327
{
  public:
    ELM327(HardwareSerial *serial);
    String get(String command)
  private:
    HardwareSerial& _HardSerial;
    String query(String command);
    String process(String command, String result);
};

#endif
