#include <PN532.h>
#include <SPI.h>
#include <HX711.h>


/*Chip select pin can be connected to D10 or D9 which is hareware optional*/
/*if you the version of NFC Shield from SeeedStudio is v2.0.*/
#define PN532_CS 10
PN532 nfc(PN532_CS);
#define  NFC_DEMO_DEBUG 1

int outputpin = A5; // 센서와 연결된 아날로그 핀 설정

#define DOUT  3
#define CLK  2
int check = 0;

HX711 scale(DOUT, CLK);

float calibration_factor = -45860; //-7050 worked for my 440lb max scale setup


void setup(void) {
#ifdef NFC_DEMO_DEBUG
  Serial.begin(9600);
  Serial.println("Hello!");
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
  // look for MiFare type cards
  id = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A);

  if (id != 0) {
#ifdef NFC_DEMO_DEBUG
    Serial.print("Read card #");
    Serial.println(id);

    if (id == 855132288) {
      scale.set_scale(calibration_factor); //Adjust to this calibration factor

      if (check == 0) {
        int reading = analogRead(outputpin);  // 센서로 부터 자료값을 받는다.
        float voltage = reading * 5.0 / 1024.0;
        float celsiustemp = (voltage - 0.5) * 100;
        // 입력받은 자료값을 수정하여 필요한 자료값으로 바꾼다.

        Serial.print(celsiustemp);
        Serial.println(" Celsius");
        // 수정하여 나온 자료값을 출력한다.(섭씨 출력)
        delay(100);

        if (celsiustemp >= 30) {
          check = 1;
        }
      }
      else {
        Serial.print("Reading: ");
        Serial.print(scale.get_units()*0.453592, 5);
        Serial.print(" kg"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
        Serial.print(" calibration_factor: ");
        Serial.print(calibration_factor);
        Serial.println();
      }

      delay(100);
    }
#endif
  }
  delay(1000);
}



