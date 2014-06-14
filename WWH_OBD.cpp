/*
 World Wide Harmonized On-Board Diagnostics (WWH OBD)
 by:
 date:
 license:

 Contains defines and algorithms spelled out in various standards documents including:
 SAE J1939      Recommended Practice for Serial Control and Communications Vehicle Network
 SAE J1939-03   On Board Diagnostics Implementation Guide
 SAE J1978      OBD II Scan Tool - Equivalent to ISO/DIS 15031-4:December 14, 2001
 SAE J1979      E/E Diagnostic Test Modes
 ISO 15031-5    Road vehicles - Communication between vehicle and external equipment for
 emissions-related diagnostics - Part 5: Emissions-related diagnostic services
 ISO 15765-4    Road vehicles - Diagnostics on controller area network (CAN) - Part 4:
 Requirements for emissions-related systems
 ISO 27145-4    Road vehicles - Implementation of WWH-OBD communication requirements - Part 4:
 Connection between vehicle and test equipment

 ISO 15031 Definitions:
 DLC = Data Length Code
 DTC = Diagnostic Trouble Code
 ECM = Engine Control Module
 ECU = Electronic Control Module
 FTB = Failure Type Byte
 KWP = Key Word Protocol (ISO 14230)
 MIL = Malfunction Indicator Lamp
 NRC = Negative Response Code
 PCI = Protocol Control Information
 PID = Parameter ID (similar to DID or LID)
 SID = Service ID
 */
 

//#include <stdio.h>
#include "WWH_OBD.h"

/*

*/
WWH_OBD::WWH_OBD()
{
  // Set CAN Controller to 250K

  // Listen for EEC1 and assess frequency of reception

}

/*

*/
byte* WWH_OBD::encodeQuery(unsigned char sid, unsigned char pid)
{
  byte J1979_data[] = {0x02, sid, pid, 0x00, 0x00, 0x00, 0x00, 0x00};
  //  CAN1.send(ID_REQUEST, stdID, 8, J1979_data);
  return J1979_data;
}

/*

*/
byte WWH_OBD::decodePID(byte* data, char* text)
{
  byte decode_buffer_size = 17;
  byte pid = data[2];
  char decode_buffer[decode_buffer_size];
  double decode_double;
  float decode_float;
  int decode_int;

  switch (pid)
  {
    case PID_FUELSYS:
      snprintf(decode_buffer, decode_buffer_size, "BITFIELD");
      break;
    case PID_ECT:
      // A - 40 [degree C]
      decode_int = data[3] - 40;
      snprintf(decode_buffer, decode_buffer_size, "%d*C", decode_int);
      break;

    case PID_RPM:
      // ((A*256)+B)/4 [RPM]
      decode_float = ((data[3] << 8) + data[4]) / 4.0;
      //sprintf(decode_buffer, "%5.2fRPM", decode_float);
      dtostrf(decode_float, 5, 2, decode_buffer);
      strlcat(decode_buffer, "RPM", decode_buffer_size);
      break;

    case PID_SPEED:
      // A [km]
      decode_int = data[3];
      snprintf(decode_buffer, decode_buffer_size, "%dkm", decode_int);
      break;

    case PID_MAF:
      // ((256*A)+B) / 100  [g/s]
      decode_int = ((data[3] << 8) + data[4]) / 100;
      snprintf(decode_buffer, decode_buffer_size, "%dg/s", decode_int);
      break;

    case PID_O2S11:
      // A * 0.005   (B-128) * 100/128 (if B==0xFF, sensor is not used in trim calc)
      decode_int = data[3] * 0.005;
      snprintf(decode_buffer, decode_buffer_size, "%dV", decode_int);
      break;

    case PID_LOAD_PCT:
    case PID_TP:				// Throttle Position
    case PID_LOAD_ABS:
    case PID_TP_R:
    case PID_TP_B:
    case PID_TP_C:
    case PID_APP_D:
    case PID_APP_E:
    case PID_APP_F:
    case PID_TAC_PCT:
    case PID_ALCH_PCT:
    case PID_APP_R:
    case PID_BAT_PWR:
      decode_float = (data[3] * 100.0) / 255.0;
      //sprintf(decode_buffer, "%3.1f%%", decode_float);
      dtostrf(decode_float, 3, 1, decode_buffer);
      strlcat(decode_buffer, "%", decode_buffer_size);
      break;
    default:
      // Print out all the data
      decode_buffer[0] = 0;
      for (byte i = 0; i < 8; i++)
      {
        char temp_buffer[3] = {0};
        if (data[i] < 0x10)                                 // If the data is less than 10 hex it will assign a zero to the front as leading zeros are ignored...
        {
          strlcat(decode_buffer, "0", decode_buffer_size);
        }
        snprintf(temp_buffer, 3, "%X", data[i]);
        strlcat(decode_buffer, temp_buffer, decode_buffer_size);
      }
      break;
  }

  // Use strlcat instead of strlcpy to allow for building up a string with multiple calls.
  strlcat(text, decode_buffer, decode_buffer_size);

  return pid;
}

