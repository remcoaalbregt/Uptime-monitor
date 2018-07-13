#include <Arduino.h>
#include <config.h>
#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>

RH_NRF24 driver;
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

static const uint8_t allLights[] = {RED,YELLOW,GREEN};
uint8_t numberOfLights = sizeof(allLights);

uint8_t data[] = "Hello World!";
uint8_t buffer[RH_NRF24_MAX_MESSAGE_LEN];

enum Command {ALARM, WARNING, GOOD, LUNCH, DEMO};

void setLights(uint8_t colors[], uint8_t state) {
  uint8_t length = sizeof(colors);

  for (int colorIndex = 0; colorIndex < length; colorIndex++) {
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

void handleCommand(Command command) {
  Serial.print("Received command: ");
  Serial.println(command);

  switch (command) {
    case ALARM:
      flashFast(RED, 5, true);
      switchLight(RED);
      break;
    case WARNING:
      flashSlow(YELLOW, 3, true);
      switchLight(YELLOW);
      break;
    case GOOD:
      switchLight(GREEN);
      break;
    case LUNCH:
      // statements
      break; 
    case DEMO:
      // statements
      break;           
    default:
      Serial.println("Command unknown");
  }
}

void setup() {
  Serial.begin(9600);

  if (!manager.init()) { Serial.println("init failed"); }

  for (int thisPin = 0; thisPin < numberOfLights; thisPin++) {
    pinMode(allLights[thisPin], OUTPUT);
  }

  Serial.println("Setup complete..");  
}

void endLoop(const String message) {
  Serial.println(message);
  delay(500);
}

void loop() {
  Serial.println("Sending to nrf24_reliable_datagram_server");
    
  // Send a message to manager_server
  if (!manager.sendtoWait(data, sizeof(data), SERVER_ADDRESS)) {
    endLoop("sendtoWait failed");
    return;
  }

  // Now wait for a reply from the server
  uint8_t length = sizeof(buffer);
  uint8_t from;   
  if (!manager.recvfromAckTimeout(buffer, &length, 2000, &from)) {
    endLoop("No reply, is nrf24_reliable_datagram_server running?");
    return;
  }
    
  Serial.print("got reply from : 0x");
  Serial.print(from, HEX);
  Serial.print(": ");
  Serial.println((char*)buffer);

  // uint8_t buffer[RH_NRF24_MAX_MESSAGE_LEN];
  // uint8_t length = sizeof(buffer);

  // if (nrf24.waitAvailableTimeout(500)) {
  //   // Should be a reply message for us now   
  //   if (nrf24.recv(buffer, &length)) {
  //     Serial.print("received command: ");
  //     Serial.println((char*)buffer);

  //     // handleCommand(ALARM);
  //   } else {
  //     Serial.println("recv failed");
  //   }
  // }
  // delay(400);
}