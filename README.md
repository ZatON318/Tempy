
<h1 align="center">
  Tempy [Work in progress]
  <br>
</h1>

<h4 align="center">Tempy is ESP32 project for measuring temperature using different sensors and communication using Ethernet over SNMP


<p align="center">
  <a href="#key-features">Key Features</a> •
  <a href="#Hardware">Hardware</a> •
  <a href="#how-to-use">How To Use</a> •
  <a href="#license">License</a>
</p>

## Key Features

* Designed to be used in server racks or anywhere you want thanks to PoE feature
* Easy setup 

## Hardware

 This project is mainly based on OLIMEX ESP32-PoE-ISO board, DS18B20 temperature sensor and 4.7k Ohm resistor.

 Temp sensor have 3 pins.  DQ pin is One Wire Bus. This pin is predefined to pin 4 of our board. Also 4.7k Ohm resistor should be connected to VDD and DQ
 ![picture alt](https://i0.wp.com/randomnerdtutorials.com/wp-content/uploads/2019/07/DS18B20-tempeature-sensor-pinout.jpg?w=750&quality=100&strip=all&ssl=1 "DS18B20")

  ### Step-by-Step

  * Connect DS18B20 GND and VDD(+5v) to ESP32
  * DQ pin of DS18B20 connect to pin 4 of ESP32
  * Connect VDD(+5v) to DQ pin using 4.7k Ohm resistor

 Link to board: https://www.olimex.com/Products/IoT/ESP32/ESP32-POE-ISO/open-source-hardware

 

## How To Use

You will need to clone this repository using git clone...

```powershell
# Clone this repository
git clone https://github.com/ZatON318/Tempy.git
```
Then open TempyNode_2.ino using Arduino IDE and upload sketch to your OLIMEX ESP32-PoE-ISO board

### Setup

> **Note**
> Setup is preaty similiar to how would you setup Ubuquity Nanostation.

1. You will need to connect ethernet to your PC and power the board using usb
2. Set your IPv4 to "192.168.0.21" , subnet to "255.255.255.0" and gateway to "192.168.0.1"
3. Open your browser and to url field type: "192.168.0.20"
4. Now you are on Tempy Interface here you can set everything you need for your Tempy borad like:
    * Hostname
    * DHCP on/off
    * IP , Subnet , Gateway
    * SNMP community
    * Username and password for this interface

## License

MIT


