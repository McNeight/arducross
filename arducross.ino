/*

 */
#include <Arduino.h>
#include <CAN.h>
#if defined(ARDUINO_ARCH_AVR)
#include <SPI.h>
#include <CAN_MCP2515.h>
#elif defined(ARDUINO_ARCH_SAM)
#include <variant.h>
#include <CAN_SAM3X8E.h>
#else
#error “This library only supports boards with an AVR or SAM processor.”
#endif

//#include <SD.h>
#include <SoftwareSerial.h>
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

// set up variables using the SD utility library functions:
//Sd2Card card;
//SdVolume volume;
//SdFile root;

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
const int chipSelect = 10;

// First we define our CAN mode and rate.

#define bitrate 500 // define CAN speed (bitrate)

/*
  Second we create CAN1 object (CAN channel) and select SPI CS Pin. Do not use "CAN" by itself as it will cause compile errors.
  Needs to be CAN0, CAN1, CAN2, or whatever name you want to give that channel. This can also allow us to create more channels
  using more MCP2515s as long as we use different SPI CS to control data.
 */

#if defined(ARDUINO_ARCH_AVR)
CAN_MCP2515 CAN1(chipSelect); //Create CAN Channel
#elif defined(ARDUINO_ARCH_SAM)
CAN_SAM3X8E CAN1;
CAN1.init(SystemCoreClock, CAN_BPS_1000K);
#else
#error “This library only supports boards with an AVR or SAM processor.”
#endif

WWH_OBD OBD;

const byte buffer_size = 100;
char buffer[buffer_size];  //Data will be temporarily stored to this buffer before being written to the file
char tempbuf[15];
char lat_str[14];
char lon_str[14];


int read_size = 0; //Used as an indicator for how many characters are read from the file
int count = 0;     //Miscellaneous variable

int LED1 = 13;
int LED2 = 8;
int LED3 = 7;

uint8_t buttons;
bool dash = false;
bool info = false;

SoftwareSerial gps =  SoftwareSerial(8, 7);
char *parseptr;
char buffidx;
uint8_t hour, minute, second, year, month, date;
uint32_t latitude, longitude;
uint8_t groundspeed, trackangle;
char latdir, longdir;
char status;
uint32_t waypoint = 0;

volatile uint8_t selected_pid = PID_RPM;

/*

*/
void setup() {
  // Open serial communications and wait for port to open:
  // Debugging output
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  //  gps.begin(9600);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  digitalWrite(LED2, LOW);

  Serial.println("ECU Reader");  /* For debug use */

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setBacklight(WHITE);

  lcd.print(F("U:Vehicle Info"));
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  lcd.print(F("D:Dashboard"));

  while (1)
  {
    buttons = lcd.readButtons();

    if (buttons & BUTTON_DOWN) {
      dash = true;
      info = false;
      lcd.print("DASH");
      Serial.println("DASH");
      break;
    }
    else if (buttons & BUTTON_UP)
    {
      dash = false;
      info = true;
      lcd.print("INFO");
      Serial.println("INFO");
      break;
    }

    delay(5);
  }

  lcd.clear();

  CAN1.begin(bitrate); //Set CAN mode and speed.

  if (CAN1.readMode() == MCP2515_MODE_NORMAL) // Check to see if we set the Mode and speed correctly. For debugging purposes only.
  {
    Serial.println("CAN Initialization complete");
    lcd.print("Init complete");
  }
  else
  {
    Serial.println("CAN Initialization failed");
    lcd.print("Init failed");
    init_fail();
  }

  lcd.setCursor(0, 1);
  if (CAN1.readRate() == bitrate)
  {
    Serial.print ("CAN speed set to: ");
    lcd.print("Speed: ");
    Serial.print(bitrate);
    lcd.print(bitrate);
    Serial.println ("kbit/s");
    lcd.print("kbit/s");
  }
  else
  {
    Serial.println("CAN speed failed");
    lcd.print("Speed failed");
    init_fail();
  }

  delay(2000);

  lcd.clear();
}

/*

*/
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

