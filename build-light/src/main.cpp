#include <Arduino.h>
#include <config.h>
#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>

RH_NRF24 driver;
RHReliableDatagram manager(driver, SERVER_ADDRESS);

static const uint8_t allLights[] = {RED,YELLOW,GREEN};

enum Command {ALARM = '1', WARNING = '2', GOOD = '3', LUNCH = '4', DEMO = '5'};

void setLights(uint8_t colors[], uint8_t state) {
  uint8_t length = sizeof(colors);

  for (int colorIndex = 0; colorIndex < (length+1); colorIndex++) {
    Serial.print("setting color: ");
    Serial.print(colors[colorIndex]);
    Serial.print(" to: ");
    Serial.println(state);
    digitalWrite(colors[colorIndex], state);
  }
}

void multipleOn(uint8_t colors[]) {
  setLights(colors, HIGH);
}

void multipleOff(uint8_t colors[]) {
  setLights(colors, LOW);
}

void allOff() {
  multipleOff(allLights);
}

void switchLight(uint8_t color) {
  allOff();
  digitalWrite(color, HIGH);
}

void flashLight(uint8_t color, uint8_t numberOfTimes, uint16_t speed, bool beep = false) {
  allOff();
  for(int count = 0; count < numberOfTimes; count++) {
    Serial.print("Flash ");
    Serial.println(count);
    digitalWrite(color, HIGH);
    delay(speed);
    digitalWrite(color, LOW);
    delay(speed);
  }
  digitalWrite(color, LOW);
}

void flashFast(uint8_t color, uint8_t numberOfTimes, bool beep = false) {
  flashLight(color, numberOfTimes, 500);
}

void flashSlow(uint8_t color, uint8_t numberOfTimes, bool beep = false) {
  flashLight(color, numberOfTimes, 1000);
}

void handleCommand(uint8_t command) {
  switch (command) {
    case ALARM:
      Serial.println("Received Alarm");
      flashFast(RED, 5, true);
      switchLight(RED);
      break;
    case WARNING:
      Serial.println("Received Warning");
      flashSlow(YELLOW, 3, true);
      switchLight(YELLOW);
      break;
    case GOOD:
      Serial.println("Received Good");
      switchLight(GREEN);
      break;
    case LUNCH:
      Serial.println("Received Lunch");
      // statements
      break; 
    case DEMO:
      Serial.println("Received Demo");
      // statements
      break;           
    default:
      Serial.println("Command unknown");
  }
}

void setup() {
  Serial.begin(BAUD_RATE);

  for (int thisPin = 0; thisPin < sizeof(allLights); thisPin++) {
    pinMode(allLights[thisPin], OUTPUT);
    digitalWrite(allLights[thisPin], LOW);
  }

  if (!manager.init()) {
      Serial.println("init failed");
  } else {
    Serial.println("Build light started");
  }
}

uint8_t data[] = "Command received";
uint8_t buffer[RH_NRF24_MAX_MESSAGE_LEN];
void loop() {
  if (manager.available()) {
    // Wait for a message addressed to us from the server
    uint8_t length = sizeof(buffer);
    uint8_t from;
    if (manager.recvfromAck(buffer, &length, &from)) {
      // Send a confirmation back to the originator server
      if (!manager.sendtoWait(data, sizeof(data), from)) {
        Serial.println("sendtoWait failed");
      }
      handleCommand(buffer[0]);
    }
  }
}