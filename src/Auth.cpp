#include "Auth.h"
#include "Utilities.h"
#include "RFIDReader.h"
#include <cmath> // For pow function

Auth* Auth::instance = nullptr;
const char* Auth::tokenUrl = "https://api.wildapricot.org/auth/token";
const char* Auth::apiEndpoint = "https://api.wildapricot.org/v2.1/accounts/your-wild-apricot-account-number/Contacts";
const char* Auth::cacheFilePath = "/tag_ids_cache.json";
const char* Auth::apiKey = "your-api-key";
const char* Auth::serverName = "api.wildapricot.org"; 
const int Auth::serverPort = 443;
extern SemaphoreHandle_t delaySemaphore;
extern volatile int rfidTaskDelay;

Auth::Auth(WiFiClient& client) : wifiClient(client), httpClient(wifiClient, serverName, serverPort) {
    Utilities::log("[Auth] Initializing");

    if (!SPIFFS.begin()) {
        Utilities::log("SPIFFS Mount Failed");
    } else {
        Utilities::log("SPIFFS Mounted Successfully");
    }
}

Auth* Auth::getInstance(WiFiClient& client) {
    if (instance == nullptr) {
        Utilities::log("[Auth] Creating instance");
        instance = new Auth(client);
    }
    return instance;
}

Auth::~Auth() {
    Utilities::log("[Auth] Destructor called");
}

void Auth::initialize() {
    Utilities::log("[Auth] Initialize called");
    updateCache(); // Fetch initial cache data
}

void Auth::authenticate(const uint32_t& tagId) {
    Utilities::log("[Auth] Authenticating tag ID: " + String(tagId));
    if (cachedTagIDs.find(tagId) != cachedTagIDs.end()) {
        Utilities::log("[Auth] Access Granted");
        Door::getInstance()->unlock();
    } else {
        Utilities::log("[Auth] Access Denied");
        backoffHandler.failedAttempt();

        if (xSemaphoreTake(delaySemaphore, portMAX_DELAY) == pdTRUE) {
            rfidTaskDelay = backoffHandler.getCurrentDelay();
            xSemaphoreGive(delaySemaphore);
        } else {
            Utilities::log("[Auth] Error taking semaphore");
        }
    }
}

void Auth::updateCache() {
    Utilities::log("[Auth] Updating cache");
    String authToken = getAuthToken();
    if (!authToken.isEmpty()) {
        auto tagIds = getTagIds(authToken);
        if (!tagIds.empty()) {
            cachedTagIDs = tagIds;
            Utilities::log("[Auth] Cache updated successfully");
        } else {
            Utilities::log("[Auth] No tag IDs fetched");
        }
    } else {
        Utilities::log("[Auth] Failed to get auth token");
    }
}

String Auth::getAuthToken() {
    Utilities::log("[Auth] Fetching auth token");
    httpClient.beginRequest();
    httpClient.post(tokenUrl);
    String encodedCredentials = base64::encode(String("APIKEY:") + apiKey);
    httpClient.sendHeader("Authorization", "Basic " + encodedCredentials);
    httpClient.sendHeader("Content-Type", "application/x-www-form-urlencoded");
    httpClient.beginBody();
    httpClient.print("grant_type=client_credentials&scope=auto");
    httpClient.endRequest();

    int statusCode = httpClient.responseStatusCode();
    String response = httpClient.responseBody();

    if (statusCode == 200) {
        Utilities::log("[Auth] Successfully retrieved auth token");
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, response);
        return doc["access_token"].as<String>();
    } else {
        Utilities::log("[Auth] Failed to retrieve auth token, HTTP Code: " + String(statusCode));
        return "";
    }
}

std::unordered_set<uint32_t> Auth::getTagIds(const String& authToken) {
    Utilities::log("[Auth] Fetching tag IDs");
    httpClient.beginRequest();
    httpClient.get(apiEndpoint);
    httpClient.sendHeader("Authorization", "Bearer " + authToken);
    httpClient.sendHeader("Content-Type", "application/json");
    httpClient.endRequest();

    int httpCode = httpClient.responseStatusCode();
    String payload = httpClient.responseBody();

    if (httpCode == 200) {
        Utilities::log("[Auth] Successfully retrieved tag data");
        DynamicJsonDocument doc(4096);
        deserializeJson(doc, payload);

        std::unordered_set<uint32_t> tagIds;
        JsonArray contacts = doc["Contacts"].as<JsonArray>();
        for (JsonObject contact : contacts) {
            uint32_t rfidValue = contact["RFIDFieldName"].as<uint32_t>();
            if (rfidValue > 0) {
                tagIds.insert(rfidValue);
            }
        }
        return tagIds;
    } else {
        Utilities::log("[Auth] Failed to retrieve tag data, HTTP Code: " + String(httpCode));
        return std::unordered_set<uint32_t>();
    }
}

String Auth::fetchRFIDJson(const String& authToken) {
    Utilities::log("[Auth] Fetching RFID data");
    httpClient.beginRequest();
    httpClient.get(apiEndpoint);
    httpClient.sendHeader("Authorization", "Bearer " + authToken);
    httpClient.sendHeader("Content-Type", "application/json");
    httpClient.endRequest();

    int httpCode = httpClient.responseStatusCode();
    String payload = httpClient.responseBody();

    if (httpCode == 200) {
        Utilities::log("[Auth] Successfully retrieved RFID data");
        return payload;
    } else {
        Utilities::log("[Auth] Failed to retrieve RFID data, HTTP Code: " + String(httpCode));
        return "";
    }
}

void Auth::parseAndCacheRFIDData(const String& jsonData) {
    Utilities::log("[Auth] Parsing and caching RFID data");
    DynamicJsonDocument doc(4096);
    deserializeJson(doc, jsonData);

    JsonArray contacts = doc["Contacts"].as<JsonArray>();
    cachedTagIDs.clear();
    for (JsonObject contact : contacts) {
        uint32_t rfidValue = contact["RFIDFieldName"].as<uint32_t>();
        if (rfidValue > 0) {
            cachedTagIDs.insert(rfidValue);
        }
    }

    if (!SPIFFS.begin()) {
        Utilities::log("Failed to mount file system");
        return;
    }

    File cacheFile = SPIFFS.open("/rfid_cache.json", FILE_WRITE);
    if (!cacheFile) {
        Utilities::log("Failed to open cache file for writing");
        SPIFFS.end();
        return;
    }

    for (const auto& id : cachedTagIDs) {
        cacheFile.println(id);
    }
    cacheFile.close();
    SPIFFS.end();
    Utilities::log("[Auth] RFID data cached successfully");
}

void Auth::fetchAndCacheRFIDData() {
    Utilities::log("[Auth] Fetching and caching RFID data");
    String authToken = getAuthToken();
    if (!authToken.isEmpty()) {
        String jsonData = fetchRFIDJson(authToken);
        if (!jsonData.isEmpty()) {
            parseAndCacheRFIDData(jsonData);
            Utilities::log("[Auth] RFID data fetched and parsed successfully");
        } else {
            Utilities::log("[Auth] No RFID data to parse");
        }
    } else {
        Utilities::log("[Auth] Failed to fetch auth token");
    }
}
