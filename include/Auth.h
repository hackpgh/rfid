#ifndef AUTH_H
#define AUTH_H

#include <Arduino.h>
#include <WiFi.h>  // Include the correct WiFi library for your hardware
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <Base64.h>
#include "Door.h"
#include <unordered_set>
#include "ExponentialBackoffHandler.h"


/**
 * @brief The `Auth` class handles WildApricot requests, authenticated tag caching, and tag authorization.
 */
class Auth {
private:
    static Auth* instance; ///< Singleton instance of the Auth class.
    static const char* apiEndpoint; ///< API endpoint for tag data retrieval.
    static const char* tokenUrl; ///< URL for obtaining the authentication token.
    static const char* cacheFilePath; ///< File path for caching tag data.
    static const char* apiKey; ///< API key for authentication.
    static const int serverPort; ///< Port number for the server.
    std::unordered_set<uint32_t> cachedTagIDs; ///< Cached RFID tag IDs.
    static const char* serverName; ///< Server name for API requests.
    WiFiClient& wifiClient; ///< Reference to the WiFi client.
    HttpClient httpClient; ///< HTTP client for API requests.
    ExponentialBackoffHandler backoffHandler; ///< Backoff handler for failed attempts.

    /**
     * @brief Private constructor for the Auth class.
     * @param client Reference to the WiFiClient object.
     */
    Auth(WiFiClient& client);

    void initWifi(); ///< Initialize WiFi connection.
    String getAuthToken(); ///< Get the authentication token.
    std::unordered_set<uint32_t> getTagIds(const String& authToken); ///< Get RFID tag IDs.
    String fetchRFIDJson(const String& authToken); ///< Fetch RFID data from the server.
    void parseAndCacheRFIDData(const String& jsonData); ///< Parse and cache RFID Json data.

public:
    /**
     * @brief Get the singleton instance of the Auth class.
     * @param client Reference to the WiFiClient object.
     * @return Pointer to the Auth instance.
     */
    static Auth* getInstance(WiFiClient& client);
    ~Auth(); ///< Destructor for the Auth class.

    /**
     * @brief Initialize the Auth module, including fetching initial cache data.
     */
    void initialize();

    /**
     * @brief Authenticate an RFID tag against cache.
     * @param tagId RFID tag ID to authenticate.
     */
    void authenticate(const uint32_t& tagId);
    void updateCache(); //TODO: Migrate from SPIFFS to LittleFS for improved wear leveling

    /**
     * @brief Check if an RFID tag is authorized.
     * @param tagId RFID tag ID to check.
     * @return True if the tag is authorized, false otherwise.
     */
    bool isTagAuthorized(const uint32_t& tagId);
    void fetchAndCacheRFIDData();

    Auth(const Auth&) = delete; ///< Disable copy constructor.
    Auth& operator=(const Auth&) = delete; ///< Disable assignment operator.
};

#endif // AUTH_H