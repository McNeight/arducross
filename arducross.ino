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
#include "Ford_OBD.h"

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

#define bitrate CAN_BPS_500K // define CAN speed (bitrate)

/* 
  Second we create CANbus object (CAN channel) and select SPI CS Pin. Do not use "CAN" by itself as it will cause compile errors.
  Can't use CAN0 or CAN1 as variable names, as they are defined in hardware/arduino/sam/system/CMSIS/Device/ATMEL/sam3xa/include/sam3x8e.h
  Needs to be CANbus0, CANbus1, CANbus2, or whatever name you want to give that channel. This can also allow us to create more channels
  using more MCP2515s as long as we use different SPI CS to control data.
 */

#if defined(ARDUINO_ARCH_AVR)
// Can't use CAN0 or CAN1 as variable names, as they are defined in
CAN_MCP2515 CANbus(10); // Create CAN channel using pin 10 for SPI chip select
#elif defined(ARDUINO_ARCH_SAM)
// Can't use CAN0 or CAN1 as variable names, as they are defined in
CAN_SAM3X8E CANbus(0);  // Create CAN channel on CAN bus 0
//CAN1.init(SystemCoreClock, CAN_BPS_500K);
#else
#error This library only supports boards with an AVR or SAM processor.
#endif

SoftwareSerial gps =  SoftwareSerial(8, 7);

WWH_OBD   OBD;
Ford_OBD FOBD;

const byte buffer_size = 100;
char buffer[buffer_size];  //Data will be temporarily stored to this buffer before being written to the file
char tempbuf[15];
char lat_str[14];
char lon_str[14];

int read_size = 0; //Used as an indicator for how many characters are read from the file
int count = 0;     //Miscellaneous variable

uint8_t buttons;
bool dash = false;
bool info = false;
bool ford = false;
// Ford transmission related info
int engine_torque; // 1E00
uint32_t shifter_status; // 1E03
uint8_t gear_commanded; // 1E12
uint8_t gear_engaged; // 1E1F

char *parseptr;
char buffidx;
uint8_t hour, minute, second, year, month, date;
uint32_t latitude, longitude;
uint8_t groundspeed, trackangle;
char latdir, longdir;
char status;
uint32_t waypoint = 0;

uint8_t selected_pid = PID_RPM;

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

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setBacklight(WHITE);

  Serial.println("Arducross");  /* For debug use */

  mainMenu();

  CAN1.begin(bitrate); //Set CAN mode and speed.

//  if (CAN1.readMode() == MCP2515_MODE_NORMAL) // Check to see if we set the Mode and speed correctly. For debugging purposes only.
//  {
//    Serial.println(F("CAN Initialization complete"));
//    lcd.print(F("Init complete"));
//  }
//  else
//  {
//    Serial.println(F("CAN Initialization failed"));
//    lcd.print(F("Init failed"));
//    init_fail();
//  }

//  lcd.setCursor(0, 1);
//  if (CAN1.readRate() == bitrate)
//  {
//    Serial.print (F("CAN speed set to: "));
//    lcd.print(F("Speed: "));
//    Serial.print(bitrate);
//    lcd.print(bitrate);
//    Serial.println (F("kbit/s"));
//    lcd.print(F("kbit/s"));
//  }
//  else
//  {
//    Serial.println(F("CAN speed failed"));
//    lcd.print(F("Speed failed"));
//    init_fail();
//  }
//
//  delay(2000);

  lcd.clear();
}

/*

*/
void loop() {

  if (dash)
  {
    dashboard();
  }
  else if (info)
  {
    vehicleInfo();
  }
  else if (ford)
  {
    fordInfo();
  }
  else
  {
    mainMenu();
  }

  delay(25);
}

/*

*/
void supportedPIDs()
{
  bool supportedList[20][8] = {0};
}

