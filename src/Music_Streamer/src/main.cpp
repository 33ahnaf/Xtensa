#include <Arduino.h>
#include <SD.h>
#include <stdint.h>
// #include "WiFi_Downloader.h"
#include "AudioStreamer.h"
// #include "AudioStreamerTemp.h"

#define SD_CS 5

const char *ssid = "Music Streamer";
const char *password = "12345678";

void setup(){
    Serial.begin(115200);
    dacLow();
    if(!SD.begin(SD_CS, SPI, 40000000))
        while(1);
}

void loop(){
    if(Serial.available() > 0){
        String inputString = Serial.readStringUntil('\n');
        inputString.trim();
        if(!strcmp(inputString.c_str(), "upload")){
            // downloadFile(ssid, password);
            // Serial.println("Uploading feature is not available right now.");
        }else{
            Serial.printf("Now Playing: %s\n", inputString.c_str());
            // Serial.printf("Bass: %4d\tMid: %4d\tTreble: %4d\n", bass, mid, treble);
            streamAudio(inputString.c_str());
            Serial.printf("Playback Stopped\n\n");
            while(Serial.available() > 0)
                Serial.read();
        }
    }
}