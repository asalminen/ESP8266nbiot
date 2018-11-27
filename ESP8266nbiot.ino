#include <SoftwareSerial.h>

// Connect modem  tx to ESP D7 (rx)
//                rx to ESP D8 (tx)
//                G to G and V to V3,3
//                Connect modem K ESP D1 ( on key pressed )

int8_t pollModem = 1;
uint32 millisTime = 0;
SoftwareSerial modem( D7, D8 );

void setup() {
  // initialize both serial ports:
  Serial.begin(115200);
  modem.begin(115200);

  pinMode(D1, OUTPUT); // Modem power key
  Serial.println("NB-IOT MQTT test");
  setupConnection();
}

void loop() {
  //Forward USB serial to modem and back ( manual AT commands )
  if (modem.available() and pollModem) {
    uint8_t inByte = modem.read();
    Serial.write(inByte);
  }
  if (Serial.available()) {
    uint8_t inByte = Serial.read();
    modem.write(inByte);
  }

  // Send data every 10 sec
  if ( millis() - millisTime > 10000) {
    if ( sendATCommand((char *)"AT+CHTTPSEND=0,0,\"/index.html?\"")) {
      Serial.println("Modeemin httpsend ei toimi.");
      reboot();
    }
    millisTime = millis();
  }
}

void setupConnection() {
  PowerOnModem();
  //char *ATCommand;
  //ATCommand="ATZ";
  pollModem = 0;
  Serial.println("Setup NB MQTT connection.");
  //sendATCommand((char *)"AT");
  if (sendATCommand((char *)"ATZ")) {
    Serial.println("Modeemin alustus ei toimi.");
    reboot();
  }
  if ( sendATCommand((char *)"AT+CPIN?")) {
    Serial.println("Modeemin SIM kortti ei toimi.");
    reboot();
  }

  sendATCommand((char *)"AT+CSQ");
  sendATCommand((char *)"AT+CGREG?");
  sendATCommand((char *)"AT+CGACT?");
  sendATCommand((char *)"AT+COPS?");

  sendATCommand((char *)"AT+CGCONTRDP");
  sendATCommand((char *)"AT+CHTTPCREATE=\"http://iot.fvh.fi/\"");
  if (sendATCommand((char *)"AT+CHTTPCON=0")) {
    Serial.println("Modeemin HTTPcon ei toimi.");
    reboot();
  }
  //sendATCommand((char *)"AT+CHTTPDISCON=0");
  //sendATCommand((char *)"AT+CHTTPDESTROY=0");
  pollModem = 1;
}

uint8_t sendATCommand(char *ATCommand) {
  uint8_t aswerbytes[4] = {0, 0, 0, 0}; // Save last two chars + NL&CR to compare if ok
  int16_t timeout=10000; // If no aswer in 10 sec timeout
  Serial.print("Sending AT command: ");
  //Send data to modem
  modem.print(ATCommand);
  modem.print("\r\n");
  //Wait for aswer
  while (!modem.available()) {  
    delay(50);
    timeout=timeout-50;
    if( timeout <= 0 ) return(2); // return 2 on timeout
  }
  // Copy aswer from modem to USB serial
  while (modem.available()) {
    uint8_t inByte = modem.read();
    Serial.write(inByte);
    aswerbytes[0] = aswerbytes[1];
    aswerbytes[1] = aswerbytes[2];
    aswerbytes[2] = aswerbytes[3];
    aswerbytes[3] = inByte;
    delay(10);
  }
  Serial.println("Sending AT DONE.");
  Serial.println();
  Serial.flush();
  if ( aswerbytes[0] == 'O' and aswerbytes[1] == 'K' ) return (0); // Return 0 if ok
  else return (1); // Returun 1 on error
}

void reboot() {
  Serial.println("Restarting...");
  ESP.restart();
}

void PowerOnModem() {
  pollModem = 0;
  Serial.println("Power on modem.");
  digitalWrite( D1, LOW );
  delay(500);
  digitalWrite( D1,  HIGH );
  delay(500);
  pollModem = 1;
}
