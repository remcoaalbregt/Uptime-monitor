#include <Arduino.h>
#include <config.h>
#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>

RH_NRF24 driver;
RHReliableDatagram manager(driver, SERVER_ADDRESS);

uint8_t data[] = "And hello back to you";
// Dont put this on the stack:
uint8_t buffer[RH_NRF24_MAX_MESSAGE_LEN];

void setupRadio() {
  if (!manager.init()) { Serial.println("init failed"); }
}

void setup() {
  Serial.begin(9600);
  setupRadio();
  Serial.println("Gateway setup complete..");
}

void loop() {
  if (manager.available()) {
    uint8_t length = sizeof(buffer);
    uint8_t from;
    if (manager.recvfromAck(buffer, &length, &from)) {
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buffer);

      // Send a reply back to the originator client
      if (!manager.sendtoWait(data, sizeof(data), from)) {
        Serial.println("sendtoWait failed");
      }
    }
  }
}