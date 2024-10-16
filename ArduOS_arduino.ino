/*
*█───█─████─████─█──█─███─█──█─████
*█───█─█──█─█──█─██─█──█──██─█─█───
*█─█─█─████─████─█─██──█──█─██─█─██
*█████─█──█─█─█──█──█──█──█──█─█──█
*─█─█──█──█─█─█──█──█─███─█──█─████
*
*This vesion support only: Arduino UNO, Arduino Nano, Arduino leonadro.
*/

#if defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
#pragma message "Thanks for using ArduOS"
#define board 0
#elif defined(__AVR_ATmega32U4__)
#pragma message "Thanks for using ArduOS"
#define board 1
#else
#pragma message "█───█─████─████─█──█─███─█──█─████ "
#pragma message "█───█─█──█─█──█─██─█──█──██─█─█───"
#pragma message "█─█─█─████─████─█─██──█──█─██─█─██"
#pragma message "█████─█──█─█─█──█──█──█──█──█─█──█"
#pragma message "─█─█──█──█─█─█──█──█─███─█──█─████"
#pragma message"This board is not officially supported by ArduOS. The firmware may not work correctly!"
//"This board is not officially supported by ArduOS. The firmware may not work correctly!"
#endif

#include <Wire.h>
#include <SoftwareSerial.h>
SoftwareSerial uart(7, 8);

#include <Eeprom24C128_256.h>
static Eeprom24C128_256 eeprom(0x50);
static Eeprom24C128_256 shield(0x54);

String proglist[4];
String netbuffer;
bool root;
void setup() {
  Serial.begin(9600);
  Serial.setTimeout(50);

  Wire.begin();

  Wire.beginTransmission(84);  // 80 - main eeprom; 84 - shield (board) eeprom;
  if (!Wire.endTransmission() == 0) {
    Serial.println("Shield not aplied or shield eeprom faulty");
    while (1) {}
  }
  eeprom.initialize();
  shield.initialize();

  if (eeprom.readByte(5)) {
    uart.begin(9600);
    uart.setTimeout(50);
  }

  pinMode(9, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(A0, INPUT);
  randomSeed(analogRead(A0));

  execute(0);
  startup();
  auth();

  Serial.println("Logged! Type help for help.");
  if (root) {
    Serial.println("root rights are activated");
  }
}

void loop() {
  if (Serial.available()) {
    commands(Serial.readString());
  }

  if (uart.available()) {
    netbuffer += uart.readString();
    Serial.print("ArduNet recived: "); Serial.println(netbuffer);
  }
}

void commands(String cmd) {
  if (cmd == "help") {
    help();
  }
  else if (cmd == "info") {
    Serial.println("ArduOS v0.1 βeta ||DEVELOPER EDITION||");
  }
  else if (cmd == "proglist") {
    programs(0, 0);
  }
  else if (cmd.startsWith("run")) {
    String runCmd = "run";
    int val = cmd.substring(runCmd.length()).toInt();
    programs(1, val);
  }
  else if (cmd.startsWith("startup") and root) {
    String startupCmd = "startup";
    int val = cmd.substring(startupCmd.length()).toInt();
    eeprom.writeByte(4, val);
    delay(10);
    Serial.print("Seted startup to "); Serial.println(val);
  }
  else if (cmd == "restart" and root) {
    Serial.println("Restarting...");
    reset();
  }
  else if (cmd.startsWith("system") and root) {
    String sysCmd = "system";
    system(cmd.substring(sysCmd.length() + 1));
  }
  else if (cmd.startsWith("net") and root) {
    String netCmd = "net";
    net(cmd.substring(netCmd.length() + 1));
  }
  else {
    Serial.println("Wrong comand!");
  }
}

void help() {
  Serial.println("Comand list:");
  Serial.println("help - shows this page");
  Serial.println("info - system information");
  Serial.println("proglist - list of programs");
  Serial.println("run - run program");
  Serial.println("*startup <program number> - add program to autorun");
  Serial.println("*restart - restart device");
  Serial.println("* - need root");
}

void auth() {
  String user;
  Serial.println("Enter login");
  while (1) {
    if (Serial.available()) {
      user = Serial.readString();
      break;
    }
  }

  Serial.println("Enter password");
  while (1) {
    if (Serial.available()) {
      int pass = Serial.parseInt();
      if (user == "root" and pass == 0000) {
        root = 1;
        break;
      }
      else if (user == "user" and pass == 1234) {
        root = 0;
        break;
      }
      else {
        Serial.println("Wrong password or username!");
        delay(2000);
        reset();
      }
    }
  }
}

void reset() {
  digitalWrite(9, 1);
}
