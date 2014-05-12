/*
 World Wide Harmonized On-Board Diagnostics (WWH OBD)
 by:
 date:
 license:

 Contains defines and algorythms spelled out in various standards documents including:
 SAE J1939      Recommended Practice for Serial Control and Communications Vehicle Network
 SAE J1939-03   On Board Diagnostics Implementation Guide
 SAE J1978      OBD II Scan Tool — Equivalent to ISO/DIS 15031-4:December 14, 2001
 SAE J1979      E/E Diagnostic Test Modes
 ISO 15031-5    Road vehicles—Communication between vehicle and external equipment for
 emissions-related diagnostics—Part 5: Emissions-related diagnostic services
 ISO 15765-4    Road vehicles—Diagnostics on controller area network (CAN)—Part 4:
 Requirements for emissions-related systems
 ISO 27145-4    Road vehicles—Implementation of WWH-OBD communication requirements—Part 4:
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
#ifndef wwh_obd__h
#define wwh_obd__h

#include <Canbus.h>
#include <mcp2515.h>

#define OBD_TIMEOUT_SHORT 2000 /* ms */
#define OBD_TIMEOUT_LONG 7000 /* ms */
#define OBD_TIMEOUT_INIT 3000 /* ms */
#define OBD_RECV_BUF_SIZE 48 /* bytes */

// SAE J1979-DA Revised OCT2011
// APPENDIX B - (NORMATIVE)
// PIDS (PARAMETER ID) FOR SERVICES $01 AND $02 SCALING AND DEFINITION
#define PID_DTCFRZF 0x02
#define PID_FUELSYS 0x03
#define PID_LOAD_PCT 0x04
#define PID_ECT 0x05
#define PID_SHRTFT13 0x06
#define PID_LONGFT13 0x07
#define PID_SHRTFT24 0x08
#define PID_LONGFT24 0x09
#define PID_FP 0x0A
#define PID_MAP 0x0B
#define PID_RPM 0x0C
#define PID_SPEED 0x0D
#define PID_SPARKADV 0x0E
#define PID_IAT 0x0F
#define PID_MAF 0x10
#define PID_TP 0x11
// Location of Oxygen Sensors, where sensor 1 is closest to the engine. Each bit indicates the
// presence or absence of an oxygen sensor at the following location.
#define PID_O2SLOC2 0x13
// NOTE: PID $13 shall only be supported by a given vehicle if PID $1D is not supported. In no
// case shall a vehicle support both PIDs. PID $13 is recommended for 1 or 2 bank O2 sensor
// engine configurations, and never for 3 or 4 bank O2 sensor engine configurations.
// See Figure B3 for an explanation of how this PID will be used to determine how many data bytes
// will be reported when short term or long term fuel trim values are reported with
// PIDs $06 to $09 and PIDs $55 to $58.

#define PID_O2S11 0x14
#define PID_O2S12 0x15
#define PID_O2S13 0x16
#define PID_O2S14 0x17
#define PID_O2S21 0x18
#define PID_O2S22 0x19
#define PID_O2S23 0x1A
#define PID_O2S24 0x1B

#define PID_O2SLOC4 0x1D

#define PID_ABS_ENGINE_LOAD 0x43
#define PID_AMBIENT_TEMP 0x46

#define PID_BAROMETRIC 0x33
#define PID_FUEL_LEVEL 0x2F
#define PID_RUNTIME 0x1F
#define PID_DISTANCE 0x31

