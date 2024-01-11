#ifndef DOOR_H
#define DOOR_H

#include <Arduino.h>

/**
 * @brief The `Door` class represents the physical door and its control.
 */
class Door {
private:
    static Door* instance; ///< Singleton instance of the Door class.
    static constexpr int doorLockPin = 27; ///< Pin for controlling the door lock.
    static constexpr int redLightPin = 25; ///< Pin for the red indicator light.
    static constexpr int greenLightPin = 26; ///< Pin for the green indicator light.
    static constexpr unsigned long relayUnlockDuration = 6000; ///< Duration for door unlock relay activation.
    unsigned long lastUnlockTime = 0; ///< Timestamp of the last door unlock.
    bool isDoorLocked = true; ///< Flag indicating whether the door is locked.

    /**
     * @brief Private constructor for the Door class.
     */
    Door();

    /**
     * @brief Turn on the indicator light connected to the specified pin.
     * @param pin The pin to turn on the light.
     */
    void turnOnLight(int pin);

    /**
     * @brief Turn off the indicator light connected to the specified pin.
     * @param pin The pin to turn off the light.
     */
    void turnOffLight(int pin);

public:
    /**
     * @brief Get the singleton instance of the Door class.
     * @return Pointer to the Door instance.
     */
    static Door* getInstance();

    /**
     * @brief Lock the door.
     */
    void lock();

    /**
     * @brief Unlock the door and activate the relay for a specified duration.
     */
    void unlock();

    /**
     * @brief Update the state of the door and indicator lights.
     */
    void update();

    Door(const Door&) = delete; ///< Disable copy constructor.
    Door& operator=(const Door&) = delete; ///< Disable assignment operator.
};

#endif // DOOR_H
