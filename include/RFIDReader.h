#ifndef RFID_READER_H
#define RFID_READER_H

#include <Arduino.h>
#include "Wiegand.h"
#include <WiFi.h>

/**
 * RFIDReader class.
 * Handles the reading of RFID tags using the Wiegand 26 protocol.
 * This class is designed as a singleton to ensure only one instance manages the RFID hardware.
 * 
 * Note: The Wiegand 26 protocol has a fast 25ms frame time. Therefore, pinning this task to a dedicated
 * CPU core is desirable for maintaining responsiveness and ensuring reliable tag reads. This approach 
 * helps to manage the time-sensitive nature of the Wiegand protocol without interference from other 
 * tasks that the ESP32 is handling concurrently.
 */
class RFIDReader {
    public:
        /**
         * Gets the singleton instance of the RFIDReader class.
         * Ensures that only one RFIDReader instance is used throughout the application.
         *
         * @param client The WiFi client, used for network communications if necessary.
         * @return A pointer to the singleton RFIDReader instance.
         */
        static RFIDReader* getInstance(WiFiClient& client);

        /**
         * Main loop function to be called regularly.
         * Checks for new RFID tags and processes them if available.
         */
        void loop();

        /**
         * Adjusts the delay between RFID tag reads.
         * This is used to throttle the read rate based on configuration in Utilities.
         *
         * @param newDelay The new delay duration in milliseconds.
         */
        void adjustDelay(unsigned long newDelay);
    private:
        /**
         * Private constructor for the RFIDReader class.
         * Part of the singleton pattern, ensuring only one instance is created.
         *
         * @param client Reference to a WiFiClient for network communications.
         */
        RFIDReader(WiFiClient& client);

        // Singleton instance of the RFIDReader class.
        static RFIDReader* instance;

        // Wiegand interface for RFID reading.
        WIEGAND wg;

        // Reference to a WiFiClient for potential network communication.
        WiFiClient& wifiClient;

        // Delay between RFID reads to avoid rapid re-reading of the same tag.
        unsigned long readDelay = 0;

        /**
         * Handles the event of an RFID tag being read.
         * This function is called when a new RFID tag is detected.
         *
         * @param tagId The ID of the read RFID tag.
         */
        void handleTagRead(const uint32_t& tagId);
};

#endif // RFID_READER_H
