Cherry Picker Board
===================

This collection of examples show how we can aggregate and augment the base board classes to reflect the physical connections of the MCU to devices.

The CherryPicker was a robot designed to pick allegorical fruits during a robotics contest - and served as testing ground for the STpp library.

(insert diagram here)

The starting point is the CherryPickerBoard class, which is a STM32 discovery board plugged onto some external devices. Most of these devices have testing examples (such as AX12 which tests a bus of AX12 servomotors).

The following examples / tests are available:
 * AX12: tests the chain of AX12 servomotors
 * BacAFruits: sequence of actions on a chain of AX12 servomotors
 * BacAFruitsBT: illustrates the use of a zigbee controller to remote-control the opening and closing of the BacAFruits
 * Lcd: illustrate the use of a LCD display. Not functional
 * nfc: example of zigbee communication. Shorthand for Not F**cing Connecting
 * shell: shows how the various hardware elements can be exposed through an interactive shell
 * srf02: role unknown
