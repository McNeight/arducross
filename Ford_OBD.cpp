//#include <stdio.h>
//#include "WWH_OBD.h"
#include "Ford_OBD.h"

Ford_OBD::Ford_OBD()
{
  // Set CAN Controller to 500K

  // Listen for EEC1 and assess frequency of reception

}

/*

*/
byte* Ford_OBD::encodeQuery(uint16_t pid)
{
  byte J1979_data[] = {0x03, 0x22, pid , 0x00, 0x00, 0x00, 0x00, 0x00};
  //  CAN1.send(ID_REQUEST, stdID, 8, J1979_data);
  return J1979_data;
}

/*

*/
uint16_t Ford_OBD::decodePID(byte* data, char* text)
{
  byte decode_buffer_size = 17;
  byte additional_data_bytes = data[0];
  uint16_t pid = ((data[2] << 8) + data[3]);
  char decode_buffer[decode_buffer_size];
  double decode_double;
  float decode_float;
  int decode_int;

  switch (pid)
  {
    case FORD_PID_GEAR:
      // (A*0.501) = 0-127
      decode_int = data[4] * 0.501;
      //sprintf(decode_buffer, "%5.2fRPM", decode_float);
      dtostrf(decode_float, 5, 2, decode_buffer);
      strlcat(decode_buffer, "RPM", decode_buffer_size);
      break;

    case FORD_PID_TRD:
      // A = Gear
      decode_int = data[4];
      snprintf(decode_buffer, decode_buffer_size, "%d", decode_int);
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

