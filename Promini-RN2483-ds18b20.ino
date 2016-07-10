

/*
 * Author: Anucha Promwungkwa, Ph.D.
 * Date:   2016-07-10
 * 
 * This code is orginal by JP Meijers, 2016-02-07
 * 
 * TTN sensor using DS18B20. Arduino ProMini + RN2483.
 * 
 * CHANGE ADDRESS!
 * Change the address this device should use to either a testing address or one from a range you "own".
 * The appropriate line is "myLora.initTTN("XXXXXX");
 * 
 * Arrange ProMini and RN2483 back-to-back. Find closest pins and connect the RN2483 as follows:
 * RN2483   -- Arduino
 * Uart TX  -- A2
 * Uart RX  -- A3
 * Reset    -- 7
 * Vcc      -- 3.3V
 * Gnd      -- Gnd
 * 
 * Connect DS18B20 to pin 2 of Arduino.
 * 
 * If you use an Arduino with a free hardware serial port, you can replace 
 * the line "rn2483 myLora(mySerial);"
 * with     "rn2483 myLora(SerialX);"
 * where the parameter is the serial port the RN2483 is connected to.
 * Remember that the serial port should be initialised before calling initTTN().
 * For best performance the serial port should be set to 57600 baud, which is impossible with a software serial port.
 * If you use 57600 baud, you can remove the line "myLora.autobaud();".
 * 
 */
 
#include <rn2483.h>
#include <SoftwareSerial.h>
#include "OneWire.h"
#include "DallasTemperature.h"

//SoftwareSerial mySerial(10, 11); // RX, TX

SoftwareSerial mySerial(A3, A2); // RX, TX

String        str;
float         temperature;
char          msgBuf[40];

unsigned long time = 0;

// OneWire bus pin for temperature sensor pin
#define ONE_WIRE_BUS 2
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress temperatureSensor;

//create an instance of the rn2483 library, 
//giving the software UART as stream to use,
//and using LoRa WAN
rn2483 myLora(mySerial);

// the setup routine runs once when you press reset:
void setup() {
  //output LED pin
  pinMode(13, OUTPUT);
  led_on();
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("Startup");

  //reset rn2483
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  delay(500);
  digitalWrite(7, HIGH);

  //initialise the rn2483 module
  myLora.autobaud();

  //print out the HWEUI so that we can register it via ttnctl
  Serial.println("When using OTAA, register this DevEUI: ");
  Serial.println(myLora.hweui());
  Serial.println("RN2483 version number:");
  Serial.println(myLora.sysver());
  
  //OTAA: init(String AppEUI, String AppKey);
  myLora.init("70B3D57ED0000139", "8AAFA2276A6C7BBA717EBD2A2894118F");
  
  //transmit a startup message
  myLora.txUncnf("Fast TXer (TTN)");

  led_off();
  delay(2000);
}

// the loop routine runs over and over again forever:
void loop() {
    led_on();

    sensors.requestTemperatures();
    temperature = sensors.getTempCByIndex(0);
    Serial.println(temperature);

    int itemp = int(temperature*10);
    sprintf(msgBuf, "%d",itemp);
    Serial.println(msgBuf);
    myLora.txUncnf(msgBuf);
   
    led_off();
    delay(30000);
}

void led_on()
{
  digitalWrite(13, 1);
}

void led_off()
{
  digitalWrite(13, 0);
}

