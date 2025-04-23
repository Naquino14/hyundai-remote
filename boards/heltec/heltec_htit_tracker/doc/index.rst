.. zephyr:board:: heltec_wifi_lora32_v3

Overview
********

Heltec HTIT-Tracker is a desc desc blah blah by Heltec Automation(TM), it's a blah 
based on ESP32-S3 + SX1262 + UC6580, it has Wi-Fi, BLE, LoRa, and GNSS functions. It also features a Li-Po battery management
system, and a 0.96" TFT OLED. [1]_

The features include the following:

- Microprocessor: ESP32-S3 (dual-core 32-bit MCU + ULP core)
- LoRa node chip SX1262
- USB C interface with a complete voltage regulator, ESD protection, short circuit protection,
  RF shielding, and other protection measures
- Onboard SH1.25-2 battery interface, integrated lithium battery management system
- Integrated WiFi, LoRa, GNSS, Bluetooth three network connections, onboard Wi-Fi, Bluetooth dedicated 2.4GHz
  metal 3D antenna, reserved IPEX (U.FL) interface for LoRa use
- Onboard 0.96-inch 160*80 TFT OLED display
- Integrated CP2102 USB to serial port chip

System requirements
*******************

Prerequisites
=============

Espressif HAL requires WiFi and Bluetooth binary blobs in order work. Run the command
below to retrieve those files.

.. code-block:: console

   west blobs fetch hal_espressif

.. note::

   It is recommended running the command above after :file:`west update`.

Building & Flashing
*******************

Simple boot
===========

The board could be loaded using the single binary image, without 2nd stage bootloader.
It is the default option when building the application without additional configuration.

.. note::

   Simple boot does not provide any security features nor OTA updates.

MCUboot bootloader
==================

User may choose to use MCUboot bootloader instead. In that case the bootloader
must be built (and flashed) at least once.

There are two options to be used when building an application:

1. Sysbuild
2. Manual build

.. note::

   User can select the MCUboot bootloader by adding the following line
   to the board default configuration file.

   .. code:: cfg

      CONFIG_BOOTLOADER_MCUBOOT=y

Sysbuild
========

The sysbuild makes possible to build and flash all necessary images needed to
bootstrap the board with the ESP32-S3 SoC.

To build the sample application using sysbuild use the command:

.. zephyr-app-commands::
   :tool: west
   :zephyr-app: samples/hello_world
   :board: heltec_htit_tracker
   :goals: build
   :west-args: --sysbuild
   :compact:

By default, the ESP32-S3 sysbuild creates bootloader (MCUboot) and application
images. But it can be configured to create other kind of images.

Build directory structure created by sysbuild is different from traditional
Zephyr build. Output is structured by the domain subdirectories:

.. code-block::

  build/
  ├── hello_world
  │   └── zephyr
  │       ├── zephyr.elf
  │       └── zephyr.bin
  ├── mcuboot
  │    └── zephyr
  │       ├── zephyr.elf
  │       └── zephyr.bin
  └── domains.yaml

.. note::

   With ``--sysbuild`` option the bootloader will be re-build and re-flash
   every time the pristine build is used.

For more information about the system build please read the :ref:`sysbuild` documentation.

Manual build
============

During the development cycle, it is intended to build & flash as quickly possible.
For that reason, images can be built one at a time using traditional build.

The instructions following are relevant for both manual build and sysbuild.
The only difference is the structure of the build directory.

.. note::

   Remember that bootloader (MCUboot) needs to be flash at least once.

Build and flash applications as usual (see :ref:`build_an_application` and
:ref:`application_run` for more details).

.. zephyr-app-commands::
   :zephyr-app: samples/hello_world
   :board: heltec_htit_tracker/esp32s3/procpu
   :goals: build

The usual ``flash`` target will work with the ``heltec_htit_tracker`` board
configuration. Here is an example for the :zephyr:code-sample:`hello_world`
application.

.. zephyr-app-commands::
   :zephyr-app: samples/hello_world
   :board: heltec_htit_tracker/esp32s3/procpu
   :goals: flash

Open the serial monitor using the following command:

.. code-block:: shell

   west espressif monitor

After the board has automatically reset and booted, you should see the following
message in the monitor:

.. code-block:: console

   ***** Booting Zephyr OS vx.x.x-xxx-gxxxxxxxxxxxx *****
   Hello World! heltec_htit_tracker

Debugging
*********

As with much custom hardware, the ESP32-S3 modules require patches to
OpenOCD that are not upstreamed yet. Espressif maintains their own fork of
the project. The custom OpenOCD can be obtained at `OpenOCD ESP32`_.

The Zephyr SDK uses a bundled version of OpenOCD by default. You can overwrite that behavior by adding the
``-DOPENOCD=<path/to/bin/openocd> -DOPENOCD_DEFAULT_PATH=<path/to/openocd/share/openocd/scripts>``
parameter when building.

Here is an example for building the :zephyr:code-sample:`hello_world` application.

.. zephyr-app-commands::
   :zephyr-app: samples/hello_world
   :board: heltec_htit_tracker/esp32s3/procpu
   :goals: build flash
   :gen-args: -DOPENOCD=<path/to/bin/openocd> -DOPENOCD_DEFAULT_PATH=<path/to/openocd/share/openocd/scripts>

You can debug an application in the usual way. Here is an example for the :zephyr:code-sample:`hello_world` application.

.. zephyr-app-commands::
   :zephyr-app: samples/hello_world
   :board: heltec_htit_tracker/esp32s3/procpu
   :goals: debug

Utilizing Hardware Features
***************************

Onboard OLED display
====================

The onboard OLED display is of type ``ST7735``, has 160*80 pixels and is
connected via I2C. Foo bar replace me with a better description. 

.. zephyr-app-commands::
   :zephyr-app: samples/subsys/display/lvgl
   :board: heltec_htit_tracker/esp32s3/procpu
   :shield: ssd1306_128x64
   :goals: flash

References
**********

- `Heltec HTIT Tracker Pinout Diagram <https://heltec.org/wp-content/uploads/2023/09/136742f483d1ffbfe67e574286b46e0.png>`_
- `Heltec HTIT Tracker Schematic Diagrams <https://resource.heltec.cn/download/Wireless_Tracker/Wireless_Tacker1.1/HTIT-Tracker_V0.5.pdf>`_
- `ESP32 Toolchain <https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-guides/tools/idf-tools.html#xtensa-esp32-elf>`_
- `esptool documentation <https://github.com/espressif/esptool/blob/master/README.md>`_
- `OpenOCD ESP32 <https://github.com/espressif/openocd-esp32/releases>`_

.. [1] https://heltec.org/project/wireless-tracker/
