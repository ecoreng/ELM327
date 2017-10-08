/*
  ELM327.cpp - Library communicating with an ELM327 device over serial
  Created by Evan d'Entremont, March 2016
*/

#include "Arduino.h"
#include "PID.h"
#include "elm327.h"
#include "HardwareSerial.h"
#include "SoftwareSerial.h"


ELM327::ELM327(HardwareSerial &serial) {
  this->serial = serial;
  this->init();
}

ELM327::ELM327(SoftwareSerial &serial) {
  this->serial = serial;
  this->init();
}

ELM327::ELM327(MockSerial &serial) {
  this->serial = serial;
  this->init();
}

void ELM327::init() {
  String result;

  delay(DELAYLENGTH);
  Retry:
    result = query("ATI");

    // Should abstract this.
    if (result.substring(1, 7) != "ELM327"){
      delay(DELAYLENGTH);
      // This really ought to return on failure.
      goto Retry;
    }
}

// Query and process
String ELM327::get(String command) {
  return process(command, query(command));
}

// Query and return
String ELM327::query(String command) {
  String inString = "";
  byte inData = 0;
  char inChar = 0;
  unsigned long time;

  // send the query
  this->serial->println(command);
  time = millis();
  
  // waiting here until we get a response
  while(this->serial->available() < 1) {
    if(millis() > (time + SERIALTIMEOUT)){
      return "";
    }
  }
  // read the whole response
  while(this->serial->available() > 0){
    inData = 0;
    inChar = 0;
    byte inData = this->serial->read();
    char inChar = char(inData);
    inString = inString + inChar;
  }

  inString.replace(command, "");
  inString.replace(">", "");
  inString.replace("OK", "");

  // Some of these look like errors that ought to be handled..
  inString.replace("STOPPED", "");
  inString.replace("SEARCHING", "");
  inString.replace("NO DATA", "");
  inString.replace("?", "");
  inString.replace(",", "");

  return inString;
}

// Process the response based on the queried command
String ELM327::process(String command, String result) {
  long DisplayValue;
  int ByteCount = 0;
  long A;
  int B;
  String WorkingString = "";

  //Check which OBD Command was sent and calculate VALUE
  //Calculate RPM I.E Returned bytes wil be 41 0C 1B E0
  if (command == PID_RPM) {
    WorkingString = result.substring(7, 9);
    A = strtol(WorkingString.c_str(), NULL, 0);
    WorkingString = result.substring(11, 13);
    B = strtol(WorkingString.c_str(), NULL, 0);

   DisplayValue = ((A * 256) + B) / 4;
   return String(DisplayValue) + " rpm";
  }

  //Calculate Vehicle speed
  // I.E Returned bytes wil be 41 0C 1B E0
  else if (command == PID_SPEED) {
    WorkingString = result.substring(7, 9);
    A = strtol(WorkingString.c_str(), NULL, 0);
    DisplayValue = A;
    return String(DisplayValue) + " km/h";
  }

  //Coolant Temp
  else if (command == PID_COOLANT) {
    WorkingString = result.substring(7, 9);
    A = strtol(WorkingString.c_str(), NULL, 0);
    DisplayValue = A;
    return String(DisplayValue) + " C";
  }

  //IAT Temp
  else if (command == PID_IAT_TEMP) {
    WorkingString = result.substring(7, 9);
    A = strtol(WorkingString.c_str(), NULL, 0);
    DisplayValue = A;
    return String(DisplayValue) + " C";
  }

  //Air flow Rate
  else if (command == PID_AIR_FLOW_RATE) {
    WorkingString = result.substring(7, 9);
    A = strtol(WorkingString.c_str(), NULL, 0);
    WorkingString = result.substring(11, 13);
    B = strtol(WorkingString.c_str(), NULL, 0);
    DisplayValue = ((A * 256) + B) / 100;
    return String(DisplayValue) + " g/s";
  }

  //Ambient Temp
  else if (command == PID_AMBIENT_TEMP) {
    WorkingString = result.substring(7, 9);
    A = strtol(WorkingString.c_str(), NULL, 0);
    DisplayValue = A;
    return String(DisplayValue) + " C";
  }

  //Throttle position
  else if (command == PID_THROTTLE) {
    WorkingString = result.substring(7, 9);
    A = strtol(WorkingString.c_str(), NULL, 0);
    DisplayValue = A;
    return String(DisplayValue) + " %";
  }

  //Barometric pressure
  else if (command == PID_BAROMETRIC_PRESSURE) {
    WorkingString = result.substring(7, 9);
    A = strtol(WorkingString.c_str(), NULL, 0);
    DisplayValue = A;
    return String(DisplayValue) + " kpa";
  }

  else {
    return String(DisplayValue);
  }
}
