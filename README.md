# IR2USB - IR to USB HID Converter based on ATtiny45/85
IR2USB receives signals from an infrared remote control and converts them into keyboard inputs, mouse movements or joystick inputs. Since it is recognized as a generic Human Interface Device (HID) via USB on the PC, no driver installation is necessary.

- Design Files (EasyEDA): https://easyeda.com/wagiminator/attiny85-ir2usb

![pic1.jpg](https://raw.githubusercontent.com/wagiminator/ATtiny85-IR2USB/main/documentation/IR2USB_pic1.jpg)

# Hardware
The schematic is shown below:

![Wiring.png](https://raw.githubusercontent.com/wagiminator/ATtiny85-IR2USB/main/documentation/IR2USB_wiring.png)

# Software
The communication via USB is handled by the [V-USB](https://www.obdev.at/products/vusb/index.html) software-only implementation of a low-speed USB device. To simplify the software development with the Arduino IDE the [VUSB_AVR board package](https://github.com/wagiminator/VUSB-AVR) is used. It includes libraries to implement keyboard, mouse and joystick devices, which makes it easy to implement a converter.

The IR receiver implementation is based on [NeoController](https://github.com/wagiminator/ATtiny13-NeoController) and supports the **NEC protocol** only, but this is used by almost all cheap IR remote controls. Alternatively, you can build such a remote control yourself with [TinyRemote](https://github.com/wagiminator/ATtiny13-TinyRemote).

# Compiling and Uploading
Since there is no ICSP header on the board, you have to program the ATtiny either before soldering using an [SOP adapter](https://aliexpress.com/wholesale?SearchText=sop-8+150mil+adapter), or after soldering using an [EEPROM clip](https://aliexpress.com/wholesale?SearchText=sop8+eeprom+programming+clip). The [AVR Programmer Adapter](https://github.com/wagiminator/AVR-Programmer/tree/master/AVR_Programmer_Adapter) can help with this.

- Open Arduino IDE.
- [Install VUSB-AVR](https://github.com/wagiminator/VUSB-AVR#Installation).
- Go to **Tools -> Board -> VUSB AVR** and select **VUSB-AVR**.
- Go to **Tools -> CPU** and select **ATtiny85 (16.5 MHz internal)**.
- Connect your programmer to your PC and to the ATtiny.
- Go to **Tools -> Programmer** and select your ISP programmer.
- Go to **Tools -> Burn Bootloader** to burn the fuses.
- Open the sketch and click **Upload**.
- Disconnect the programmer and connect the device via USB to your PC.

# References, Links and Notes
1. [ATtiny45/85 Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2586-AVR-8-bit-Microcontroller-ATtiny25-ATtiny45-ATtiny85_Datasheet.pdf)
2. [TSOP4838 datasheet](https://www.vishay.com/docs/82459/tsop48.pdf)
3. [V-USB](https://www.obdev.at/products/vusb/index.html)
4. [TinyRemote](https://github.com/wagiminator/ATtiny13-TinyRemote)

![pic2.jpg](https://raw.githubusercontent.com/wagiminator/ATtiny85-IR2USB/main/documentation/IR2USB_pic2.jpg)

# License
![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
