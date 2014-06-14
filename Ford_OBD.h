/*
 Ford On-Board Diagnostics (Ford OBD)
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
#ifndef ford_obd_h_
#define ford_obd_h_

#include <Arduino.h>
//#include "WWH_OBD.h"

#define FORD_PID_BPA 0xA211  // BRAKE PRESSURE APPLIED SWITCH
#define FORD_PID_GEAR 0x11B3 // TRANSIMISSION GEAR STATUS NUMBER
#define FORD_PID_TR 0x11B6   // TRANSMISSION SELECTOR POSITION INPUT STATUS
#define FORD_PID_TRD 0x16B5  // TRANSMISSION SELECTOR POSITION INPUT STATUS (DIGITAL)

// ISO 14229 DID
#define FORD_GMRDB_DID_1E00  0x1E00 // Calculated Engine Torque
#define FORD_GMRDB_DID_1E01  0x1E01 // Auto Shift Manual Status
#define FORD_GMRDB_DID_1E02  0x1E02 // Transmission Requested Engine Idle Speed
#define FORD_GMRDB_DID_1E03  0x1E03 // Shifter Status
#define FORD_GMRDB_DID_1E04  0x1E04 // Transmission Status
// 0x1E05 - 0x1E08 undefined
#define FORD_GMRDB_DID_1E09  0x1E09 // Shift Solenoid  E
#define FORD_GMRDB_DID_1E0A  0x1E0A // Pressure Control Solenoid A
#define FORD_GMRDB_DID_1E0B  0x1E0B // Torque Converter Clutch (TCC) Solenoid Duty Cycle
#define FORD_GMRDB_DID_1E0C  0x1E0C // Shift Solenoid A (SSA) Current Fault Status
#define FORD_GMRDB_DID_1E0D  0x1E0D // Shift Solenoid B (SSB) Current Fault Status
#define FORD_GMRDB_DID_1E0E  0x1E0E // Shift Solenoid C (SSC) Current Fault Status
#define FORD_GMRDB_DID_1E0F  0x1E0F // Shift Solenoid D (SSD) Current Fault Status
#define FORD_GMRDB_DID_1E10  0x1E10 // Shift Solenoid E (SSE) Current Fault Status
#define FORD_GMRDB_DID_1E11  0x1E11 // Pressure Control Solenoid A Commanded Current
#define FORD_GMRDB_DID_1E12  0x1E12 // Gear Commanded
#define FORD_GMRDB_DID_1E13  0x1E13 // Raw/Actual Turbine Shaft Speed (TSS)
#define FORD_GMRDB_DID_1E14  0x1E14 // Measured Slip Across The Torque Convertor
#define FORD_GMRDB_DID_1E15  0x1E15 // Speed Ratio Across Torque Converter
#define FORD_GMRDB_DID_1E16  0x1E16 // Measured Transmission Gear Ratio
#define FORD_GMRDB_DID_1E17  0x1E17 // Pressure Control Solenoid A Fault Status
#define FORD_GMRDB_DID_1E18  0x1E18 // Transmission Range Sensor (TRS) Bit Pattern
#define FORD_GMRDB_DID_1E19  0x1E19 // Commanded Transmission Gear Ratio
#define FORD_GMRDB_DID_1E1A  0x1E1A // Commanded Transmission Main Line Pressure
#define FORD_GMRDB_DID_1E1B  0x1E1B // Actual Output Shaft Speed (OSS)
#define FORD_GMRDB_DID_1E1C  0x1E1C // Raw Transmission Fluid Temperature (TFT)
#define FORD_GMRDB_DID_1E1D  0x1E1D // Transmission Fluid Temperature (TFT) Sensor Voltage
// 0x1E1E undefined
#define FORD_GMRDB_DID_1E1F  0x1E1F // Transmission Gear Engaged
// 0x1E20 undefined
#define FORD_GMRDB_DID_1E21  0x1E21 // Torque Converter Lockup - Calculated By Strategy
#define FORD_GMRDB_DID_1E22  0x1E22 // Transmission Range (TR) Sensor Position -Actual
#define FORD_GMRDB_DID_1E23  0x1E23 // Transmission Range (TR) Sensor Position - Corrected
#define FORD_GMRDB_DID_1E24  0x1E24 // Torque Converter Lockup State - Measured
#define FORD_GMRDB_DID_1E25  0x1E25 // Torque Converter Lockup State

class Ford_OBD
{
  public:
    Ford_OBD();
    uint16_t decodePID(byte* data, char* text);
    byte* encodeQuery(uint16_t pid);

  private:
};

#endif // _ford_obd_h_

