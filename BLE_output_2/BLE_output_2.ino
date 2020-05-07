/*
  Peripheral Explorer

  This example scans for BLE peripherals until one with a particular name ("LED")
  is found. Then connects, and discovers + prints all the peripheral's attributes.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.

  You can use it with another board that is compatible with this library and the
  Peripherals -> LED example.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>

const int buttonPin = 8;
const int ledPin    = 6;
const int pompPin   = 5;

int oldButtonState = LOW;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // initialize the BLE hardware
  if (!BLE.begin()) while (1); //Serial.println("starting BLE failed!");


  // configure the button pin as input
  pinMode(buttonPin, INPUT);
  pinMode(ledPin,   OUTPUT);

  // start scanning for peripherals
  //Serial.println("BLE Central - Peripheral Explorer");
}

void loop() {
  // start scanning for peripherals
  BLE.scanForUuid("19B10010-E8F2-537E-4F6C-D104768A1214");

  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    // discovered a peripheral, print out address, local name, and advertised service
    // Serial.print("\n\nFound ");
    // Serial.print(peripheral.address());
    // Serial.print(" '");
    // Serial.print(peripheral.localName());
    // Serial.print("' ");
    // Serial.print(peripheral.advertisedServiceUuid());
    // Serial.println();

    // see if peripheral is a LED
    if (peripheral.localName() == "Pressure") {
      // stop scanning
      BLE.stopScan();

      explorerPeripheral(peripheral);

    }
  }
  delay(1000);
  // we are done exploring, disconnect
  Serial.println("\n\nDisconnecting ...");
  peripheral.disconnect();
  Serial.println("Disconnected");
}

void explorerPeripheral(BLEDevice peripheral) {
  // connect to the peripheral
  //Serial.println("Connecting ...");

  if (!peripheral.connect()) return;


  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (!peripheral.discoverAttributes()) {peripheral.disconnect()}

  // read and print device name of peripheral
  Serial.println();
  Serial.print("Device name: ");
  Serial.println(peripheral.deviceName());
  Serial.print("Appearance: 0x");
  Serial.println(peripheral.appearance(), HEX);

  // loop the services of the peripheral and explore each
  //for (int i = 0; i < peripheral.serviceCount(); i++) {BLEService service = peripheral.service(i);exploreService(service);}

  // retrieve the PRESSURE characteristic
  BLEService service = peripheral.service("19B10010-E8F2-537E-4F6C-D104768A1214");
  BLECharacteristic characteristic = service.characteristic(1);


  // subscribe to the simple key characteristic
  Serial.println("Subscribing to simple key characteristic ...");
  if (!characteristic) {
    Serial.println("no simple key characteristic found!");
    peripheral.disconnect();
    return;
  } else if (!characteristic.canSubscribe()) {
    Serial.println("simple key characteristic is not subscribable!");
    peripheral.disconnect();
    return;
  } else if (!characteristic.subscribe()) {
    Serial.println("subscription failed!");
    peripheral.disconnect();
    return;
  } else {
    Serial.println("Subscribed");
    Serial.println("Press the right and left buttons on your SensorTag.");
  }

  while (peripheral.connected()) {
    // while the peripheral is connected

    // read the button pin
    int buttonState = digitalRead(buttonPin);
    //if (oldButtonState == buttonState || characteristic.written()) continue;
   
    oldButtonState = buttonState;
    
    if (!characteristic.valueUpdated()) continue;

    //if (!buttonState) continue;
    // check if the characteristic is readable
    //if (!characteristic.canRead()) return;
    // read the characteristic value
    characteristic.read();

    // print the UUID and properties of the characteristic
    Serial.print("\n\n\tCharacteristic ");
    Serial.print(characteristic.uuid());
    Serial.print(", properties 0x");
    Serial.print(characteristic.properties(), HEX);

    Serial.print("\n\tPREASSURE:  ");
    int pressure = getValue(characteristic.value(), characteristic.valueLength());
    Serial.print(pressure); Serial.print(" Pa,  (0x");
    printData(characteristic.value(), characteristic.valueLength());Serial.print(")");
    
    if (pressure > 103000) {digitalWrite(ledPin, HIGH);}
    else {digitalWrite(ledPin, LOW);}
    

  }
  Serial.println();
}


int getValue(const uint8_t data[], int length) {
  int val = 0;
  for (int i = 0; i < length; i++) val += byteValue(data[i], i);
  return val;
}

int byteValue(int val, int p) {
  if (p > 0) val = byteValue(val, p - 1) * 256;
  return val;
}

void printData(const unsigned char data[], int length) {
  for (int i = 0; i < length; i++) {
    unsigned char b = data[i];

    if (b < 16) {
      Serial.print("0");
    }

    Serial.print(b, HEX);
  }
}
