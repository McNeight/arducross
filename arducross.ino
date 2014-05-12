/*
 Welcome to the ECU Reader project. This sketch uses the Canbus library.
 It requires the CAN-bus shield for the Arduino. This shield contains the MCP2515 CAN controller
 and the MCP2551 CAN-bus driver.
 A connector for an EM406 GPS receiver and an uSDcard holder with 3v level convertor for use in
 data logging applications.
 The output data can be displayed on a serial LCD.

 The SD test functions requires a FAT16 formated card with a text file of WRITE00.TXT in the card.


 SK Pang Electronics www.skpang.co.uk
 v4.0 04-03-12 Updated for Arduino 1.0
 v3.0 21-02-11  Use library from Adafruit for sd card instead.

 */
#include <Canbus.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include "WWH_OBD.h"

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the backlight color
#define OFF 0x0
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

char buffer[100];  //Data will be temporarily stored to this buffer before being written to the file

int read_size = 0; //Used as an indicator for how many characters are read from the file
int count = 0;     //Miscellaneous variable

int D10 = 10;

int LED1 = 13;
int LED2 = 8;
int LED3 = 7;

uint8_t buttons;
bool dash = true;

char *parseptr;
char buffidx;
uint8_t hour, minute, second, year, month, date;
uint8_t groundspeed, trackangle;
char status;
uint32_t waypoint = 0;

void setup() {
  // Debugging output
  Serial.begin(115200);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  digitalWrite(LED2, LOW);

  Serial.println("ECU Reader");  /* For debug use */

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setBacklight(WHITE);

  lcd.print("U:Vehicle Info");
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  lcd.print(F("D:Dashboard"));

  while (1)
  {
    buttons = lcd.readButtons();

    if (buttons & BUTTON_DOWN) {
      dash = true;
      lcd.print("DASH");
      Serial.println("DASH");
      break;
    }
    else if (buttons & BUTTON_UP)
    {
      dash = false;
      lcd.print("INFO");
      Serial.println("INFO");
      break;
    }

    delay(1);
  }

  lcd.clear();

  if (Canbus.init(CANSPEED_500)) /* Initialise MCP2515 CAN controller at the specified speed */
  {
    lcd.print("CAN Init ok");
  }
  else
  {
    lcd.print("Can't init CAN");
  }

  delay(1000);

  lcd.clear();
}


void loop() {

  if (dash)
  {
    dashboard();
  }

  delay(10);
}

uint32_t parsedecimal(char *str) {
  uint32_t d = 0;

  while (str[0] != 0) {
    if ((str[0] > '9') || (str[0] < '0'))
      return d;
    d *= 10;
    d += str[0] - '0';
    str++;
  }
  return d;
}

void dashboard()
{
  if (Canbus.ecu_req(PID_RPM, buffer) == 1)        /* Request for engine RPM */
  {
    lcd.setCursor(0, 0);
    lcd.print(buffer);                         /* Display data on LCD */
  }
  digitalWrite(LED2, HIGH);

  if (Canbus.ecu_req(PID_SPEED, buffer) == 1)
  {
    lcd.setCursor(9, 0);
    lcd.print(buffer);
  }

  if (Canbus.ecu_req(PID_ECT, buffer) == 1)
  {
    lcd.setCursor(0, 1);
    lcd.print(buffer);
  }

  if (Canbus.ecu_req(PID_TP, buffer) == 1)
  {
    lcd.setCursor(9, 1);
    lcd.print(buffer);
    //file.print(buffer);
  }
  //  Canbus.ecu_req(O2_VOLTAGE,buffer);

  digitalWrite(LED2, LOW);
  delay(100);
}

void init_fail()
{
  // If none of the protocol tests shown above succeeds, the equipment shall repeat all of them
  // and advise the user:
  //    a. That communication with the vehicle could not be established,
  //    b. To confirm that the ignition key is in the “ON” position,
  //    c. To check the emissions label or vehicle service information to confirm that
  //       the vehicle is OBD equipped,
  //    d. To confirm that the external test equipment is connected to the vehicle correctly.
  // The equipment shall continue to repeat the protocol tests shown above until either one of them
  // passes or the user chooses to abandon the attempt. The equipment may also indicate the number
  // of failed initialization attempts to the user.
}