/*

*/
void query_ecu(unsigned char sid, unsigned char pid)
{
  byte J1979_data[] = {0x02, sid, pid, 0x00, 0x00, 0x00, 0x00, 0x00};
  CAN1.send(ID_REQUEST, stdID, 8, J1979_data);
}

/*

*/
void vehicleInfo()
{
  byte decoded_pid;
  unsigned long can_ID;                                       // assign a variable for Message ID
  byte can_length;                                            //assign a variable for length
  byte can_data[8];                                           //assign an array for data

  lcd.home();
  buffer[0] = 0;

  buttons = lcd.readButtons();

  if (buttons & BUTTON_LEFT) {
    selected_pid--;
    lcd.clear();
    Serial.print("selected_pid = 0x");
    Serial.println(selected_pid, HEX);
  }
  else if (buttons & BUTTON_RIGHT)
  {
    selected_pid++;
    lcd.clear();
    Serial.print("selected_pid = 0x");
    Serial.println(selected_pid, HEX);
  }

  query_ecu(SIDRQ_DIAG, selected_pid);

  if (CAN1.msgAvailable() == true) {          // Check to see if a valid message has been received.

    //CAN1.read(&message);                     //read message, it will follow the J1939 structure of ID,Priority, source address, destination address, DLC, PGN,
    CAN1.read(&can_ID, &can_length, can_data);                        // read Message and assign data through reference operator &

    if (can_ID == ID_REPLY_1)
    {
      decoded_pid = OBD.decodePID(can_data, buffer);
      //      lcd.print("ID:");
      //      lcd.print(can_ID, HEX);
      lcd.print("Len:");
      lcd.print(can_length);                           // Displays message length
      lcd.print(" PID:");
      if (decoded_pid < 0x10)                   //Adds a leading zero
      {
        lcd.print("0");
      }
      lcd.print(decoded_pid, HEX);           //Display PID

      lcd.setCursor(0, 1);
      lcd.print(buffer);

      //#define SERIAL_DEBUG
#ifdef SERIAL_DEBUG
      Serial.print("Time | ");
      Serial.print(millis());
      Serial.print(" | ID");
      Serial.print(" | ");
      Serial.print(can_ID, HEX);                                // Displays received ID
      Serial.print(" | ");
      Serial.print("Data Length");
      Serial.print(" | ");
      Serial.print(can_length, HEX);                           // Displays message length
      Serial.print(" | ");
      Serial.print("Data");
      for (byte i = 0; i < can_length; i++) {
        Serial.print(" | ");
        if (can_data[i] < 0x10)                                 // If the data is less than 10 hex it will assign a zero to the front as leading zeros are ignored...
        {
          Serial.print("0");
        }
        Serial.print(can_data[i], HEX);                         // Displays message data

      }
      Serial.println();                                     // adds a line
#endif
    }
  }
  else
  {
#define LCD_FIT_TEST
#ifdef LCD_FIT_TEST
    can_ID = ID_REPLY_1;
    can_length = 8;
    can_data[0] = 0x04;
    can_data[1] = SIDPR_DIAG;
    can_data[2] = selected_pid;
    can_data[3] = 0x7F;
    can_data[4] = 0xFF;
    can_data[5] = 0x00;
    can_data[6] = 0x00;
    can_data[7] = 0x00;

    decoded_pid = OBD.decodePID(can_data, buffer);
    //    lcd.print("ID:");
    //    lcd.print(can_ID, HEX);
    lcd.print("Len:");
    lcd.print(can_length);                           // Displays message length
    lcd.print(" PID:");
    if (decoded_pid < 0x10)                   //Adds a leading zero
    {
      lcd.print("0");
    }
    lcd.print(decoded_pid, HEX);           //Display PID
#else
    lcd.print("NO CAN MESSAGE");
#endif
    lcd.setCursor(0, 1);
    lcd.print(buffer);
  }

}

