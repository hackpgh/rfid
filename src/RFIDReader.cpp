#include "RFIDReader.h"
#include "Auth.h"
#include "Utilities.h"

RFIDReader* RFIDReader::instance = nullptr;

RFIDReader::RFIDReader(WiFiClient& client) : wifiClient(client) {
    Utilities::log("[RFIDReader] Constructor");
    // Initialize the Wiegand interface here
    // e.g., wg.begin(D0, D1);
    Utilities::log("[RFIDReader] Wiegand interface initialized");
    
}

RFIDReader* RFIDReader::getInstance(WiFiClient& client) {
    if (instance == nullptr) {
        instance = new RFIDReader(client);
    }
    return instance;
}

void RFIDReader::loop() {
    //Utilities::log("[RFIDReader] Checking for tag");
    if (wg.available()) {
        uint32_t tagId = uint32_t(wg.getCode());
        handleTagRead(tagId);
        delay(readDelay);
    } else {
        //Utilities::log("[RFIDReader] No tag detected");
    }
}

void RFIDReader::adjustDelay(unsigned long newDelay) {
    readDelay = newDelay;
}

void RFIDReader::handleTagRead(const uint32_t& tagId) {
    Utilities::log("[RFIDReader] Tag Read: " + tagId);
    Auth::getInstance(wifiClient)->authenticate(tagId);
}
