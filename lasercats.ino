#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define PAN_PIN     5
#define TILT_PIN    4
#define LASER_PIN   16
#define PJRC_PIN    0
#define USERBTN_PIN 2

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

Servo pan, tilt;
static void playwithme();

struct config_t
{
    uint8_t pan_max = 180;
    uint8_t pan_min = 0;
    uint8_t tilt_max = 180;
    uint8_t tilt_min = 0;
    uint8_t speed_slow = 2;
    uint8_t speed_med = 10;
    uint8_t speed_fast = 20;
    const char *ssid = "robonet24";
    const char *psk = "aaaaaaaa";
} config;

void setup() {
    Serial.begin(115200);
    Serial.println("\n\n");
    // put your setup code here, to run once:
    attachInterrupt(PJRC_PIN, motion, CHANGE);
  
    Serial.print("setup, connect to wifi: ");
    Serial.print(config.ssid);
    Serial.print(" password: ");
    Serial.println(config.psk);
    
    pinMode(LASER_PIN, OUTPUT);
    pinMode(USERBTN_PIN, INPUT);
    digitalWrite(LASER_PIN, HIGH);

    WiFi.mode(WIFI_STA);
    WiFi.begin(config.ssid, config.psk);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }
    Serial.print("WIFI connected ip: ");
    Serial.println(WiFi.localIP());
    
    ArduinoOTA.setHostname("lasercat1");
    ArduinoOTA.onStart([]() {
        Serial.println("Start");
    });
  
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
  
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

    Serial.println("START OTA");
    ArduinoOTA.begin();
}

void loop()
{
    if (digitalRead(2))
    {
        Serial.println("user button not pressed, not doing anything");
        if (pan.attached())
            pan.detach();

        if (tilt.attached())
            tilt.detach();

        digitalWrite(LASER_PIN, HIGH);
        ArduinoOTA.handle();
        goto loopend;
    }

    if (!tilt.attached())
        tilt.attach(PAN_PIN);

    if (!pan.attached())
        pan.attach(TILT_PIN);

    playwithme();

    ArduinoOTA.handle();
    
loopend:
    delay(450);
}

void playwithme()
{
    digitalWrite(LASER_PIN, LOW);
    for (int i=0; i<180; i++)
    {
        pan.write(i);
        tilt.write((180-i)/2);
        delay(13);
    }

    delay(250);

    digitalWrite(LASER_PIN, LOW);
    for (int i=180; i>=0; i--)
    {
        pan.write(i);
        tilt.write(i/2);
        delay(10);
    }
}

void motion()
{
    Serial.println("change");
}

