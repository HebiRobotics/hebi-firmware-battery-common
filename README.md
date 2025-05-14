# hebi-firmware-battery-common
This repo contains common driver files for HEBI Robotics' open-source Wattman BIB.

## Getting Started
This firmware was designed with VSCode, WSL, and the ARM GNU Toolchain. The Toolchain Setup Guide can be found [here](toolchain/toolchain.md).

## Project Background
The goal of this project is to develop an open-source smart battery solution for robotics applications. 

We've searched far and wide for the ideal battery. Our favorite battery was previously Grin Technologies' [LIGO battery](https://ebikes.ca/product-info/retired/ligo-batteries.html). Sadly, this product line has was discontinued in 2022 - so we had to develop our own solution.

We took what we liked and didn't like about various different battery solutions to come up with the following design crtieria:
- 36V nominal (10S) battery
- Each battery pack under 100Wh to allow for ease of travel and shipping 
- Integrated BMS in every battery pack
- I2C communications with a battery gauge chip
- Built-in output enable/disable circuitry
- "Long and Skinny" form factor
- Stackable
- High power density

The Wattman battery pack has been powering HEBI's internal development since 2022! We initially released the battery with a simple battery cap that contains an on/off pushbutton controller. This works well for most applications but has its limitations. When batteries are wired in parallel or not easily accessible it's helpful to have more data and control over the system. To address this we've developed the Wattman "Battery Interface Board" (or BIB for short). The BIB enables us to gather rich information about current draw, state of charge, temperature, and more for every battery in a system.

### Main Features
- Modular architecture (run up to 253 batteries in parallel!)
- Low Power STM32L4 microcontroller
- CAN 2.0B communication interface
- Rich feedback with an RGB LED and buzzer
- Separate Charge / Discharge control
- Battery fault recovery capabilities
- BQ34Z100 data readout (Current, Voltage, State of Charge, etc.)
- Reverse polarity protection
- Integration with the HEBI Platform through the use of the Battery Controller

### Planned Features
- Bootloader
- Robust hot swapping support
- Smart fault detection and recovery

## More Info
- [ECAD Files](https://github.com/HebiRobotics/hebi-electrical-public/tree/main/PCBA-2121-01%20Battery%20Cap%20Power%20Controller)
- [CAN Protocol](https://github.com/HebiRobotics/hebi-firmware-battery-protocol)
- [Application Source](https://github.com/HebiRobotics/hebi-firmware-battery)
- [Bootloader Source](https://github.com/HebiRobotics/hebi-firmware-battery-bootloader)