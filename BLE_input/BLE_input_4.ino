#include <ArduinoBLE.h>
#include <Arduino_LPS22HB.h>

const int buttonPin = 2; // set buttonPin to digital pin 4
long previousMillis = 0;  // last time the battery level was checked, in ms

BLEService pressureService("19B10010-E8F2-537E-4F6C-D104768A1214"); // create service

// create button characteristic and allow remote device to get notifications
BLEByteCharacteristic buttonCharacteristic("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
// create switch characteristic and allow remote device to read and write
BLEIntCharacteristic pressureCharacteristic("19B10013-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
// create switch characteristic and allow remote device to read and write


void setup() {
  // Serial.begin(9600);
  // while (!Serial);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(buttonPin, INPUT); // use button pin as an input
  
  // begin init 
  if (!BARO.begin()) while (1);
  // begin initialization
  if (!BLE.begin()) while (1);


  // set the local name peripheral advertises
  BLE.setLocalName("Pressure");
  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(pressureService);

  // add the characteristics to the service
  pressureService.addCharacteristic(buttonCharacteristic);
  pressureService.addCharacteristic(pressureCharacteristic);

  // add the service
  BLE.addService(pressureService);

  buttonCharacteristic.writeValue(0);
  pressureCharacteristic.writeValue(0);
  

  // start advertising
  BLE.advertise();

  //Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {

  // poll for BLE events
  BLE.poll();

  // read the current button pin state
  char buttonValue = digitalRead(buttonPin);
  // has the value changed since the last read
  boolean buttonChanged = (buttonCharacteristic.value() != buttonValue);
  //if (!buttonChanged) return;
  // button state changed, update characteristics
  buttonCharacteristic.writeValue(buttonValue);
  
  long currentMillis = millis();
  if (currentMillis - previousMillis < 200) return;
  previousMillis = currentMillis;

  
  // PRESSURE, read the sensor value
  int pressure = 1000 * BARO.readPressure(); 
  
  if (abs(pressureCharacteristic.value()-pressure) < 10) return;
  //Serial.println(pressureCharacteristic.value());

  //if (!buttonValue) return;
  pressureCharacteristic.writeValue(pressure);
  
  blink(100,2);
  
}



void blink(int t, int n){
  do {  n--;
  digitalWrite(13, HIGH);
  delay(t);
  digitalWrite(13, LOW);
  delay(t);
  } while (n>0);
}