#include <SoftwareSerial.h>
int8_t pollModem=1;
SoftwareSerial modem( D7, D8 );

void setup() {
  // initialize both serial ports:
  Serial.begin(115200);
  modem.begin(115200);

  pinMode(D1, OUTPUT); // Modem power key
  digitalWrite( D1, LOW ); // Start modem
  delay(200);
  digitalWrite( D1, HIGH );
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
  pollModem=0;
  Serial.println("Setup NB MQTT connection.");
  sendATCommand((char *)"AT");
  sendATCommand((char *)"ATZ");
  sendATCommand((char *)"AT+CPIN?");
  sendATCommand((char *)"AT+CSQ");
  sendATCommand((char *)"AT+CGREG?");
  sendATCommand((char *)"AT+CGACT?");
  sendATCommand((char *)"AT+COPS?");
  sendATCommand((char *)"AT+CGCONTRDP");
  sendATCommand((char *)"AT+CHTTPCREATE=\"http://www.sim.com/\"");
  sendATCommand((char *)"AT+CHTTPCON=0");
  sendATCommand((char *)"AT+CHTTPSEND=0,0,\"/index.html\"");
  sendATCommand((char *)"AT+CHTTPDISCON=0");
  sendATCommand((char *)"AT+CHTTPDESTROY=0");
  pollModem=1;
}

uint8_t sendATCommand(char *ATCommand) {
  Serial.print("Sending AT command: ");
  modem.print(ATCommand);
  modem.print("\r\n");
   while (!modem.available()) {
    delay(100);
  }
  while (modem.available()) {
    int8_t inByte = modem.read();
    Serial.write(inByte);
    delay(10);
  }
  Serial.flush();
  return (0);
}
