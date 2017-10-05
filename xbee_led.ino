// This sketch will cause an XBee radio attached to the Arduino to
// listen for requests from another XBee radio. It will take the
// payload received by the XBee and add it to a dynamically allocated
// array of strings (character arrays). It will attempt to grow the 
// array holding the strings as it adds them.

// In order to use this sketch, the XBee radio (Series 1 used here)
// must be setup to use API mode (AP=2 in X-CTU).
// I use pins 2 and 3 here, but those can be changed in the defines
// below.

#include <XBee.h>
#include <SoftwareSerial.h>
#include <stdlib.h>
#include <string.h>

#define ssRxPin 4
#define ssTxPin 5
#define ledPin 10


// Setup XBee variables
SoftwareSerial nss(ssRxPin, ssTxPin);
XBee xbee = XBee();

// Turn on I/O sampling
uint8_t irCmd[] = {'I','R'};
// Set sample rate to 65 seconds (0xffff/1000)
uint8_t irValue[] = { 0xff, 0xff };

// Set DIO0 (pin 20) to Digital Output {0x4 == LOW, 0x5 == HIGH }
uint8_t d0Cmd[] = { 'D', '0' };
uint8_t low_voltage[] = { 0x4 };
uint8_t high_voltage[] = { 0x5 };

// SH + SL of your remote radio
XBeeAddress64 remoteAddress = XBeeAddress64(0x0013a200, 0x414faf83);
// Create a remote AT request with the IR command
RemoteAtCommandRequest remoteAtRequest = RemoteAtCommandRequest(remoteAddress, irCmd, irValue, sizeof(irValue));

// Create a Remote AT response object
RemoteAtCommandResponse remoteAtResponse = RemoteAtCommandResponse();


void flashLed(int pin, int times, int wait) {
    
    for (int i = 0; i < times; i++) {
      digitalWrite(pin, HIGH);
      delay(wait);
      digitalWrite(pin, LOW);
      
      if (i + 1 < times) {
        delay(wait);
      }
    }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  
  Serial.begin(9600);
  nss.begin(9600);
  xbee.setSerial(nss);
  
  flashLed(ledPin, 3, 50);
}


// continuously writes AT packets, sending to ZB transmitter
void loop() {
    switch2NPN(HIGH, 5000);
    switch2NPN(LOW, 5000);
}

// send current to switch on transistor NPN 2P2222 base pin
void switch2NPN(boolean sw1, uint8_t d1) {    
  sendRemoteAtCommand();
  // now reuse same object for DIO0 command
  remoteAtRequest.setCommand(d0Cmd);
  if (sw1 == LOW) {
    remoteAtRequest.setCommandValue(low_voltage);
    remoteAtRequest.setCommandValueLength(sizeof(low_voltage));
  } else {
    remoteAtRequest.setCommandValue(high_voltage);
    remoteAtRequest.setCommandValueLength(sizeof(high_voltage));    
  }
  sendRemoteAtCommand();  
  // it's a good idea to clear the set value so that the object can be reused for a query
  remoteAtRequest.clearCommandValue();
  delay(d1);
}

void sendRemoteAtCommand() {
  Serial.println("Sending command to the XBee");

  // send the command
  xbee.send(remoteAtRequest);

  // wait up to 5 seconds for the status response
  if (xbee.readPacket(5000)) {
    // got a response!

    // should be an AT command response
    if (xbee.getResponse().getApiId() == REMOTE_AT_COMMAND_RESPONSE) {
      xbee.getResponse().getRemoteAtCommandResponse(remoteAtResponse);

      if (remoteAtResponse.isOk()) {
        Serial.print("Command [");
        Serial.print(remoteAtResponse.getCommand()[0]);
        Serial.print(remoteAtResponse.getCommand()[1]);
        Serial.println("] was successful!");

        if (remoteAtResponse.getValueLength() > 0) {
          Serial.print("Command value length is ");
          Serial.println(remoteAtResponse.getValueLength(), DEC);

          Serial.print("Command value: ");
          
          for (int i = 0; i < remoteAtResponse.getValueLength(); i++) {
            Serial.print(remoteAtResponse.getValue()[i], HEX);
            Serial.print(" ");
          }

          Serial.println("");
        }
      } else {
        Serial.print("Command returned error code: ");
        Serial.println(remoteAtResponse.getStatus(), HEX);
      }
    } else {
      Serial.print("Expected Remote AT response but got ");
      Serial.print(xbee.getResponse().getApiId(), HEX);
    }    
  } else if (xbee.getResponse().isError()) {
    Serial.print("Error reading packet.  Error code: ");  
    Serial.println(xbee.getResponse().getErrorCode());
  } else {
    Serial.print("No response from radio");  
  }
}


