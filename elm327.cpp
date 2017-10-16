/*
  ELM327.cpp - Library communicating with an ELM327 device over serial
  Created by Evan d'Entremont, March 2016
*/

#include "Arduino.h"
#include "elm327.h"
#include "HardwareSerial.h"
#include "SoftwareSerial.h"
#include "PID.h"


template <class T>
ELM327<T>::ELM327(T *serial) {
  this->serial = serial;
}

template <class T>
void ELM327<T>::init() {
  String result;

  Retry:
    result = query("ATI");
    // Should abstract this.
    if (result.substring(1, 7) != "ELM327") {
      delay(DELAYLENGTH);
      // This really ought to return on failure.
      goto Retry;
    }
}

// Query and process
template <class T>
Measurement ELM327<T>::get(String command) {
  return process(command, query(command));
}

// Query and return
template <class T>
String ELM327<T>::query(String command) {
  String inString = "";
  byte inData = 0;
  char inChar = 0;
  unsigned long time;

  // send the query
  this->serial->println(command);

  if (DEBUG_ELM == 1) {
    Serial.println("sending: " + command);
  }

  time = millis();
  
  // waiting here until we get a response
  while(this->serial->available() < 1) {
    if(millis() > (time + SERIALTIMEOUT)) {
      return "";
    }
  }
  // read the whole response
  while(this->serial->available() > 0) {
    inData = 0;
    inChar = 0;
    byte inData = this->serial->read();
    char inChar = char(inData);
    inString = inString + inChar;
  }

  if (DEBUG_ELM == 1) {
    Serial.println("received: " + inString);
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
template <class T>
Measurement ELM327<T>::process(String command, String result) {
  long DisplayValue;
  int ByteCount = 0;
  long A;
  int B;
  Measurement measurement;
  String WorkingString = "";

  //Check which OBD Command was sent and calculate VALUE
  //Calculate RPM I.E Returned bytes wil be 41 0C 1B E0
  if (command == PID_RPM) {
    WorkingString = "0x" + result.substring(7, 9);
    A = strtol(WorkingString.c_str(), NULL, 0);
    WorkingString = "0x" + result.substring(11, 13);
    B = strtol(WorkingString.c_str(), NULL, 0);
    DisplayValue = ((A * 256) + B) / 4;

    measurement.value = String(DisplayValue);
    measurement.unit = "rpm";
    measurement.raw = DisplayValue / 16383.75 * 1023;
  }

  //Calculate Vehicle speed
  // I.E Returned bytes wil be 41 0C 1B E0
  else if (command == PID_SPEED) {
    WorkingString = "0x" + result.substring(7, 9);
    A = strtol(WorkingString.c_str(), NULL, 0);
    DisplayValue = A;

    measurement.value = String(DisplayValue);
    measurement.unit = "km/h";
    measurement.raw = DisplayValue / 255 * 1023;
  }

  //Coolant Temp
  else if (command == PID_COOLANT_TEMP) {
    WorkingString = "0x" + result.substring(7, 9);
    A = strtol(WorkingString.c_str(), NULL, 0);
    DisplayValue = A;

    measurement.value = String(DisplayValue);
    measurement.unit = "C";
    measurement.raw = DisplayValue / 255 * 1023;
  }

  //IAT Temp
  else if (command == PID_IAT_TEMP) {
    WorkingString = "0x" + result.substring(7, 9);
    A = strtol(WorkingString.c_str(), NULL, 0);
    DisplayValue = A;

    measurement.value = String(DisplayValue);
    measurement.unit = "C";
    measurement.raw = DisplayValue / 255 * 1023;
  }

  //Air flow Rate
  else if (command == PID_AIR_FLOW_RATE) {
    WorkingString = "0x" + result.substring(7, 9);
    A = strtol(WorkingString.c_str(), NULL, 0);
    WorkingString = "0x" + result.substring(11, 13);
    B = strtol(WorkingString.c_str(), NULL, 0);
    DisplayValue = ((A * 256) + B) / 100;

    measurement.value = String(DisplayValue);
    measurement.unit = "g/s";
    measurement.raw = DisplayValue / 655.35 * 1023;
  }

  //Ambient Temp
  else if (command == PID_AMBIENT_TEMP) {
    WorkingString = "0x" + result.substring(7, 9);
    A = strtol(WorkingString.c_str(), NULL, 0);
    DisplayValue = A;

    measurement.value = String(DisplayValue);
    measurement.unit = "C";
    measurement.raw = DisplayValue / 255 * 1023;
  }

  //Throttle position
  else if (command == PID_THROTTLE) {
    WorkingString = "0x" + result.substring(7, 9);
    A = strtol(WorkingString.c_str(), NULL, 0);
    DisplayValue = A;

    measurement.value = String(DisplayValue);
    measurement.unit = "%";
    measurement.raw = DisplayValue / 100 * 1023;
  }

  //Barometric pressure
  else if (command == PID_BAROMETRIC_PRESSURE) {
    WorkingString = "0x" + result.substring(7, 9);
    A = strtol(WorkingString.c_str(), NULL, 0);
    DisplayValue = A;

    measurement.value = String(DisplayValue);
    measurement.unit = "kpa";
    measurement.raw = DisplayValue / 255 * 1023;
  }

  else {
    measurement.value = String(DisplayValue);
    measurement.unit = "";
    measurement.raw = DisplayValue;
  }

  if (DEBUG_ELM == 1) {
    Serial.println(measurement.value + " " + measurement.unit);
    Serial.println("raw: " + String(measurement.raw));
  }

  return measurement;
}
