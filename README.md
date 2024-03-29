# powergrid-frequncy
Measure and display the frequency of the powergrid with an ESP8266

This project is based on the Project [Lamp_Simple1Button](https://github.com/orithena/Lamp_Simple1Button) by Dave. Thank's for the inspiration ;)

## Parts

* 1x Board: [WeMos D1 mini](https://wiki.wemos.cc/products:d1:d1_mini) with ESP8266 processor
* LED strip with 15 [WS2812b](https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf) LEDs
* 1x 100kOhm resistor
* 3x 10kOhm resistor
* 1 diode
* Powered via MicroUSB (5V, 1A)


## Software

* [Arduino IDE](https://www.arduino.cc/en/Main/Software)
* ESP8266 toolchain
  * Add in File -> Preferences -> Additional board manager URLs: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
  * Search in Tools -> Board -> Board Manager for "esp8266", then install version 2.5.2
  	* Select "WeMos D1 R2 & mini" or "LOLIN(WeMos) D1 R2 & mini" in Tools -> Board menu
* Libraries
  * Sketch -> Include Library -> Manage Libraries -> Search for "Button" and "[FastLED](http://fastled.io/)", install them.

## Troubleshooting

* Compile error: "D6/D3 was not declared in this scope": Select the right board (see above)

## Details

