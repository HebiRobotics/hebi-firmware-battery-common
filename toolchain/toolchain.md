# Firmware Toolchain
These instructions are written for a WSL / VSCode development environment.

# Toolchain WSL Installation Instructions
1. [Install WSL](https://docs.microsoft.com/en-us/windows/wsl/install)
2. Open WSL
3. Run `sudo apt-get update`
4. Install [GNU make](https://www.gnu.org/software/make/manual/make.html) with `sudo apt install make`
5. Install the [ARM GNU Toolchain](https://learn.arm.com/install-guides/gcc/arm-gnu/) with `sudo apt install gcc-arm-none-eabi`
6. Follow [this guide](https://code.visualstudio.com/docs/cpp/config-wsl) to use WSL with VS Code

## Recommended Extensions for VSCode
- [C/C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack)
- [Makefile Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.makefile-tools)
- [Remote WSL](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-wsl)
- [Cortex Debug](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug)

### Makefile Tools Extension Setup
1. Install the [Makefile Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.makefile-tools) extension in VSCode
2. Copy the example `c_cpp_properties.json` and `settings.json` files into your project
3. Navigate to the Makefile extension menu, select your target + configuration, and run `Makefile: Build the Current Target`
4. The Makefile tools extension should now populate the correct paths for Intellisense

## Cortex Debug Extension Setup
1. Install OpenOCD
    1. [`apt-get install openocd`](https://openocd.org/pages/getting-openocd.html)
    2. Copy the udev rules file to your udev directory `sudo cp 60-openocd.rules /etc/udev/rules.d`
    3. Restart the udev service `sudo service udev restart`
    4. Reload udev permissions `sudo udevadm control --reload-rules && sudo udevadm trigger`
2. Install [USBIPD](https://learn.microsoft.com/en-us/windows/wsl/connect-usb) to enable USB usage in WSL
    1. Install the latest version of [usbipd-win(https://github.com/dorssel/usbipd-win/releases)] on *Windows*
    2. Run the following commands on *Linux*: `sudo apt install linux-tools-generic hwdata` `sudo update-alternatives --install /usr/local/bin/usbip usbip /usr/lib/linux-tools/*-generic/usbip 20`
    3. Open a command line on Windows with administrator priveleges
    4. Run `usbipd wsl list` to find the debugger probe
    5. Run `usbipd wsl attach --busid <busid>` to attach the probe to usbipd (or `usbipd wsl attach --hardware-id 0483:3748` for the STLink V2)
    6. Check that the device is bound in *Linux* with the following command: `lsusb`
3. Run OpenOCD
    1. You can test openocd with this command: `openocd -f interface/stlink.cfg -f target/stm32f7x.cfg -c "program <binary-file-location> verify reset exit 0x08000000"`. Even with an invalid binary file it will attempt to connect to the chip through the debugger probe.
    2. Troubleshooting
        - In newer versions of Ubuntu you may need to run `sudo apt-get install libncurses5`
4. Install the [Cortex Debug](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug) extension in VSCode
    1. Copy the example `launch.json` file into your VSCode project
5. Disable flash read out protection on your target device. Program your device with the *test bootloader*. This is very important - read out protection will prevent the debugger from functioning properly.
6. Set the key / info for the device and bootload it normally
7. You should now be able to attach to the running device and use the debugger

## Additional Notes
https://learn.microsoft.com/en-us/windows/wsl/connect-usb
https://stackoverflow.com/questions/41066653/openocd-error-libusb-open-failed-with-libusb-error-not-supported
Must disable readout protection for debugging - use test bootloader