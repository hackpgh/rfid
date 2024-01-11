#include <Arduino.h>
#include "RFIDReader.h"
#include "Door.h"
#include "Auth.h"
#include "Utilities.h"

// WiFi credentials
const char* ssid = "your-wifi-ssid";
const char* password = "your-wifi-password";
WiFiClient wifiClient;

// Timing constants
const long updateInterval = 300000;  // 5 minutes in milliseconds
long lastCacheUpdateTime = 0;

// Eastern Time Zone (EST/EDT)
const long gmtOffset_sec = -5 * 3600; // GMT -5 hours for EST
const int daylightOffset_sec = 3600;  // 1 hour for EDT
void pollRFIDTask(void * parameter); // Forward declaration of the RFID polling task

SemaphoreHandle_t delaySemaphore;
volatile int rfidTaskDelay = 10;

/**
 * Task function for polling RFID reader.
 * Continuously checks for new RFID tags and processes them.
 * Under standard operating procedure, this task will poll every 15ms meeting the
 * Wiegand26 protocol's 25ms frame time requirement.
 */
void pollRFIDTask(void *parameter) {
    RFIDReader* rfidReader = RFIDReader::getInstance(wifiClient);
    for (;;) {
        rfidReader->loop();
        int currentDelay;

        // Safely get the current delay using semaphore for synchronization
        if (xSemaphoreTake(delaySemaphore, portMAX_DELAY) == pdTRUE) {
            currentDelay = rfidTaskDelay;
            xSemaphoreGive(delaySemaphore);
        } else {
            Serial.println("[Error] Failed to take semaphore");
            currentDelay = 10; // Default delay in case of error
        }

        // Delay the task for the specified time
        vTaskDelay(currentDelay / portTICK_PERIOD_MS);
    }
}

/**
 * Initialize and synchronize time with NTP servers for logging.
 * Configures time zone and daylight saving settings.
 */
void initNTP() {
  delaySemaphore = xSemaphoreCreateMutex();
  if (delaySemaphore == NULL) {
      Serial.println("[Error] Semaphore creation failed!");
  }
  Serial.println("[NTP] Configuring time for Eastern Time Zone");
  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov", "time.google.com");

  time_t now = time(nullptr);
  int attempt = 0;
  while (now < 8 * 3600 * 2) { // Wait for time to be set
      delay(500);
      Serial.print(".");
      attempt++;
      now = time(nullptr);
      if (attempt >= 20) { // Timeout after 10 seconds
          Serial.println("\n[NTP] Time sync failed");
          break;
      }
  }
  if (now >= 8 * 3600 * 2) {
      Serial.println("\n[NTP] Time synchronized");
  }
}

/**
 * Setup function for initial configuration.
 * Configures WiFi, NTP, and initializes Door, Auth, and RFIDReader singletons.
 */
void setup() {
    Serial.begin(115200);
    Serial.println("[Main] Starting setup");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("[Main] Connecting to WiFi...");
    }
    Serial.println("[Main] WiFi Connected");

    // Initialize NTP for time synchronization
    initNTP();

    Utilities::log("[Main] Initializing RFIDReader");
    RFIDReader::getInstance(wifiClient);
    Utilities::log("[Main] Initializing RFIDReaderTask, Door, and Auth objects");
    xTaskCreatePinnedToCore(
                pollRFIDTask,   /* Task function. */
                "pollRFIDTask", /* name of task. */
                10000,          /* Stack size of task */
                NULL,           /* parameter of the task */
                1,              /* priority of the task */
                NULL,           /* Task handle to keep track of created task */
                1);             /* pin task to core 1 */
    
    // Initial update for door status and RFID cache
    Door::getInstance()->update();
    Auth::getInstance(wifiClient)->fetchAndCacheRFIDData();
    lastCacheUpdateTime = millis();
    Utilities::log("[Main] Setup complete");
}

/**
 * Main loop function.
 * Handles reconnection to WiFi and periodic cache updates on default CPU core 0.
 * Secondary CPU core (1) is already constantly polling (via setup() of pollRFIDTask)
 * Wiegand26 tag scans
 */
void loop() {
  // Reconnect to WiFi if disconnected
  if (WiFi.status() != WL_CONNECTED) {
      WiFi.reconnect();
      Utilities::log("[Main] Reconnecting to WiFi...");
      while (WiFi.status() != WL_CONNECTED) {
          delay(500);
      }
      Utilities::log("[Main] WiFi Reconnected");
  }

  // Periodically update the RFID cache and check door status
  //Utilities::log("[Main] Loop start");
  if (millis() - lastCacheUpdateTime >= updateInterval) {
    Utilities::log("[Main] Checking Door is Locked");
    Door::getInstance()->update();
    Utilities::log("[Main] Updating RFID cache");
    Auth::getInstance(wifiClient)->fetchAndCacheRFIDData();
    lastCacheUpdateTime = millis(); // Reset the timer
  }
  delay(10);
  //Utilities::log("[Main] Loop end");
}