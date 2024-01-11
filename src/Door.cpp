#include "Door.h"
#include "Utilities.h"

Door* Door::instance = nullptr;

Door::Door() {
    Utilities::log("[Door] Constructor");
    pinMode(doorLockPin, OUTPUT);
    pinMode(redLightPin, OUTPUT);
    pinMode(greenLightPin, OUTPUT);
    digitalWrite(doorLockPin, HIGH); // Start with the door locked
    digitalWrite(redLightPin, HIGH); // Start with red light on
    digitalWrite(greenLightPin, LOW); // Start with green light off
    Utilities::log("[Door] Initialized with door locked and red light on");
}

void Door::turnOnLight(int pin) {
    Utilities::log("[Door] Turning on light at pin: ");
    Utilities::log("[Door] " + pin);
    digitalWrite(pin, HIGH);
}

void Door::turnOffLight(int pin) {
    Utilities::log("[Door] Turning off light at pin: ");
    Utilities::log("[Door] " + pin);
    digitalWrite(pin, LOW);
}

Door* Door::getInstance() {
    if (instance == nullptr) {
        Utilities::log("[Door] Creating instance");
        instance = new Door();
    }
    return instance;
}

void Door::lock() {
    Utilities::log("[Door] Locking door");
    if (!isDoorLocked) {
        digitalWrite(doorLockPin, HIGH);
        turnOnLight(redLightPin);
        turnOffLight(greenLightPin);
        isDoorLocked = true;
        Utilities::log("[Door] Door locked, red light on, green light off");
    } else {
        Utilities::log("[Door] Door already locked");
    }
}

void Door::unlock() {
    Utilities::log("[Door] Unlocking door");
    if (isDoorLocked) {
        digitalWrite(doorLockPin, LOW);
        turnOffLight(redLightPin);
        turnOnLight(greenLightPin);
        isDoorLocked = false;
        lastUnlockTime = millis();
        Serial.println("[Door] Door unlocked, green light on, red light off");
    } else {
        Utilities::log("[Door] Door already unlocked");
    }
}

void Door::update() {
    Utilities::log("[Door] Updating door state");
    if (!isDoorLocked && millis() - lastUnlockTime > relayUnlockDuration) {
        lock();
        Utilities::log("[Door] Auto-relocking door");
    }
}
