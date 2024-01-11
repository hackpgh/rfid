
# ERRASv2 (RFID Door Project)

## Project Description

This RFID reading system, powered by an ESP32 microcontroller, is designed for the HackPGH makerspace to authenticate users by reading RFID tags and verifying them against a list of authorized tag IDs from WildApricot. The system utilizes the Wiegand 26 protocol for high responsiveness and reliability in RFID reading. Additionally, it controls a magnetic door lock via a relay connected to the ESP32's GPIO pins. For added security, the system includes a fail-safe mechanism that defaults to keeping the door locked in case of system errors or power failures. The current implementation uses WiFi for network connectivity, with plans to transition to an Ethernet connection for enhanced network stability and performance in the final version.

## System Requirements

-   ESP32 microcontroller (WROOM variant preferred)
-   RFID reader module compatible with the Wiegand 26 protocol
-   Magnetic lock and relay module for door control
-   5v - 3.3v Level shifter to step down the RFID tag scan's signal
-   WiFi network for initial setup (temporary)
-   Ethernet module for final setup (future enhancement)

## Installation and Setup

1.  Assemble the hardware components: Connect the RFID reader and relay module to the ESP32 as per the Wiegand 26 protocol and GPIO specifications.
2.  Flash the provided firmware onto your ESP32.
3.  Configure WiFi credentials in the `main.cpp` file.
4.  (Future) Set up the Ethernet module as per the provided documentation in the future release.

## Usage

-   Power up the ESP32.
-   The system automatically connects to the configured WiFi network.
-   Present an RFID tag to the reader; the system processes the tag and checks it against the authorized list.
-   If authenticated, the system disengages the magnetic lock for six (6) seconds; otherwise, the door remains locked and the exponential backoff delay on the RFID reader is increased.
-   (Future) The system will utilize an Ethernet connection for network functionalities.

## Key Components and Their Roles

-   `RFIDReader`: Manages RFID tag reading using the Wiegand 26 protocol.
-   `Door`: Controls the magnetic door lock mechanism. Checks lock status periodically to ensure the door is appropriately locked
-   `Auth`: Authenticates RFID tags against the authorized list from WildApricot.
-   `Utilities`: Provides logging and time formatting utilities.
-   `ExponentialBackoffHandler`: Handles RFID scan retries with an exponential backoff strategy.

## Future Enhancements

### Ethernet Connection

Transitioning from WiFi to an Ethernet connection for improved network stability and performance, particularly crucial for the time-sensitive RFID reading process.

### Enhanced Security with Self-Hosted Backend Server

Planning to implement a self-hosted backend server as an intermediary between the ESP32 and WildApricot. This will improve security by removing the need to store API credentials on the ESP32 and allowing for more robust security measures.

## Contributing

Contributions to this project are welcome.

## License

This project is licensed under the MIT License. For more details, see the LICENSE file.