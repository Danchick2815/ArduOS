void programs(int param, int value) {
  switch (param) {
    case 0:
      Serial.println("Programs list:");
      for (int i = 0; i < 3; i++) {
        if (eeprom.readByte(4) - 1 == i) {
          Serial.print(proglist[i]);
          Serial.println(" < startup");
        }
        else {
          Serial.println(proglist[i]);
        }
      }
      break;
    case 1:
      Serial.print("Runing program number: "); Serial.println(value);
      execute(value);
      break;
  }
}

void system(String value) {
  if (value == "bv") {
    Serial.print("Board version: "); Serial.println(shield.readByte(0));
  }
  else if (value.startsWith("setbv")) {
    String sbvCmd = "setbv";
    int pval = value.substring(sbvCmd.length() + 1).toInt();
    byte val = pval;
    shield.writeByte(0, val);
    delay(10);
    Serial.print("Seted board version to: "); Serial.println(shield.readByte(0));
  }
  else if (value.startsWith("pin")) {
    String pinCmd = "pin";
    int val = value.substring(pinCmd.length() + 1).toInt();
    digitalWrite(val, !digitalRead(val));
    Serial.print("Pin "); Serial.print(val); Serial.print(" is now "); Serial.println(digitalRead(val));
  }
  else {
    Serial.println(value);
    Serial.println("Wrong system command!");
  }
}

void net(String value) {
  if (value == "info") {
    Serial.print("Net is now: "); Serial.println(eeprom.readByte(5));
  }
  else if (value == "toggle") {
    byte val = !eeprom.readByte(5);
    delay(1);
    eeprom.writeByte(5, val);
    delay(10);
    Serial.print("Seted net toggle to: "); Serial.println(eeprom.readByte(5));
  }
  else if (value.startsWith("send")) {
    String sendCmd = "send";
    uart.println(value.substring(sendCmd.length() + 1));
    Serial.print("Send "); Serial.println(value.substring(sendCmd.length() + 1));
  }
  else {
    Serial.println(value);
    Serial.println("Wrong net command!");
  }
}

void execute(int value) {
  switch (value) {
    case 0:
      proglist[0] = "Program 1";
      proglist[1] = "Random   ";
      proglist[2] = "I2C scan ";
      break;

    case 1:
      Serial.println("Program 1");
      break;

    case 2:
      while (1) {
        static uint32_t tmr;
        if (millis() - tmr >= 100) {
          Serial.println(random(0, 255));
        }
      }

      break;

    case 3:
      int nDevices = 0;

      Serial.println("Scanning...");

      for (byte address = 1; address < 127; ++address) {
        // The i2c_scanner uses the return value of
        // the Wire.endTransmission to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        byte error = Wire.endTransmission();

        if (error == 0) {
          Serial.print("I2C device found at address 0x");
          if (address < 16) {
            Serial.print("0");
          }
          Serial.print(address, HEX);
          Serial.println("  !");

          ++nDevices;
        } else if (error == 4) {
          Serial.print("Unknown error at address 0x");
          if (address < 16) {
            Serial.print("0");
          }
          Serial.println(address, HEX);
        }
      }
      if (nDevices == 0) {
        Serial.println("No I2C devices found\n");
      } else {
        Serial.println("done\n");
      }
      break;
  }
}

void startup() {
  execute(eeprom.readByte(4));
}