// ISO 15765-4:2011
// Table 6
// 11 bit legislated OBD/WWH-OBD CAN identifiers
#define ID_REQUEST 0x7DF // CAN identifier for functionally addressed request messages sent by external test equipment
#define ID_REQUEST_1 0x7E0 // Physical request CAN identifier from external test equipment to ECU #1
#define ID_REPLY_1 0x7E8 // Physical response CAN identifier from ECU #1 to external test equipment
#define ID_REQUEST_2 0x7E1 // Physical request CAN identifier from external test equipment to ECU #2
#define ID_REPLY_2 0x7E9 // Physical response CAN identifier from ECU #2 to external test equipment
#define ID_REQUEST_3 0x7E2 // Physical request CAN identifier from external test equipment to ECU #3
#define ID_REPLY_3 0x7EA // Physical response CAN identifier from ECU #3 to external test equipment
#define ID_REQUEST_4 0x7E3 // Physical request CAN identifier from external test equipment to ECU #4
#define ID_REPLY_4 0x7EB // Physical response CAN identifier from ECU #4 to external test equipment
#define ID_REQUEST_5 0x7E4 // Physical request CAN identifier from external test equipment to ECU #5
#define ID_REPLY_5 0x7EC // Physical response CAN identifier from ECU #5 to external test equipment
#define ID_REQUEST_6 0x7E5 // Physical request CAN identifier from external test equipment to ECU #6
#define ID_REPLY_6 0x7ED // Physical response CAN identifier from ECU #6 to external test equipment
#define ID_REQUEST_7 0x7E6 // Physical request CAN identifier from external test equipment to ECU #7
#define ID_REPLY_7 0x7EE // Physical response CAN identifier from ECU #7 to external test equipment
#define ID_REQUEST_8 0x7E7 // Physical request CAN identifier from external test equipment to ECU #8
#define ID_REPLY_8 0x7EF // Physical response CAN identifier from ECU #8 to external test equipment

// SAE J1979 Service ID
// SIDRQ - Request Service Identifier
// SIDPR - Positive Response Service Identifier
// SIDNR - Negative Response Service Identifier
// Service $01 - Request Current Powertrain Diagnostic Data
#define SIDRQ_DIAG 0x01 // Request current powertrain diagnostic data request SID
#define SIDPR_DIAG 0x41 // Request current powertrain diagnostic data response SID
// Service $02 - Request Powertrain Freeze Frame Data
#define SIDRQ_FF 0x02 // Request powertrain freeze frame data request SID
#define SIDPR_FF 0x42 // Request powertrain freeze frame data response SID
// Service $03 - Request Emission-Related Diagnostic Trouble Codes
#define SIDRQ_DTC 0x03 // Request emission-related DTC request SID
#define SIDPR_DTC 0x43 // Request emission-related DTC response SID
// Service $04 - Clear/Reset Emission-Related Diagnostic Information
#define SIDRQ_CR 0x04 // Clear/reset emission-related diagnostic information request SID
#define SIDPR_CR 0x44 // Clear/reset emission-related diagnostic information response SID
// Service $05 - Request Oxygen Sensor Monitoring Test Results
//  Service $05 is not supported for ISO 15765-4. The functionality of Service $05 is implemented in Service $06.
// Service $06 - Request On-Board Monitoring Test Results for Specific Monitored Systems
#define SIDRQ_OBD 0x06 // Request on-board monitoring test results for specific monitored systems request SID
#define SIDPR_OBD 0x46 // Request on-board monitoring test results for specific monitored systems response SID
// Service $07 - Request Emission-Related Diagnostic Trouble Codes Detected During Current or Last Completed Driving Cycle
#define SIDRQ_LCDC 0x07 // Request emission-related diagnostic trouble codes detected during current or last completed driving cycle request SID
#define SIDPR_LCDC 0x47 // Request emission-related diagnostic trouble codes detected during current or last completed driving cycle response SID
// Service $08 - Request Control of On-Board System, Test or Component
#define SIDRQ_RC 0x08 // Request control of on-board device request SID
#define SIDPR_RC 0x48 // Request control of on-board device response message SID
// Service $09 - Request Vehicle Information
#define SIDRQ_INFO 0x09 // Request vehicle information request SID
#define SIDPR_INFO 0x49 // Request vehicle information response SID
// Service $0A - Request Emission-Related Diagnostic Trouble Codes with Permanent Status
#define SIDRQ_PERM 0x0A // Request emission-related diagnostic trouble codes with permanent status request SID
#define SIDPR_PERM 0x4A // Request emission-related diagnostic trouble codes with permanent status response SID

#define SIDNR 0x7F // Negative Response Service Identifier

class WWH_OBD
{
  public:
    WWH_OBD();
    char* decodePID(tCAN messageIn);
    char* ecuQuery(unsigned char sid, unsigned char pid);

  private:
};

#endif



