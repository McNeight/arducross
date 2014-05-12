#include <stdio.h>
#include <Canbus.h>
#include "WWH_OBD.h"

WWH_OBD::WWH_OBD()
{
  // Set CAN Controller to 250K

  // Listen for EEC1 and assess frequency of reception

}

char* WWH_OBD::decodePID(tCAN messageIn)
{
  char buffer[16];
  double engine_double;
  float engine_float;
  int engine_int;

  switch (messageIn.data[2])
  {
      // A - 40 [degree C]
    case PID_ECT:
      engine_int =  messageIn.data[3] - 40;
      snprintf(buffer, 16, "%d °C", engine_int);
      break;
      // ((A*256)+B)/4 [RPM]
    case PID_RPM:
      engine_float =  ((messageIn.data[3] << 8) + messageIn.data[4]) / 4.0;
      sprintf(buffer, "%5.2f RPM", engine_float);
      break;
      // A [km]
    case PID_SPEED:
      engine_int =  messageIn.data[3];
      sprintf(buffer, "%d km ", engine_int);
      break;

    case PID_MAF:   			// ((256*A)+B) / 100  [g/s]
      engine_int =  ((messageIn.data[3] << 8) + messageIn.data[4]) / 100;
      sprintf(buffer, "%d g/s", engine_int);
      break;

    case PID_O2S11:    		// A * 0.005   (B-128) * 100/128 (if B==0xFF, sensor is not used in trim calc)
      engine_int = messageIn.data[3] * 0.005;
      sprintf(buffer, "%d V", engine_int);

    case PID_TP:				// Throttle Position
      engine_int = (messageIn.data[3] * 100) / 255;
      sprintf(buffer, "%d %% ", engine_int);
      break;
  }
}

char* WWH_OBD::ecuQuery(unsigned char sid, unsigned char pid)
{
  tCAN message;
  int timeout = 0;
  char message_ok = 0;

  // Prepare message
  message.id = PID_REQUEST;
  message.header.rtr = 0;
  message.header.length = 8;
  message.data[0] = 0x02; // Number of additional data bytes
  message.data[1] = sid; // SID
  message.data[2] = pid; // PID
  message.data[3] = 0x55;
  message.data[4] = 0x55;
  message.data[5] = 0x55;
  message.data[6] = 0x55;
  message.data[7] = 0x55;

  mcp2515_bit_modify(CANCTRL, (1 << REQOP2) | (1 << REQOP1) | (1 << REQOP0), 0);
  //		SET(LED2_HIGH);
  if (mcp2515_send_message(&message)) {
  }

}



