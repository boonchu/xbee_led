##### XBee #####

- Youtube links:
    - Xbee Wireless LED application (https://youtu.be/5BizgV030pQ)

###### Hardwares :
- Arduino UNO R3
- 2x XBee S2C edition (https://www.digi.com/products/models/xb24cz7wit-004)
- XBee Explorer USB adapter
- 2x XBee breakout boards
- Voltage regulator from 5 to 3.3 volt

###### Sample: writing data to remote radio device (AT mode at router)
- This example demostrates how write data to radio Xbee router

```
    "Second XBee" unit (Router): 
        XBee PIN 1: vcc 3v3 
        XBee PIN 10: GND    
        XBee PIN 20: NPN Transistor base pin
```

- see breadboard pin wiring figure below
- upload "xbee_led" to UNO

###### How it works:
- coordinator XBee transmits data to remote XBee 
- Local XBee writes AT strings and transmits to remote XBee to configure IOSamping rate
```
+++
ATIR 0xffff
ATWR
ATCN
```

- At the loop, XBee writes AT data to transfer AT commands to switch D0 to HIGH and LOW 

```
+++
ATD0 5 (LOW->HIGH)
ATCN
+++
ATD0 4 (HIGH->LOW)
ATCN
```

- observe changes from serial console 
- the message status changes from HIGH to LOW or LOW to HIGH when D0 status change from
  remote radio end

![ScreenShot](https://github.com/boonchu/xbee_led/blob/master/NPN_2P2222_Switch.png)

![ScreenShot](https://github.com/boonchu/xbee_led/blob/master/XBee_PCB_design.png)

![ScreenShot](https://github.com/boonchu/xbee_led/blob/master/output.png)
