#ifndef UTILITIES_H
#define UTILITIES_H

#include <Arduino.h>

/**
 * Utilities class.
 * Provides utility functions for logging and time formatting.
 */
class Utilities {
public:
    /**
     * Logs a message to the serial output.
     * This function is useful for debugging and tracking the flow of the program.
     * TODO: Implement a rolling log with efficient wear leveling strategy
     * 
     * @param message The message to be logged.
     */
    static void log(const String& message);

    /**
     * Gets the current time formatted as a human-readable string.
     * This can be used for timestamping in logs or any other place where
     * time needs to be displayed.
     * 
     * @return A string representing the current time formatted.
     */
    static String getFormattedTime();
};

#endif // UTILITIES_H