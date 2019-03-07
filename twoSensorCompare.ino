//Adapted from "Multiple" example from DallasTemperature Library

// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
// manually inputed but code could be modified to store after searching
DeviceAddress insideThermometer = { 0x28, 0xD2, 0xF2, 0x8E, 0x9, 0x0, 0x0, 0xD3 };
DeviceAddress outsideThermometer   = { 0x28, 0x6E, 0xCD, 0xEE, 0x0A, 0x0, 0x0, 0x36 };

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  // Start up the library
  sensors.begin();

  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  // method 1: by index
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 1");
  if (!sensors.getAddress(outsideThermometer, 1)) Serial.println("Unable to find address for Device 2");
  // show the addresses we found on the bus
  Serial.print("Device 1 Address: ");
  printAddress(insideThermometer);
  Serial.println();

  Serial.print("Device 2 Address: ");
  printAddress(outsideThermometer);
  Serial.println();

  // set the resolution to 9 bit per device
  sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(outsideThermometer, TEMPERATURE_PRECISION);

  Serial.print("Device 1 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC);
  Serial.println();

  Serial.print("Device 2 Resolution: ");
  Serial.print(sensors.getResolution(outsideThermometer), DEC);
  Serial.println();

  Serial.print("Device 1 Type:");
  printDeviceType(insideThermometer);
  Serial.println();

  Serial.print("Device 2 Type:");
  printDeviceType(outsideThermometer);
  Serial.println();
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

float compareTemp;
// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempDifference;
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.print(DallasTemperature::toFahrenheit(tempC));
  Serial.println();
  if (deviceAddress==insideThermometer){
    compareTemp=tempC;
  }
  else if (deviceAddress==outsideThermometer){
    tempDifference=abs(compareTemp-tempC);
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    if (tempDifference > 10){
      Serial.println("Critical: Temperature Difference greater than 10!");
      digitalWrite(10, HIGH);
    }
    else if (tempDifference >= 5 && tempDifference <= 10){
      Serial.println("Warning: Temperature Difference is greater than 5");
      digitalWrite(9, HIGH);
    }
    else {
      Serial.println("Temperatures are similar");
      digitalWrite(8, HIGH);
    }
  }
}

// function to print a device's type (chip number)

void printDeviceType(DeviceAddress deviceAddress)
{
  byte type_s;
  switch (deviceAddress[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  }
}

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  if (deviceAddress==insideThermometer){
    Serial.println("Device 1");
  }
  else if (deviceAddress==outsideThermometer){
    Serial.println("Device 2");
  }
  Serial.print("Device Address: ");
  printAddress(deviceAddress);
  Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}

/*
   Main function, calls the temperatures in a loop.
*/
void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  sensors.requestTemperatures();
  // print the device information
  printData(insideThermometer);
  printData(outsideThermometer);
  delay(5000);
}
