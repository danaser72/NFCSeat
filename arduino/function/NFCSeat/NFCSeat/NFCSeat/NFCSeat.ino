#include <PN532.h>
#include <SPI.h>
#include <HX711.h>

#include <SoftwareSerial.h>  

/*Chip select pin can be connected to D10 or D9 which is hareware optional*/
/*if you the version of NFC Shield from SeeedStudio is v2.0.*/
#define PN532_CS 10   // nfc와 연결된 디지털핀 설정
PN532 nfc(PN532_CS);
#define  NFC_DEMO_DEBUG 1

int outputpin = A5; // 온도센서와 연결된 아날로그 핀 설정

#define DOUT  5     // 무게센서와 연결된 디지털 핀 설정
#define CLK  4      // 무게센서 클럭

#define SSID "JK"  //공유기 SSID
#define PASS "khu12345"   //공유기 비번
#define DST_IP "34.211.222.235"   //MYSQL 서버 주소 

SoftwareSerial dbgSerial(3, 2); // RX, TX 2번,3번핀

int check = 0;

HX711 scale(DOUT, CLK);

float calibration_factor = -45860; //-7050 worked for my 440lb max scale setup


void setup(void) {
#ifdef NFC_DEMO_DEBUG
  Serial.begin(9600);
  dbgSerial.begin(9600);
  Serial.println("ESP8266 connect");

  boolean connected = false;
  for (int i = 0; i < 10; i++)
  {
    if (connectWiFi())
    {
      connected = true;
      break;
    }
  }

  if (!connected) { while (1); }
  delay(5000);

  dbgSerial.println("AT+CIPMUX=0");
#endif
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();

  if (!versiondata) {
#ifdef NFC_DEMO_DEBUG
    Serial.print("Didn't find PN53x board");
#endif
    while (1); // halt
  }
#ifdef NFC_DEMO_DEBUG
  // Got ok data, print it out!
  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);
  Serial.print("Supports ");
  Serial.println(versiondata & 0xFF, HEX);
  Serial.println("HX711 calibration sketch");

  scale.set_scale();
  scale.tare(); //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
                       //long zero_factor = 8319787;
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
#endif
  // configure board to read RFID tags and cards
  nfc.SAMConfig();
}


void loop(void) {
  uint32_t id;
  float count = 0;
  float endTime = 0;
  char att = '3';
  // look for MiFare type cards
  id = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A);

  if (id != 0) {
#ifdef NFC_DEMO_DEBUG
    Serial.print("Read card #");
    Serial.println(id);
    scale.set_scale(calibration_factor); //Adjust to this calibration factor

    while (1) {
      if (check == 0) {
        int reading = analogRead(outputpin);  // 센서로 부터 자료값을 받는다.
        float voltage = reading * 5.0 / 1024.0;
        float celsiustemp = (voltage - 0.5) * 100;
        // 입력받은 자료값을 수정하여 필요한 자료값으로 바꾼다.

        Serial.print(celsiustemp);
        Serial.println(" Celsius");
        // 수정하여 나온 자료값을 출력한다.(섭씨 출력)
        delay(100);
        count += 0.1;
        endTime += 0.1;
        if (EndArduino(endTime)) {
          return;
        }

        if (celsiustemp >= 38) {
          check = 1;
        }
      }
      else {
        Serial.print("Reading: ");
        Serial.print(scale.get_units()*0.453592, 2);
        Serial.print(" kg"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
        Serial.print(" calibration_factor: ");
        Serial.print(calibration_factor);
        Serial.println();

        if (scale.get_units()*0.453592 >= 4) {
          String cmd = "AT+CIPSTART=\"TCP\",\"";
          cmd += DST_IP;
          cmd += "\",80";
          Serial.println(cmd);
          dbgSerial.println(cmd);
          if (dbgSerial.find("Error"))
          {
            Serial.println("TCP connect error");
            return;
          }

          for (int i = 0; i < 10; i++) {
            cmd = "GET /run.php?NFCID=" + String(id, DEC) + "&att=" + att;
            dbgSerial.print("AT+CIPSEND=");
            dbgSerial.println(cmd.length());

            Serial.println(cmd);

            if (dbgSerial.find(">"))
            {
              Serial.print(">");
              break;
            }
            else
            {
              dbgSerial.println("AT+CIPCLOSE");
              Serial.println("connect timeout");
              delay(1000);
              if (i == 9){
                return;              
              }
            }
          }
          Serial.print(cmd);
          dbgSerial.print(cmd);
          delay(2000);
          //Serial.find("+IPD");  
          while (Serial.available())
          {
            char c = Serial.read();
            dbgSerial.write(c);
            if (c == '\r') dbgSerial.print('\n');
          }
          Serial.println("====");

          check = 0;
          // 15분동안 정지
          count = 0;
          for (int i = 0; i < 90000; i++) {
            delay(1);
            endTime++;
            if (EndArduino(endTime)) {
              return;
            }
          }
        }
        delay(100);
        count += 0.1;
        endTime += 0.1;
        if (EndArduino(endTime)) {
          return;
        }
      }
      att = attendance(count);
    }

#endif
  }
  delay(1000);
}

boolean connectWiFi()
{
  //dbgSerial.println("AT+CWMODE=1");  

  String cmd = "AT+CWJAP=\"";
  cmd += SSID;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"";
  dbgSerial.println(cmd);
  Serial.println(cmd);
  delay(3000);

  if (dbgSerial.find("OK"))
  {
    Serial.println("OK, Connected to WiFi.");
    return true;
  }
  else
  {
    Serial.println("Can not connect to the WiFi.");
    return false;
  }
}

char attendance(float count) {
  if (count > 1800000) {
    return '2';
  }
  else if (count > 900000) {
    return '1';
  }
  else {
    return '0';
  }
}

boolean EndArduino(float endTime) {
  if (endTime > 10800000) {
    return true;
  }
  else {
    return false;
  }
}
