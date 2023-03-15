# Development Environment

We are using nRF Connect SDK with Zephyr RTOS to build our project.

Please follow the Nordic's nRF Connect SDK [getting started guide](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/getting_started.html) on how to install and setup development environment.

We are using the following SDK and Zephyr version
nRF Connect SDK version: 2.3.99
Zephyr version: 3.2.99

# How to build firmware

Once you have the build environment setup, you can simply navigate to `Firmware` folder and compile the code by running `west build -b nrf52dk_nrf52810` command
After a few seconds you should see a message that looks something like this
```
[183/183] Linking C executable zephyr\zephyr.elf
Memory region         Used Size  Region Size  %age Used
           FLASH:      111484 B       192 KB     56.70%
             RAM:       16936 B        24 KB     68.91%
        IDT_LIST:          0 GB         2 KB      0.00%
```
This means you have succesfully compiled the firmware and you are ready to flash it onto your board.

# How to flash firmware

We are using J-Link programmer to flash firmware onto our boards.
Simply connect the J-Link programmer to your board, provide power to the e-ink gadget and run `west flash` to flash latest binary onto your board.


