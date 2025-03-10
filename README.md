# SPI Library Together With nRF24L01
SPI library for the AVR Attiny84a used together with the nRF24L01 radio module 
with adapter board.

## Description
The project is built around the nRF24L01 radio module with a new SPI library for
the attiny84a, written as part of the project.
The radio library comes with an example where "ON" and "OFF" are sent to
controll a LED. The example has been rewritten to count button pushes, utilizing
interrupt and timer functionallity. The number of button pushes is sent and a
LED at the receiver side blinks the number received.

<img width="250" src="spi-avr-radio.png"/>

## Getting Started

### Dependencies
(Orignal version, included and modified in the project)
avr-nrf24l01
https://github.com/antoineleclair/avr-nrf24l01

### Installing

1. Clone the repository
1. Build the code for the transmitter side for ATtiny84 using the AVR 8-bit GNU toolchain

   ```
   avr-gcc -Wall -g -Os -mmcu=attiny84 -o main.bin attiny84a_spi.c avr-nrf24l01/nrf24l01.c avr-nrf24l01/Tx/main.c
   avr-objcopy -j .text -j .data -O ihex main.bin main.hex
   ```
1. Flash the microcontroller using i.e. AVRDUDE
   
   `avrdude -p attiny84 -c usbasp -U flash:w:main.hex:i -F -P usb`

1. Build the code for the receiver side for ATtiny84 using the AVR 8-bit GNU toolchain
   
   ```
   avr-gcc -Wall -g -Os -mmcu=attiny84 -o main.bin attiny84a_spi.c avr-nrf24l01/nrf24l01.c avr-nrf24l01/Rx/main.c
   avr-objcopy -j .text -j .data -O ihex main.bin main.hex
   ```
1. Flash the microcontroller using i.e. AVRDUDE, see above

## Authors
Sara Rydh

## License
This project is licensed under the GPL License - see the LICENSE.md
file for details.