/*

*/
void mainMenu()
{
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
      ford = false;
      lcd.print(F("DASH"));
      Serial.println(F("DASH"));
      break;
    }
    else if (buttons & BUTTON_UP)
    {
      dash = false;
      info = true;
      ford = false;
      lcd.print(F("INFO"));
      Serial.println(F("INFO"));
      break;
    }
    else if (buttons & BUTTON_LEFT)
    {
      dash = false;
      info = false;
      ford = true;
      lcd.print(F("FORD"));
      Serial.println(F("FORD"));
      break;
    }

    delay(5);
  }

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
void query_ecu(byte sid, byte pid)
{
  byte J1979_data[] = {0x02, sid, pid, 0x00, 0x00, 0x00, 0x00, 0x00};
  CAN1.write(ID_REQUEST, CAN_BASE_FRAME, 8, J1979_data);
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
    Serial.print(F("selected_pid = 0x"));
    Serial.println(selected_pid, HEX);
  }
  else if (buttons & BUTTON_RIGHT)
  {
    selected_pid++;
    lcd.clear();
    Serial.print(F("selected_pid = 0x"));
    Serial.println(selected_pid, HEX);
  }
  else if (buttons & BUTTON_DOWN)
  {
    lcd.clear();
    info = false;
    dash = false;
    ford = false;
    return;
  }

  query_ecu(SIDRQ_DIAG, selected_pid);

  if (CAN1.available() == true) {          // Check to see if a valid message has been received.

    //CAN1.read(&message);                     //read message, it will follow the J1939 structure of ID,Priority, source address, destination address, DLC, PGN,
    CAN1.read(&can_ID, &can_length, can_data);                        // read Message and assign data through reference operator &

    if (can_ID == ID_REPLY_1)
    {
      decoded_pid = OBD.decodePID(can_data, buffer);
      //      lcd.print("ID:");
      //      lcd.print(can_ID, HEX);
      lcd.print(F("Len:"));
      lcd.print(can_length);                           // Displays message length
      lcd.print(F(" PID:"));
      if (decoded_pid < 0x10)                   //Adds a leading zero
      {
        lcd.print("0");
      }
      lcd.print(decoded_pid, HEX);           //Display PID

      lcd.setCursor(0, 1);
      lcd.print(buffer);

      //#define SERIAL_DEBUG
#ifdef SERIAL_DEBUG
      Serial.print(F("Time | "));
      Serial.print(millis());
      Serial.print(F(" | ID"));
      Serial.print(F(" | "));
      Serial.print(can_ID, HEX);                                // Displays received ID
      Serial.print(F(" | "));
      Serial.print(F("Data Length"));
      Serial.print(F(" | "));
      Serial.print(can_length, HEX);                           // Displays message length
      Serial.print(F(" | "));
      Serial.print(F("Data"));
      for (byte i = 0; i < can_length; i++) {
        Serial.print(" | ");
        if (can_data[i] < 0x10)                                 // If the data is less than 10 hex it will assign a zero to the front as leading zeros are ignored...
        {
          Serial.print(F("0"));
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
    lcd.print(F("Len:"));
    lcd.print(can_length);                           // Displays message length
    lcd.print(F(" PID:"));
    if (decoded_pid < 0x10)                   //Adds a leading zero
    {
      lcd.print(F("0"));
    }
    lcd.print(decoded_pid, HEX);           //Display PID
#else
    lcd.print(F("NO CAN MESSAGE"));
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
  else if (buttons & BUTTON_UP)
  {
    lcd.clear();
    info = false;
    dash = false;
    ford = false;
    return;
  }

  query_ecu(SIDRQ_DIAG, PID_APP_R);

  if (CAN1.available() == true) {          // Check to see if a valid message has been received.

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
    //strlcpy(buffer, "NO APP", buffer_size);
  }
  //Serial.println("Done with RPM");

  // Add a space between values
  strlcat(buffer, " ", buffer_size);

  query_ecu(SIDRQ_DIAG, PID_TP_R);

  if (CAN1.available() == true) {          // Check to see if a valid message has been received.

    //CAN1.read(&message);                     //read message, it will follow the J1939 structure of ID,Priority, source address, destination address, DLC, PGN,
    CAN1.read(&can_ID, &can_length, can_data);                        // read Message and assign data through reference operator &

    if (can_ID == ID_REPLY_1)
    {
      decoded_pid = OBD.decodePID(can_data, buffer);
    }
  }
  else
  {
    //strlcat(buffer, "NO TP", buffer_size);
  }

  lcd.print(buffer);

  lcd.setCursor(0, 1);
  buffer[0] = 0;

  query_ecu(SIDRQ_DIAG, PID_RPM);

  if (CAN1.available() == true) {          // Check to see if a valid message has been received.

    //CAN1.read(&message);                     //read message, it will follow the J1939 structure of ID,Priority, source address, destination address, DLC, PGN,
    CAN1.read(&can_ID, &can_length, can_data);                        // read Message and assign data through reference operator &

    if (can_ID == ID_REPLY_1)
    {
      decoded_pid = OBD.decodePID(can_data, buffer);
    }
  }
  else
  {
    //strlcpy(buffer, "NO RPM", buffer_size);
  }

  // Add a space between values
  strlcat(buffer, " ", buffer_size);

  query_ecu(SIDRQ_DIAG, PID_LOAD_PCT);

  if (CAN1.available() == true) {          // Check to see if a valid message has been received.

    //CAN1.read(&message);                     //read message, it will follow the J1939 structure of ID,Priority, source address, destination address, DLC, PGN,
    CAN1.read(&can_ID, &can_length, can_data);                        // read Message and assign data through reference operator &

    if (can_ID == ID_REPLY_1)
    {
      decoded_pid = OBD.decodePID(can_data, buffer);
    }
  }
  else
  {
    //strlcat(buffer, "NO LOAD", buffer_size);
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

/*

*/
void fordInfo()
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
    lcd.clear();
    info = false;
    dash = false;
    ford = false;
    return;
  }
  else if (buttons & BUTTON_UP)
  {
    byte J1979_data[] = {0x03, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    CAN1.write(ID_REQUEST, CAN_BASE_FRAME, 8, J1979_data);

    //query_ecu(SIDRQ_DIAG, 0x00);
  }

  if (CAN1.available() == true) {          // Check to see if a valid message has been received.

    //CAN1.read(&message);                     //read message, it will follow the J1939 structure of ID,Priority, source address, destination address, DLC, PGN,
    CAN1.read(&can_ID, &can_length, can_data);                        // read Message and assign data through reference operator &

    if (can_ID == ID_REPLY_1)
    {
      decoded_pid = OBD.decodePID(can_data, buffer);

      lcd.clear();
      lcd.print(F("Len:"));
      Serial.print(F("CAN Length:"));
      lcd.print(can_length);                           // Displays message length
      Serial.print(can_length);
      lcd.print(F(" PID:"));
      Serial.print(F(" PID:"));
      if (decoded_pid < 0x10)                   //Adds a leading zero
      {
        lcd.print(F("0"));
        Serial.print(F("0"));
      }
      lcd.print(decoded_pid, HEX);           //Display PID
      Serial.println(decoded_pid, HEX);
      lcd.setCursor(0, 1);
      lcd.print(buffer);
      Serial.println(buffer);
    }
  }
  else
  {
    //Serial.println("No CAN message available");
    //strlcpy(buffer, "NO APP", buffer_size);
  }
}

/*

*/
void initialization()
{
}

/*

*/
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