/*

*/
void dashboard()
{
  byte decoded_pid;
  unsigned long can_ID;                                       // assign a variable for Message ID
  byte can_length;                                            //assign a variable for length
  byte can_data[8];                                           //assign an array for data

  lcd.home();
  buffer[0] = 0;

  buttons = lcd.readButtons();

  if (buttons & BUTTON_LEFT) {
    //    selected_pid--;
    //    lcd.clear();
    //    Serial.print("selected_pid = 0x");
    //    Serial.println(selected_pid, HEX);
  }
  else if (buttons & BUTTON_RIGHT)
  {
    //    selected_pid++;
    //    lcd.clear();
    //    Serial.print("selected_pid = ");
    //    Serial.println(selected_pid);
  }

  query_ecu(SIDRQ_DIAG, PID_APP_R);

  if (CAN1.msgAvailable() == true) {          // Check to see if a valid message has been received.

    //CAN1.read(&message);                     //read message, it will follow the J1939 structure of ID,Priority, source address, destination address, DLC, PGN,
    CAN1.read(&can_ID, &can_length, can_data);                        // read Message and assign data through reference operator &

    if (can_ID == ID_REPLY_1)
    {
      decoded_pid = OBD.decodePID(can_data, buffer);
    }
  }
  else
  {
    //Serial.println("No CAN message available");
    strlcpy(buffer, "NO APP", buffer_size);
  }
  //Serial.println("Done with RPM");

  // Add a space between values
  strlcat(buffer, " ", buffer_size);

  query_ecu(SIDRQ_DIAG, PID_TP_R);

  if (CAN1.msgAvailable() == true) {          // Check to see if a valid message has been received.

    //CAN1.read(&message);                     //read message, it will follow the J1939 structure of ID,Priority, source address, destination address, DLC, PGN,
    CAN1.read(&can_ID, &can_length, can_data);                        // read Message and assign data through reference operator &

    if (can_ID == ID_REPLY_1)
    {
      decoded_pid = OBD.decodePID(can_data, buffer);
    }
  }
  else
  {
    strlcat(buffer, "NO TP", buffer_size);
  }

  lcd.print(buffer);

  lcd.setCursor(0, 1);
  buffer[0] = 0;

  query_ecu(SIDRQ_DIAG, PID_RPM);

  if (CAN1.msgAvailable() == true) {          // Check to see if a valid message has been received.

    //CAN1.read(&message);                     //read message, it will follow the J1939 structure of ID,Priority, source address, destination address, DLC, PGN,
    CAN1.read(&can_ID, &can_length, can_data);                        // read Message and assign data through reference operator &

    if (can_ID == ID_REPLY_1)
    {
      decoded_pid = OBD.decodePID(can_data, buffer);
    }
  }
  else
  {
    strlcpy(buffer, "NO RPM", buffer_size);
  }

  // Add a space between values
  strlcat(buffer, " ", buffer_size);

  query_ecu(SIDRQ_DIAG, PID_LOAD_PCT);

  if (CAN1.msgAvailable() == true) {          // Check to see if a valid message has been received.

    //CAN1.read(&message);                     //read message, it will follow the J1939 structure of ID,Priority, source address, destination address, DLC, PGN,
    CAN1.read(&can_ID, &can_length, can_data);                        // read Message and assign data through reference operator &

    if (can_ID == ID_REPLY_1)
    {
      decoded_pid = OBD.decodePID(can_data, buffer);
    }
  }
  else
  {
    strlcat(buffer, "NO LOAD", buffer_size);
  }

  lcd.print(buffer);

  // in order, show:
  // pedal posistion
  //   PID_APP_D
  //   PID_APP_E
  //   PID_APP_F
  // throttle position
  //
  // engine rpm
  // speed

}

void init_fail()
{
  // If none of the protocol tests shown above succeeds, the equipment shall repeat all of them
  // and advise the user:
  //    a. That communication with the vehicle could not be established,
  //    b. To confirm that the ignition key is in the â€œONâ€� position,
  //    c. To check the emissions label or vehicle service information to confirm that
  //       the vehicle is OBD equipped,
  //    d. To confirm that the external test equipment is connected to the vehicle correctly.
  // The equipment shall continue to repeat the protocol tests shown above until either one of them
  // passes or the user chooses to abandon the attempt. The equipment may also indicate the number
  // of failed initialization attempts to the user.

  while (1);
}

void loop() {

  if (dash)
  {
    dashboard();
  }
  else if (info)
  {
    vehicleInfo();
  }

  delay(25);
}

