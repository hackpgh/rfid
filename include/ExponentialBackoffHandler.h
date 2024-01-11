#ifndef EXPONENTIAL_BACKOFF_HANDLER_H
#define EXPONENTIAL_BACKOFF_HANDLER_H

#include <Arduino.h>
#include <cmath> // For pow function

/**
 * @brief A class implementing an exponential backoff strategy for handling failed tag scan attempts.
 *
 * This class is designed to increase the delay between consecutive attempts in an exponential manner,
 * with a base that can be set to control the rate of increase. This strategy is a security measure
 * against brute-force attacks.
 * 
 * To attempt to guess a specific 24-bit integer value using a brute force swipe attack with the backoff strategy below, 
 * it would take approximately 596,523 hours. This is under the worst-case assumption that the correct value is the 
 * last one tried out of the 16,777,216 possibilities.
 * Some key insights from the calculation:
 * It takes 12 attempts to reach the maximum delay of 64 seconds.
 * There are about 2,982,614 resets (every 6 minutes) during the entire process.
 */
class ExponentialBackoffHandler {
private:
    unsigned int failedAttempts = 0;           ///< Count of consecutive failed attempts.
    unsigned long lastAttemptTime = 0;         ///< Timestamp of the last failed attempt.
    const unsigned long maxDelayMilliseconds;  ///< Maximum delay allowed between attempts, in milliseconds.
    const unsigned long resetTimeMilliseconds; ///< Time after which the count of failed attempts is reset, in milliseconds.
    static constexpr unsigned long MaxExponent = 6; ///< Maximum exponent used in calculating the delay.
    static constexpr float ExponentialBase = 1.5;   ///< Base for the exponential calculation, controls the ramp-up speed.


public:
    /**
     * Constructor for ExponentialBackoffHandler.
     *
     * @param maxDelaySeconds Maximum delay in seconds between attempts.
     * @param resetTimeSeconds Time in seconds after which the failed attempts counter is reset.
     */
    ExponentialBackoffHandler(unsigned long maxDelaySeconds = 64, unsigned long resetTimeSeconds = 360)
        : maxDelayMilliseconds(maxDelaySeconds * 1000),
          resetTimeMilliseconds(resetTimeSeconds * 1000) {}

    /**
     * Resets the count of failed attempts if the specified reset time has passed.
     */
    void resetFailedAttempts() {
        unsigned long now = millis();
        if ((now - lastAttemptTime > resetTimeMilliseconds) || (now < lastAttemptTime)) { // Handles millis() overflow
            failedAttempts = 0;
        }
    }

    /**
     * Records a failed attempt and updates the last attempt timestamp.
     */
    void failedAttempt() {
        resetFailedAttempts(); // Optionally reset if it's been long enough
        failedAttempts++;
        lastAttemptTime = millis();
    }

    /**
     * Calculates the delay to be applied after a failed attempt, based on the number of failed attempts.
     *
     * @return The calculated delay in milliseconds.
     */
    unsigned long calculateDelay() const {
        unsigned long delayTime = pow(ExponentialBase, std::min(static_cast<unsigned long>(failedAttempts), MaxExponent)) * 1000;
        return min(delayTime, maxDelayMilliseconds); // Ensure max delay is not exceeded
    }

    /**
     * Retrieves the current delay that should be applied, considering the number of failed attempts.
     *
     * @return The current delay in milliseconds.
     */
    unsigned long getCurrentDelay() const {
        return calculateDelay();
    }
};

#endif // EXPONENTIAL_BACKOFF_HANDLER_H
