#include <SoftwareSerial.h>

// Connect modem  tx to ESP D7 (rx)
//                rx to ESP D8 (tx)

int8_t pollModem = 1;
SoftwareSerial modem( D7, D8 );

void setup() {
  // initialize both serial ports:
  Serial.begin(115200);
  modem.begin(115200);

  pinMode(D1, OUTPUT); // Modem power key
  digitalWrite( D1, LOW ); // Start modem
  delay(200);
  //  digitalWrite( D1, HIGH );
  Serial.println("NB-IOT MQTT test");
  setupConnection();
}

void loop() {
  // read from port 1, send to port 0:

  if (modem.available() and pollModem) {
    int8_t inByte = modem.read();
    Serial.write(inByte);
  }
  // read from port 0, send to port 1:
  if (Serial.available()) {
    int inByte = Serial.read();
    modem.write(inByte);
  }
}

void setupConnection() {
  //char *ATCommand;
  //ATCommand="ATZ";
  pollModem = 0;
  Serial.println("Setup NB MQTT connection.");
  //sendATCommand((char *)"AT");
  sendATCommand((char *)"ATZ");
  sendATCommand((char *)"AT+CPIN?");
  sendATCommand((char *)"AT+CSQ");
  sendATCommand((char *)"AT+CGREG?");
  sendATCommand((char *)"AT+CGACT?");
  sendATCommand((char *)"AT+COPS?");
  sendATCommand((char *)"AT+CGCONTRDP");
  sendATCommand((char *)"AT+CHTTPCREATE=\"http://iot.fvh.fi/\"");
  sendATCommand((char *)"AT+CHTTPCON=0");
  sendATCommand((char *)"AT+CHTTPSEND=0,0,\"/index.html?\"");
  sendATCommand((char *)"AT+CHTTPDISCON=0");
  sendATCommand((char *)"AT+CHTTPDESTROY=0");
  pollModem = 1;
}

uint8_t sendATCommand(char *ATCommand) {
  uint8_t aswerbytes[4]={0,0,0,0}; // Save last two chars + NL&CR to compare if ok
  Serial.print("Sending AT command: ");
  //Send data to modem
  modem.print(ATCommand);
  modem.print("\r\n");
  //Wait for aswer
  while (!modem.available()) {
    delay(50);
  }
  // Copy aswer from modem to USB serial
  while (modem.available()) {
    uint8_t inByte = modem.read();
    Serial.write(inByte);
    aswerbytes[0]=aswerbytes[1];
    aswerbytes[1]=aswerbytes[2];
    aswerbytes[2]=aswerbytes[3];
    aswerbytes[3]=inByte;
    delay(10);
  }
  Serial.println("Sending AT DONE.");
  Serial.println();
  Serial.flush();
  if( aswerbytes[0]=='O' and aswerbytes[1]=='K' ) return (0); // Return 0 if ok
  else return(1); // Returun 1 on error
}
