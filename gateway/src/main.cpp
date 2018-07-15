#include <Arduino.h>
#include <config.h>
#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>

RH_NRF24 driver;
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
enum Command {ALARM = 1, WARNING = 2, GOOD = 3, LUNCH = 4, DEMO = 5};

void setup() {
  Serial.begin(9600);
  if (!manager.init()) {
     Serial.println("init failed");
  } else {
    Serial.println("Server started");
  }
}

void sendToClient(uint8_t data[], uint8_t client) {
    Serial.println("Sending to client");
    // Send a message to manager_server
    if (manager.sendtoWait(data, sizeof(data), SERVER_ADDRESS)) {
      // Now wait for a reply from the server
      uint8_t len = sizeof(buf);
      uint8_t from;   
      if (manager.recvfromAckTimeout(buf, len, 2000, &from)) {
        Serial.print("got confirmation from client: ");
        Serial.println(from, HEX);
      } else { Serial.println("No reply, is the client running?"); }
    } else { Serial.println("sendtoWait failed"); }
}

void sendToBuildLight(uint8_t data[]) {
  sendToClient(data, SERVER_ADDRESS);
}

uint8_t alarm[] = "1";
uint8_t warning[] = "2";
uint8_t good[] = "3";
uint8_t lunch[] = "4";
uint8_t demo[] = "5";

void loop() {
  if (Serial.available() > 0) {
    int serialCommand = Serial.read();

    switch (serialCommand) {
      case '1':
        sendToBuildLight(alarm);
        break;
      case '2':
        sendToBuildLight(warning);
        break;
      case '3':
        sendToBuildLight(good);
        break;
      case '4':
        sendToBuildLight(lunch);
        break;
      case '5':
        sendToBuildLight(demo);
        break;
    }
  }
  delay(500);
}