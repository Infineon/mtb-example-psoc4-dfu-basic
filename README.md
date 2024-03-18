# PSoC&trade; 4: Basic device firmware upgrade (DFU)

This example demonstrates how to create a field upgradable project for PSoC&trade; 4 using the DFU middleware.
The device firmware update (DFU) will occur over the I2C or UART interfaces. This includes downloading an application from a host device (PC/MCU) and installing it in PSoC&trade; 4 device flash.
 This document details the following:
1. A high-level overview of the DFU process described in [Design and implementation](#design-and-implementation) section.
2. An example to explain the installation of DFU firmware inside the flash of the target device and upload an application code via the I2C or UART interface.

This example bundles two applications:

**Bootloader app:** Implements a [DFU middleware library](https://github.com/Infineon/dfu)-based basic bootloader application run by the CPU. The bootloader handles the image download, verification, erase, and upgrade process. When the image is valid, the bootloader lets the CPU boot the application.

**Blinky app:** A tiny CPU-run application that continuously blinks an LED at a 5 Hz rate. This application transfers control to the bootloader when you press the user button. Here, the application project is a simple Blinky project, but you can change it according to your requirements.

> **Note:** For `CY8CKIT-040T` and `CY8CPROTO-040T`, only I2C is available as the DFU transport layer. For `CY8CKIT-040T`, switching between the application and the bootloader is controlled by an I2C command rather than a button press.

[View this README on GitHub.](https://github.com/Infineon/mtb-example-psoc4-dfu-basic)

[Provide feedback on this code example.](https://cypress.co1.qualtrics.com/jfe/form/SV_1NTns53sK2yiljn?Q_EED=eyJVbmlxdWUgRG9jIElkIjoiQ0UyMzYzOTUiLCJTcGVjIE51bWJlciI6IjAwMi0zNjM5NSIsIkRvYyBUaXRsZSI6IlBTb0MmdHJhZGU7IDQ6IEJhc2ljIGRldmljZSBmaXJtd2FyZSB1cGdyYWRlIChERlUpIiwicmlkIjoiZGFzYXZpaml0IiwiRG9jIHZlcnNpb24iOiIyLjAuMCIsIkRvYyBMYW5ndWFnZSI6IkVuZ2xpc2giLCJEb2MgRGl2aXNpb24iOiJNQ0QiLCJEb2MgQlUiOiJJQ1ciLCJEb2MgRmFtaWx5IjoiUFNPQyJ9)

## Requirements

- [ModusToolbox&trade;](https://www.infineon.com/modustoolbox) v3.2 or later (tested with v3.2)
- Board support package (BSP) minimum required version: 3.2.0

- Programming language: C

- Associated parts: All [PSoC&trade; 4 MCU](https://www.infineon.com/cms/en/product/microcontroller/32-bit-psoc-arm-cortex-microcontroller/psoc-4-32-bit-arm-cortex-m0-mcu/) parts

## Supported toolchains (make variable 'TOOLCHAIN')

- GNU Arm&reg; Embedded Compiler v11.3.1 (`GCC_ARM`) – Default value of `TOOLCHAIN`
- Arm&reg; Compiler v6.16 (`ARM`)
- IAR C/C++ Compiler v9.30.1 (`IAR`)

## Supported kits (make variable 'TARGET')

- [PSoC&trade; 4100S Max Pioneer Kit](https://www.infineon.com/CY8CKIT-041S-MAX) (`CY8CKIT-041S-MAX`) - Default value of `TARGET`
- [PSoC&trade; 4100S Plus Prototyping Kit](https://www.infineon.com/CY8CKIT-149) (`CY8CKIT-149`)
- [PSoC&trade; 4000S CAPSENSE&trade; Prototyping Kit](https://www.infineon.com/CY8CKIT-145-40XX) (`CY8CKIT-145-40XX`)
- [PSoC&trade; 4500S Pioneer Kit](https://www.infineon.com/CY8CKIT-045S) (`CY8CKIT-045S`)
- [PSoC&trade; 4000T CAPSENSE&trade; Evaluation Kit](https://www.infineon.com/CY8CKIT-040T) (`CY8CKIT-040T`)
- [PSoC&trade; 4000T CAPSENSE&trade; Prototyping Kit](https://www.infineon.com/CY8CPROTO-040T) (`CY8CPROTO-040T`)


## Supported transports (make variable 'TRANSPORT_OPT')

I2C is the default transport. See the [Operation](#operation) section for instructions to change the transport option,  `TRANSPORT_OPT`.

 TARGET | UART | I2C 
 :----- | :--- | :--- 
 CY8CKIT-041S-MAX | Yes | Yes 
 CY8CKIT-149 | Yes | Yes 
 CY8CKIT-145-40XX | Yes | Yes 
 CY8CKIT-045S | Yes | Yes 
 CY8CKIT-040T | No  | Yes 
 CY8CPROTO-040T | No  | Yes 

<br>


## Hardware setup

This example uses the board's default configuration for the kits listed above, except for the CY8CPROTO-040T kit. For the CY8CPROTO-040T kit, make the following hardware changes:

   - Change the SW2 switch towards I2C on the kit to use I2C functionality.
   - Short J6.2 and J6.3 with the jumper shunt on the kit to use the user button.

See the kit user guide to ensure that the board is configured correctly.

> **Note:** Some of the PSoC&trade; 4 kits are shipped with KitProg2 installed. ModusToolbox&trade; requires KitProg3. Before using this code example, make sure the board is upgraded to KitProg3. The tool and instructions are available in the [Firmware Loader](https://github.com/Infineon/Firmware-loader) GitHub repository. If you do not upgrade, you will see an error like "unable to find CMSIS-DAP device" or "KitProg firmware is out of date".

## Software setup

See the [ModusToolbox&trade; tools package installation guide](https://www.infineon.com/ModusToolboxInstallguide) for information on installing and configuring the tools package.

This example requires no additional software or tools.

## Using the code example

### Create the project

The ModusToolbox&trade; tools package provides the Project Creator as both a GUI tool and a command line tool.

<details><summary><b>Use Project Creator GUI</b></summary>

1. Open the Project Creator GUI tool.

   There are several ways to do this, including launching it from the dashboard or from inside the Eclipse IDE. For more details, see the [Project Creator user guide](https://www.infineon.com/ModusToolboxProjectCreator) (locally available at *{ModusToolbox&trade; install directory}/tools_{version}/project-creator/docs/project-creator.pdf*).

2. On the **Choose Board Support Package (BSP)** page, select a kit supported by this code example. See [Supported kits](#supported-kits-make-variable-target).
   > **Note:** To use this code example for a kit not listed here, you may need to update the source files. If the kit does not have the required resources, the application may not work.

3. On the **Select Application** page:

   a. Select the **Applications(s) Root Path** and the **Target IDE**.

   > **Note:** Depending on how you open the Project Creator tool, these fields may be pre-selected for you.

   b.	Select this code example from the list by enabling its check box.

   > **Note:** You can narrow the list of displayed examples by typing in the filter box.

   c. (Optional) Change the suggested **New Application Name** and **New BSP Name**.

   d. Click **Create** to complete the application creation process.

</details>

<details><summary><b>Use Project Creator CLI</b></summary>

The 'project-creator-cli' tool can be used to create applications from a CLI terminal or from within batch files or shell scripts. This tool is available in the *{ModusToolbox&trade; install directory}/tools_{version}/project-creator/* directory.

Use a CLI terminal to invoke the 'project-creator-cli' tool. On Windows, use the command-line 'modus-shell' program provided in the ModusToolbox&trade; installation instead of a standard Windows command-line application. This shell provides access to all ModusToolbox&trade; tools. You can access it by typing "modus-shell" in the search box in the Windows menu. In Linux and macOS, you can use any terminal application.

The following example clones the "[mtb-example-psoc4-dfu-basic](https://github.com/Infineon/mtb-example-psoc4-dfu-basic)" application with the desired name "MyDFU" configured for the *CY8CKIT-041S-MAX* BSP into the specified working directory, *C:/mtb_projects*:

   ```
   project-creator-cli --board-id CY8CKIT-041S-MAX --app-id mtb-example-psoc4-dfu-basic --user-app-name MyDFU --target-dir "C:/mtb_projects"
   ```

The 'project-creator-cli' tool has the following arguments:

Argument | Description | Required/optional
---------|-------------|-----------
`--board-id` | Defined in the <id> field of the [BSP](https://github.com/Infineon?q=bsp-manifest&type=&language=&sort=) manifest | Required
`--app-id`   | Defined in the <id> field of the [CE](https://github.com/Infineon?q=ce-manifest&type=&language=&sort=) manifest | Required
`--target-dir`| Specify the directory in which the application is to be created if you prefer not to use the default current working directory | Optional
`--user-app-name`| Specify the name of the application if you prefer to have a name other than the example's default name | Optional

> **Note:** The project-creator-cli tool uses the `git clone` and `make getlibs` commands to fetch the repository and import the required libraries. For details, see the "Project creator tools" section of the [ModusToolbox&trade; tools package user guide](https://www.infineon.com/ModusToolboxUserGuide) (locally available at {ModusToolbox&trade; install directory}/docs_{version}/mtb_user_guide.pdf).


</details>

### Open the project

After the project has been created, you can open it in your preferred development environment.

<details><summary><b>Eclipse IDE</b></summary>


If you opened the Project Creator tool from the included Eclipse IDE, the project will open in Eclipse automatically.

For more details, see the [Eclipse IDE for ModusToolbox&trade; user guide](https://www.infineon.com/MTBEclipseIDEUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_ide_user_guide.pdf*).

</details>


<details><summary><b>Visual Studio (VS) Code</b></summary>

Launch VS Code manually, and then open the generated *{project-name}.code-workspace* file located in the project directory.

For more details, see the [Visual Studio Code for ModusToolbox&trade; user guide](https://www.infineon.com/MTBVSCodeUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_vscode_user_guide.pdf*).

</details>


<details><summary><b>Keil µVision</b></summary>

Double-click the generated *{project-name}.cprj* file to launch the Keil µVision IDE.

For more details, see the [Keil µVision for ModusToolbox&trade; user guide](https://www.infineon.com/MTBuVisionUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_uvision_user_guide.pdf*).

</details>

<details><summary><b>IAR Embedded Workbench</b></summary>

Open IAR Embedded Workbench manually, and create a new project. Then select the generated *{project-name}.ipcf* file located in the project directory.

For more details, see the [IAR Embedded Workbench for ModusToolbox&trade; user guide](https://www.infineon.com/MTBIARUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_iar_user_guide.pdf*).

</details>

<details><summary><b>Command line</b></summary>


If you prefer to use the CLI, open the appropriate terminal, and navigate to the project directory. On Windows, use the command-line 'modus-shell' program; on Linux and macOS, you can use any terminal application. From there, you can run various `make` commands.

For more details, see the [ModusToolbox&trade; tools package user guide](https://www.infineon.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mtb_user_guide.pdf*).

</details>

The project has the necessary settings by default. The **Operation** section describes how to test the example. 

## Operation

1. Connect the board to the PC using the provided USB cable through the KitProg3 USB connector.

2. Select 'mtb-example-psoc4-dfu-basic' and click the **Build Application** as shown in **Figure 1** to build a project to bundle the bootloader and the Blinky application together.

   **Figure 1. Building the application**  

   <img src="images/figure_1.png" alt="Figure_1" width="1024"/>

   > **Note:** I2C is configured as the default DFU transport. To change the DFU transport, edit the **common.mk** makefile to set `TRANSPORT_OPT?=<transport>` before building the project, as shown in **Figure 2**.

   **Figure 2. TRANSPORT_OPT**  

   <img src="images/figure_2.png" alt="Figure_2" width="1024"/>

3. Flash the bootloader
   <details><summary><b>Using Eclipse IDE</b></summary>

      1. Select the bootloader_cm0p project (*mtb-example-psoc4-dfu-basic.bootloader_cm0p*) in the Project Explorer.

      2. In the **Quick Panel**, scroll down to launches and click **mtb-example-psoc4-dfu-basic.bootloader_cm0p Program (KitProg3_MiniProg4)**.

      **Figure 3. Flashing the bootloader** 

      <img src="images/figure_3.png" alt="Figure_3" width="1024"/>

   </details>

   <details><summary><b>In other IDEs</b></summary>

   Follow the instructions in your preferred IDE.
   </details>
   <details><summary><b>Using CLI</b></summary>

     From the terminal, execute the `make program` command to build and program the application using the default toolchain to the default target with default DFU transport. You can specify a target, toolchain and transport manually:
      ```
      make program TOOLCHAIN=<toolchain>
      ```

      Example:
      ```
      make program TOOLCHAIN=GCC_ARM
      ```
      > **Note:** The **TOOLCHAIN** variable is found on the **common.mk** makefile and is changed according to the user. However, the user has to provide the compiler path *CY_COMPILER_PATH=* in the makefiles of both the bootloader and the Blinky application.
   </details>

4. After programming, the bootloader restarts, making the user LED blink at approximately 1 Hz.

5. Perform the device firmware upgrade using the **DFU Host Tool**:

   a. Open the [Device Firmware Upgrade Host Tool](https://www.infineon.com/dgdlac/Infineon-ModusToolbox_Device_Firmware_Update_Host_Tool_%28Version_1.2%29-Software-v01_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f8d42ba76bb) under the **Tools** section in the **Quick Panel**. Plug in the kit.
      
      **Figure 4. Device Firmware Upgrade Host Tool**  

      <img src="images/figure_4.png" alt="Figure_4" width="1024"/>

   b. Make a note of the path to the **blinky_cm0p_crc.cyad2** file. You need it to configure the DFU Host Tool. By default, it will be generated in the *mtb-example-psoc4-dfu-basic/blinky_cm0p/build/APP_CY8CKIT-041S-MAX/Debug* directory. Paste this path in the *File* field of the DFU Host Tool.

      > **Note:** CY8CKIT-041S-MAX is the target kit in this example. If you are using a different kit, the name of the *APP_xxx* folder inside the _build_ folder will change accordingly.

      **Figure 5. Locating the \*.cyacd2 file**  

      <img src="images/figure_5.png" alt="Figure_5" width="1024"/>  

   c. Select the appropriate port based on the transport (`TRANSPORT_OPT`) configured in the *common.mk* makefile. 
   I2C is the default transport configuration and the default parameters are:
   - I2C speed = 400 kHz
   - I2C address = 12

   If you are using UART as the transport, the default parameters are:
   - Baud = 115200
   - Data  bits = 8
   - Stop bits = 1
   - Parity = None

   **Figure 6. Setting up the DFU Host Tool**  

   <img src="images/figure_6.png" alt="Figure_6" width="773"/>

   d. Click **Program** and observe the image download progress status on the progress bar, and wait for the download to complete.
      
      **Figure 7. Uploading the application firmware**               

    <img src="images/figure_7.png" alt="Figure_7" width="773"/>

      > **Note:** See the [DFU Host Tool](https://www.infineon.com/dgdlac/Infineon-ModusToolbox_Device_Firmware_Update_Host_Tool_%28Version_1.2%29-Software-v01_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f8d42ba76bb) documentation for further details on selecting a port and configuring it for communication based on the transport enabled in the bootloader.

   After you download the application firmware, the device starts to boot in the *blinky_cm0p* project. Observe that the user LED blinks at 5 Hz.

### Troubleshooting

   1. **Issue:**  DFU Host Tool actions failing for the PSoC&trade; 4000T WLCSP devices.
   
      For PSoC&trade; 4000T devices featuring a factory-programmed bootloader within the WLCSP package, it is essential to use a unique product ID as detailed in the *WLCSP Bootloader Package* section of the [PSoC&trade; 4000T device datasheet](https://www.infineon.com/002-33949). To ensure successful firmware flashing, a correct product ID must be assigned to the `__cy_product_id` variable for the GCC_ARM and IAR compilers within the linker scripts, or as the macro `#define CY_PRODUCT_ID` in the *dfu_common.h* file if you are using the ARM compiler. Look for section **Memory layout** of this document for locating the linker scripts.

   2. **Issue:**  DFU Host Tool actions failing after updating the bootloader.
   
      The device might be previously programmed with a valid application and on reset, the bootloader boots to its previous application. To solve this issue, erase the entire device flash by running the ```make erase``` command in the terminal, as shown in **Figure 8**, and repeat from Step 3 of the previous section. <br>

         **Figure 8. Erasing the device chip**   

         <img src="images/figure_8.png" alt="Figure_8" width="773"/>
<br>

### Switching between the bootloader and the application

**For kits other than `CY8CKIT-040T`**

Dynamic switching between the bootloader and the application is triggered by the user button events. Press the user button to switch to the bootloader while the application is running.Similarly, press the user button to switch to the application while the bootloader is running. Note that the bootloader accepts the switching request if there is a valid application in the memory.

**For `CY8CKIT-040T` kit**

Dynamic switching between the bootloader and the application is triggered by sending commands over I2C. To send the commands, a USB-to-I2C bridge and a I2C serial terminal software like **Bridge Control Panel** (BCP) is used. BCP is included with the installation of [PSoC&trade; Programmer](https://www.infineon.com/cms/en/design-support/tools/programming-testing/psoc-programming-solutions/). To know how to write data using BCP, see the **Help Topics** section under the **Help** tab in the menu bar. The CY8CKIT-040T kit has an onboard KitProg which acts as the USB-to-I2C bridge and it is connected to the I2C bus of the PSoC&trade; 4000T device.

> **Note:** **Bridge Control Panel** (BCP) is currently supported only on Windows.


To switch to the bootloader while the application is running, send the following 3 bytes to the I2C address `0x0C`, using BCP, as shown in **Figure 9**.
```
0x01 0xEA 0x17
```
   **Figure 9. BCP command 1**  

   <img src="images/figure_9.png" alt="Figure_9" width="839"/>

The command follows a basic packet structure that consists of Start Of Packet (SOP): 0x01, Command Byte (this is the actual command to trigger a flag in the main.c): 0xEA, and End Of Packet (EOP): 0x17.

Send the following 7 bytes to the I2C address `0x0C` to switch from the bootloader to the application.
```
0x01 0x3B 0x00 0x00 0xC4 0xFF 0x17
```
   **Figure 10. BCP command 2**  

   <img src="images/figure_10.png" alt="Figure_10" width="839"/>

 The **Exit Bootloader** command is an integrated command given by the DFU MW. The command details and the packet structure is found in the host **command/response** protocol section of the application note [AN236282 - Device firmware update (DFU) middleware (MW) FOR ModusToolbox&trade;](https://www.infineon.com/AN236282).

  Note that the bootloader accepts the switching request to jump to an application only if there is a valid application in the memory.


## Debugging

You can debug the example to step through the code.


<details><summary><b>In Eclipse IDE</b></summary>

Use the **\<Application Name> Debug (KitProg3_MiniProg4)** configuration in the **Quick Panel**. For details, see the **Program and debug** section in the [Eclipse IDE for ModusToolbox&trade; user guide](https://www.infineon.com/MTBEclipseIDEUserGuide).

</details>


<details><summary><b>In other IDEs</b></summary>

Follow the instructions in your preferred IDE.
</details>


## Design and implementation

### Overview

This example demonstrates the basic device firmware upgrade (DFU) operations based on the [DFU middleware library](https://github.com/Infineon/dfu).

   **Figure 11. Bootloader flow diagram**

   <img src="images/bootloader_flow.png" alt="Figure_11" width=""/>



### Bootloader implementation

This bootloader_cm0p project implements an immutable bootloader with support to upgrade the application firmware over I2C or UART interface.

The [DFU Host Tool](https://www.infineon.com/dgdlac/Infineon-ModusToolbox_Device_Firmware_Update_Host_Tool_%28Version_1.2%29-Software-v01_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f8d42ba76bb) (typically running on the host PC) sends the application (*\<APPNAME>.cyacd2*) to the device. The bootloader receives the application image in chunks and overwrites the existing application image. See **Figure 11**. If a DFU session is interrupted, the application firmware will not be in a usable state. However, the device can still run the bootloader and perform necessary actions to download and install the application in the subsequent DFU sessions.

**Figure 12. DFU design overview**

<img src="images/dfu-overwrite.png" alt="Figure_12" width="839"/>



### DFU interfaces

The project supports I2C or UART interfaces to communicate with the DFU host. The *config* directory in the *mtb-example-psoc4-dfu-basic/bsps/TARGET_APP_CY8CKIT-041S-MAX/config* includes all the necessary configurations to select the supported interfaces. These configurations can also be edited or reconfigured using the **Device Configurator** under the *BSP Configurators* in the **Quick Panel**.

> **Note:** The Device Configurator is common for both the bootloader and the application, therefore, the resources are shared among them.

   **Figure 13. Device Configurator**  
   <img src="images/figure_13.png" alt="Figure_13" width="839"/>

See the following table for the default configuration details. You can change the default configurations according to the use case. However, ensure that the configuration of the DFU Host Tool matches with that of the project.

**DFU transport configurations**

  By default, both I2C and UART modes are enabled and configured for transport. For more details on transport resources see [Resources and settings](#resources-and-settings).

**Default I2C configuration**

  Parameter| Default Setting|Description
   :--------------- | :------ | :-----
   Mode  | Slave | Device acts as a slave
   Address | 12  | 7-bit slave device address
   Data rate |400 kbps| DFU supports standard data rates from 50 kbps to 1 Mbps

   <br>

   **Figure 14. I2C configuration**  

   <img src="images/figure_14.png" alt="Figure_14" width="839"/>

   <br>

**Default UART configuration**

   Parameter| Default Setting|Description
   :--------------- | :------ | :-----
   Com Mode | Standard | Standard, SmartCard, and IrDA are supported UART modes in SCB
   Baud rate(bps)|115200 | Supports standard baud rates from 19200 to 115200
   Bit order| LSB first| Standard frame
   Data width| 8 bits| Standard frame
   Parity| None | Standard frame
   Stop bits| 1 bit | Standard frame
   
   <br>
   
   **Figure 15. UART configuration**  

   <img src="images/figure_15.png" alt="Figure_15" width="500"/>
   
   <br>



### Memory layout

The first 16 KB region of the flash is reserved for the bootloader followed by a 4 KB reserved region for future enhancements. Another 32 KB is allocated to the application firmware followed by the empty/reserved region. The size of this empty/reserved region depends on the size of the flash available on the selected target device. See the respective device datasheets for more details. The last 256 B region of the flash is reserved for bootloader metadata.

To change the memory layout or usage, update the respective target linker scripts. The files are found in the *bsps* folder.

For example: *mtb-example-psoc4-dfu-basic/bsps/TARGET_APP_CY8CKIT-041S-MAX/COMPONENT_CM0p/TOOLCHAIN_GCC_ARM*

**Figure 16. Linker scripts**  

   <img src="images/figure_16.png" alt="Figure_16" width="500"/>

There are two linker scripts, one for each application in the corresponding toolchain directory. In the TOOLCHAIN_GCC_ARM directory, the _dfu_cm0p_app0.ld_ linker script is for the bootloader and the _dfu_cm0p_app1.ld_ linker script is for the Blinky application. The linker scripts can also be modified to define dedicated regions of the memory for each application.

The RAM is shared by the bootloader and Blinky application, with a common area used by both projects. The RAM regions must be aligned to the 1 KB boundary because they contain the interrupt vector table remapped at the start.

   **Figure 17. Flash layout of CY8CKIT-041S-MAX**  

   <img src="images/dfu-memory-map.png" alt="Figure_16" width=""/>
   
   <br>

### Software reset

The recommended method of transferring control from one application to another is through a device software reset. This enables each application to initialize device hardware blocks and signal routing from a known state. It is possible to freeze the state of I/O pins so they are maintained through a software reset. Defined portions of the SRAM are also maintained through a software reset. For more information, see the device-specific  reference manual.


### Resources and settings

**Table 1. Bootloader resources**

 Resource  |  Alias/object     |    Purpose
 :------- | :------------    | :------------
 SCB (I2C) (PDL) | DFU_I2C          | I2C slave driver to communicate with the DFU host
 SCB (UART)(PDL) | DFU_UART          | UART driver to communicate with the DFU host
 GPIO (PDL)    | CYBSP_USER_LED         | User LED
 GPIO (PDL)    | CYBSP_USER_BTN         | User button

<br>

> **Note:** The *Alias/Name* must be DFU_\<TRANSPORT_OPT> to support DFU communication API. Like DFU_I2C for I2C and DFU_UART for UART.
<br>

**Table 2. Application resources**

 Resource  |  Alias/object     |    Purpose
 :------- | :------------    | :------------
 GPIO (PDL)    | CYBSP_USER_LED         | User LED
 GPIO (PDL)    | CYBSP_USER_BTN         | User button

<br>

## Related resources

Resources  | Links
-----------|----------------------------------
Application notes  | [AN79953](https://www.infineon.com/AN79953) – Getting started with PSoC&trade; 4
Code examples  | [Using ModusToolbox&trade;](https://github.com/Infineon/Code-Examples-for-ModusToolbox-Software) on GitHub 
Device documentation | [PSoC&trade; 4 datasheets](https://www.infineon.com/cms/en/search.html#!view=downloads&term=psoc4&doc_group=Data%20Sheet) <br>[PSoC&trade; 4 technical reference manuals](https://www.infineon.com/cms/en/search.html#!view=downloads&term=psoc4&doc_group=Additional%20Technical%20Information)
Development kits | Select your kits from the [Evaluation board finder](https://www.infineon.com/cms/en/design-support/finder-selection-tools/product-finder/evaluation-board)
Libraries on GitHub | [mtb-pdl-cat2](https://github.com/Infineon/mtb-pdl-cat2) – PSoC&trade; 4 Peripheral Driver Library (PDL)<br> [mtb-hal-cat2](https://github.com/Infineon/mtb-hal-cat2) – Hardware Abstraction Layer (HAL) library
Middleware on GitHub  | [Device Firmware Update (DFU) Middleware Library](https://github.com/Infineon/dfu) - Links to DFU SDK middleware
Tools  | [ModusToolbox&trade;](https://www.infineon.com/modustoolbox) – ModusToolbox&trade; software is a collection of easy-to-use libraries and tools enabling rapid development with Infineon MCUs for applications ranging from wireless and cloud-connected systems, edge AI/ML, embedded sense and control, to wired USB connectivity using PSoC&trade; Industrial/IoT MCUs, AIROC&trade; Wi-Fi and Bluetooth&reg; connectivity devices, XMC&trade; Industrial MCUs, and EZ-USB&trade;/EZ-PD&trade; wired connectivity controllers. ModusToolbox&trade; incorporates a comprehensive set of BSPs, HAL, libraries, configuration tools, and provides support for industry-standard IDEs to fast-track your embedded application development.

<br>

## Other resources

Infineon provides a wealth of data at [www.infineon.com](https://www.infineon.com) to help you select the right device, and quickly and effectively integrate it into your design.

## Document history

Document title: *CE236395* - *PSoC&trade; 4: Basic device firmware upgrade (DFU)*

 Version | Description of change
 ------- | ---------------------
 1.0.0   | New code example
 1.1.0   | Updated Linker files
 1.1.1   | Minor README update
 1.2.0   | Added support for CY8CPROTO-040T and updated to support ModusToolbox&reg; v3.1
 2.0.0   | README template update and bug fix in Makefile
 
<br>

---------------------------------------------------------

© Cypress Semiconductor Corporation, 2023-2024. This document is the property of Cypress Semiconductor Corporation, an Infineon Technologies company, and its affiliates ("Cypress").  This document, including any software or firmware included or referenced in this document ("Software"), is owned by Cypress under the intellectual property laws and treaties of the United States and other countries worldwide.  Cypress reserves all rights under such laws and treaties and does not, except as specifically stated in this paragraph, grant any license under its patents, copyrights, trademarks, or other intellectual property rights.  If the Software is not accompanied by a license agreement and you do not otherwise have a written agreement with Cypress governing the use of the Software, then Cypress hereby grants you a personal, non-exclusive, nontransferable license (without the right to sublicense) (1) under its copyright rights in the Software (a) for Software provided in source code form, to modify and reproduce the Software solely for use with Cypress hardware products, only internally within your organization, and (b) to distribute the Software in binary code form externally to end users (either directly or indirectly through resellers and distributors), solely for use on Cypress hardware product units, and (2) under those claims of Cypress's patents that are infringed by the Software (as provided by Cypress, unmodified) to make, use, distribute, and import the Software solely for use with Cypress hardware products.  Any other use, reproduction, modification, translation, or compilation of the Software is prohibited.
<br>
TO THE EXTENT PERMITTED BY APPLICABLE LAW, CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS DOCUMENT OR ANY SOFTWARE OR ACCOMPANYING HARDWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  No computing device can be absolutely secure.  Therefore, despite security measures implemented in Cypress hardware or software products, Cypress shall have no liability arising out of any security breach, such as unauthorized access to or use of a Cypress product. CYPRESS DOES NOT REPRESENT, WARRANT, OR GUARANTEE THAT CYPRESS PRODUCTS, OR SYSTEMS CREATED USING CYPRESS PRODUCTS, WILL BE FREE FROM CORRUPTION, ATTACK, VIRUSES, INTERFERENCE, HACKING, DATA LOSS OR THEFT, OR OTHER SECURITY INTRUSION (collectively, "Security Breach").  Cypress disclaims any liability relating to any Security Breach, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any Security Breach.  In addition, the products described in these materials may contain design defects or errors known as errata which may cause the product to deviate from published specifications. To the extent permitted by applicable law, Cypress reserves the right to make changes to this document without further notice. Cypress does not assume any liability arising out of the application or use of any product or circuit described in this document. Any information provided in this document, including any sample design information or programming code, is provided only for reference purposes.  It is the responsibility of the user of this document to properly design, program, and test the functionality and safety of any application made of this information and any resulting product.  "High-Risk Device" means any device or system whose failure could cause personal injury, death, or property damage.  Examples of High-Risk Devices are weapons, nuclear installations, surgical implants, and other medical devices.  "Critical Component" means any component of a High-Risk Device whose failure to perform can be reasonably expected to cause, directly or indirectly, the failure of the High-Risk Device, or to affect its safety or effectiveness.  Cypress is not liable, in whole or in part, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any use of a Cypress product as a Critical Component in a High-Risk Device. You shall indemnify and hold Cypress, including its affiliates, and its directors, officers, employees, agents, distributors, and assigns harmless from and against all claims, costs, damages, and expenses, arising out of any claim, including claims for product liability, personal injury or death, or property damage arising from any use of a Cypress product as a Critical Component in a High-Risk Device. Cypress products are not intended or authorized for use as a Critical Component in any High-Risk Device except to the limited extent that (i) Cypress's published data sheet for the product explicitly states Cypress has qualified the product for use in a specific High-Risk Device, or (ii) Cypress has given you advance written authorization to use the product as a Critical Component in the specific High-Risk Device and you have signed a separate indemnification agreement.
<br>
Cypress, the Cypress logo, and combinations thereof, ModusToolbox, PSoC, CAPSENSE, EZ-USB, F-RAM, and TRAVEO are trademarks or registered trademarks of Cypress or a subsidiary of Cypress in the United States or in other countries. For a more complete list of Cypress trademarks, visit www.infineon.com. Other names and brands may be claimed as property of their respective owners.
