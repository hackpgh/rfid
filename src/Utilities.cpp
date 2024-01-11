#include "Utilities.h"
#include "time.h"

void Utilities::log(const String& message) {
    String formattedTime = getFormattedTime();
    Serial.print(formattedTime);
    Serial.print(" ");
    Serial.println(message);
}

String Utilities::getFormattedTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return "";
    }
    char buffer[30];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return String(buffer);
}
