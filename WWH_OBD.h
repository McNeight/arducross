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
 
/*
The international standards governing On Board Diagnostics (OBD) is based on
the Open Systems Interconnection (OSI) Basic Reference Model in accordance
with ISO 7498-1 and ISO/IEC 10731, which structures communication systems
into seven layers. When mapped on this model, the services used by a
diagnostic tester (client) and an Electronic Control Unit (ECU, server) are
broken into the following layers in accordance with Table 1:

- Application layer (layer 7), unified diagnostic services specified in:
  - ISO 14229-1 Unified diagnostic services (UDS)
  - ISO 14229-3 UDSonCAN
  - ISO 14229-4 UDSonFR
  - ISO 14229-5 UDSonIP
  - ISO 14229-6 UDSonK-Line
  - ISO 14229-7 UDSonLIN
  - ISO 27145-3 WWH-OBD
  - and further standards
- Presentation layer (layer 6), vehicle manufacturer specific:
  - ISO 27145-2 WWH-OBD
- Session layer services (layer 5) specified in:
  - ISO 14229-2
- Transport layer services (layer 4), specified in:
  - ISO 15765-2 DoCAN
  - ISO 10681-2 Communication on FlexRay
  - ISO 13400-2 DoIP
  - ISO 17987-2 LIN
  - ISO 27145-4 WWH-OBD
- Network layer services (layer 3), specified in:
  - ISO 15765-2 DoCAN
  - ISO 10681-2 Communication on FlexRay
  - ISO 13400-2 DoIP
  - ISO 17987-2 LIN
  - ISO 27145-4 WWH-OBD
- Data link layer (layer 2), specified in:
  - ISO 11898-1 Controller area network (CAN)
  - ISO 11898-2 High Speed CAN (HSCAN)
  - ISO 17458-2
  - ISO 13400-3
  - IEEE 802.3
  - ISO 14230-2
  - ISO 17987-3 LIN
  - ISO 27145-4 WWH-OBD
  - and further standards
- Physical layer (layer 1), specified in:
  - ISO 11898-1
  - ISO 11898-2
  - ISO 17458-4
  - ISO 13400-3
  - IEEE 802.3
  - ISO 14230-1
  - ISO 17987-4 LIN
  - ISO 27145-4 WWH-OBD
  - and further standards

NOTE The diagnostic services in this standard are implemented in various
applications e.g. Road vehicles - Tachograph systems, Road vehicles -
Interchange of digital information on electrical connections between towing
and towed vehicles, Road vehicles - Diagnostic systems, etc. It is required
that future modifications to this standard provide long-term backward
compatibility with the implementation standards as described above.

Table 1 - Example of diagnostic/programming specifications applicable to
          the OSI layers

Applicability| OSI seven  |      Enhanced diagnostics services       | WWH-OBD
             |   layer    |                                          |
-------------------------------------------------------------------------------
             |Application | ISO 14229-1, ISO 14229-3 UDSonCAN,       |
             | (layer 7)  | ISO 14229-4 UDSonFR, ISO 14229-5 UDSonIP,|   ISO
             |            | ISO 14229-6 UDSonK-Line,                 | 27145-3
             |            | ISO 14229-7 UDSonLIN, further standards  |
             |-----------------------------------------------------------------
             |Presentation|                                          |   ISO
             | (layer 6)  |      vehicle manufacturer specific       | 27145-2
             |-----------------------------------------------------------------
             |  Session   |
             | (layer 5)  |               ISO 14229-2
             |-----------------------------------------------------------------
             | Transport  |       |       |       |       |       |
             | (layer 4)  |  ISO  |  ISO  |  ISO  |  Not  |  ISO  |
             |------------|15765-2|10681-2|13400-2| Appl. |17987-2|
             |  Network   |       |       |       |       |       |
             | (layer 3)  |       |       |       |       |       |    ISO
             |-----------------------------------------------------  27145-4
             | Data link  |       |  ISO  |       |  ISO  |  ISO  |
             | (layer 2)  |  ISO  |17458-2|  ISO  |14230-2|17987-3|
             |------------|11898-1|-------|13400-3|----------------
             |  Physical  |  ISO  |  ISO  | IEEE  |  ISO  |  ISO  |
             | (layer 1)  |11898-2|17458-4| 802.3 |14230-1|17987-4|
-------------------------------------------------------------------------------


Seven layer
according to
ISO/IEC7498-1
and
ISO/IEC 10731


*/

#ifndef wwh_obd_h_
#define wwh_obd_h_

#include <Arduino.h>

// ISO 15031-5:2011
// Section 6.2.2.7, Table 7
// This is a system-wide parameter related to diagnostic response times.
// Each server (ECU) is required to respond to a request between P2CAN_min
// and P2CAN_max.
#define P2CAN_MIN 0  // milliseconds
#define P2CAN_MAX 50 // milliseconds 

#define OBD_TIMEOUT_SHORT 2000 /* ms */
#define OBD_TIMEOUT_LONG  7000 /* ms */
#define OBD_TIMEOUT_INIT  3000 /* ms */
#define OBD_RECV_BUF_SIZE 48 /* bytes */

// ISO 14229-1:2013
// Annex A, Table A.1
// Negative Response Code (NRC) definition and values
#define NRC_PR      0x00 // positiveResponse
// 0x01 - 0x0F ISOSAEReserved
#define NRC_GR      0x10 // generalReject
#define NRC_SNS     0x11 // serviceNotSupported
#define NRC_SFNS    0x12 // subFunctionNotSupported
#define NRC_IMLOIF  0x13 // incorrectMessageLengthOrInvalidFormat
#define NRC_RTL     0x14 // responseTooLong
// 0x15 - 0x20 ISOSAEReserved
#define NRC_BRR     0x21 // busy-RepeatRequest
#define NRC_CNC     0x22 // conditionsNotCorrect
// 0x23 ISOSAEReserved
#define NRC_RSE     0x24 // requestSequenceError
#define NRC_NRFSC   0x25 // noResponseFromSubnetComponent
#define NRC_FPEORA  0x26 // FailurePreventsExecutionOfRequestedAction
// 0x27 - 0x30 ISOSAEReserved
#define NRC_ROOR    0x31 // requestOutOfRange
// 0x32 ISOSAEReserved
#define NRC_SAD     0x33 // securityAccessDenied
// 0x34 ISOSAEReserved
#define NRC_IK      0x35 // invalidKey
#define NRC_ENOA    0x36 // exceedNumberOfAttempts
#define NRC_RTDNE   0x37 // requiredTimeDelayNotExpired
// 0x38 - 0x4F reservedByExtendedDataLinkSecurityDocument
// 0x50 - 0x6F ISOSAEReserved
#define NRC_UDNA    0x70 // uploadDownloadNotAccepted
#define NRC_TDS     0x71 // transferDataSuspended
#define NRC_GPF     0x72 // generalProgrammingFailure
#define NRC_WBSC    0x73 // wrongBlockSequenceCounter
// 0x74 - 0x77 ISOSAEReserved
#define NRC_RCRRP   0x78 // requestCorrectlyReceived-ResponsePending
// 0x79 - 0x7D ISOSAEReserved
#define NRC_SFNSIAS 0x7E // sub-functionNotSupportedInActiveSession
#define NRC_SNSIAS  0x7F // serviceNotSupportedInActiveSession
// 0x80 ISOSAEReserved

// 0x8E ISOSAEReserved

// 0x94 - 0xEF reservedForSpecificConditionsNotCorrect
// 0xF0 - 0xFE vehicleManufacturerSpecificConditionsNotCorrect
// 0xFF ISOSAEReserved

// ISO 15031-5:2011
// Section 6.3.4, Table 16

typedef struct
{
  const char display[9];
  uint8_t pid;
  uint8_t start;    // starting with which data byte
  uint8_t size;     // taking up how many bytes
  uint8_t bitmask;  // if less than a byte, which bits are used
} WWH_OBD_PID;

//WWH_OBD_PID pid_dictionary[] = {
//  {"DTC_CNT" , 0x01, 0, 1, 0x7F},
//  {"MIL"     , 0x01, 0, 1, 0x80},
//  {"MIS_SUP" , 0x01, 1, 1, 0x01},
//  {"FUEL_SUP", 0x01, 1, 1, 0x02},
//  {"CCM_SUP" , 0x01, 1, 1, 0x04},
//  {"N/A"     , 0x01, 1, 1, 0x08},
//  {"MIS_RDY" , 0x01, 1, 1, 0x10},
//  {"FUEL_RDY", 0x01, 1, 1, 0x20},
//  {"CCM_RDY" , 0x01, 1, 1, 0x40},
//  /* Bit 7 is unused */
//  {"CAT_SUP" , 0x01, 2, 1, 0x01},
//  {"HCAT_SUP", 0x01, 2, 1, 0x02},
//
//  {"DTCFRZF" , 0x02, 0, 1, 0xFF},
//  {"FUELSYS" , 0x03, 0, 1, 0xFF}
//};

// ISO 14229-1:2013
// Annex C, Table C.1
// DID data-parameter definitions
// 0x0000 - 0x00FF ISOSAEReserved
// 0x0100 - 0xA5FF VehicleManufacturerSpecific
// 0xA600 - 0xA7FF ReservedForLegislativeUse
// 0xA800 - 0xACFF VehicleManufacturerSpecific
// 0xAD00 - 0xAFFF ReservedForLegislativeUse
// 0xB000 - 0xB1FF VehicleManufacturerSpecific
// 0xB200 - 0xBFFF ReservedForLegislativeUse
// 0xC000 - 0xC2FF VehicleManufacturerSpecific
// 0xC300 - 0xCEFF ReservedForLegislativeUse
// 0xCF00 - 0xEFFF VehicleManufacturerSpecific
// 0xF000 - 0xF00F networkConfigurationDataForTractorTrailerApplicationData-
//                   Identifier
// 0xF010 - 0xF0FF vehicleManufacturerSpecific
// 0xF100 - 0xF17F identificationOptionVehicleManufacturerSpecificDataIdentifier
#define DID_BSIDID       0xF180 // BootSoftwareIdentificationDataIdentifier
#define DID_ASIDID       0xF181 // applicationSoftwareIdentificationDataIdentifier
#define DID_ADIDID       0xF182 // applicationDataIdentificationDataIdentifier
#define DID_BSFPDID      0xF183 // bootSoftwareFingerprintDataIdentifier
#define DID_ASFPDID      0xF184 // applicationSoftwareFingerprintDataIdentifier
#define DID_ADFPDID      0xF185 // applicationDataFingerprintDataIdentifier
#define DID_ADSDID       0xF186 // ActiveDiagnosticSessionDataIdentifier
#define DID_VMSPNDID     0xF187 // vehicleManufacturerSparePartNumberDataIdentifier
#define DID_VMECUSNDID   0xF188 // vehicleManufacturerECUSoftwareNumberDataIdentifier
#define DID_VMECUSVNDID  0xF189 // vehicleManufacturerECUSoftwareVersionNumberDataIdentifier
#define DID_SSIDDID      0xF18A // systemSupplierIdentifierDataIdentifier
#define DID_ECUMDDID     0xF18B // ECUManufacturingDateDataIdentifier
#define DID_ECUSNDID     0xF18C // ECUSerialNumberDataIdentifier
#define DID_SFUDID       0xF18D // supportedFunctionalUnitsDataIdentifier
#define DID_VMKAPNDID    0xF18E // VehicleManufacturerKitAssemblyPartNumberDataIdentifier
// 0xF18F ISOSAEReservedStandardized

// 0xF1A0 - 0xF1EF identificationOptionVehicleManufacturerSpecific
// 0xF1F0 - 0xF1FF identificationOptionSystemSupplierSpecific
// 0xF200 - 0xF2FF periodicDataIdentifier
// 0xF300 - 0xF3FF DynamicallyDefinedDataIdentifier
// 0xF400 - 0xF4FF OBDDataIdentifier
// 0xF500 - 0xF5FF OBDDataIdentifier
// 0xF600 - 0xF6FF OBDMonitorDataIdentifier
// 0xF700 - 0xF7FF OBDMonitorDataIdentifier
// 0xF800 - 0xF8FF OBDInfoTypeDataIdentifier
// 0xF900 - 0xF9FF TachographDataIdentifier
// 0xFA00 - 0xFA0F AirbagDeploymentDataIdentifier

// 0xFA13 - 0xFA18 EDREntries
// 0xFA19 - 0xFAFF SafetySystemDataIdentifier
// 0xFB00 - 0xFCFF ReservedForLegislativeUse
// 0xFD00 - 0xFEFF SystemSupplierSpecific
#define DID_UDSVDID      0xFF00 // UDSVersionDataIdentifier
// 0xFF01 - 0xFFFF ISOSAEReserved

// ISO 14229-1:2013
// Annex C, Table C.2
// scalingByte (High Nibble) parameter definitions
#define SBYT_USN    0x0 // unSignedNumeric (1 to 4 bytes)
#define SBYT_SN     0x1 // signedNumeric (1 to 4 bytes)
#define SBYT_BMRWOM 0x2 // bitMappedReportedWithOutMask
#define SBYT_BMRWM  0x3 // bitMappedReportedWithMask
#define SBYT_BCD    0x4 // BinaryCodedDecimal
#define SBYT_SEV    0x5 // stateEncodedVariable (1 byte)
#define SBYT_ASCII  0x6 // ASCII (1 to 15 bytes for each scalingByte)
#define SBYT_SFP    0x7 // signedFloatingPoint
#define SBYT_P      0x8 // packet
#define SBYT_F      0x9 // formula
#define SBYT_U      0xA // unit/format
#define SBYT_SACT   0xB // stateAndConnectionType (1 byte)
// 0xC - 0xF ISOSAEReserved

// ISO 14229-1:2013
// Annex C, Table C.3
// scalingByte (Low Nibble) parameter definition
// 0x0 - 0xF numberOfBytesOfParameter

// SAE J1979-DA Revised OCT2011
// APPENDIX B - (NORMATIVE)
// PIDS (PARAMETER ID) FOR SERVICES $01 AND $02 SCALING AND DEFINITION
#define PID_DTCFRZF 0x02 // DTC that caused required freeze frame data storage
#define PID_FUELSYS 0x03 // Fuel system status
#define PID_LOAD_PCT 0x04 // Calculated LOAD Value
#define PID_ECT 0x05 // Engine Coolant Temperature
#define PID_SHRTFT13 0x06 // Short Term Fuel Trim - Banks 1 & 3
#define PID_LONGFT13 0x07 // Long Term Fuel Trim - Banks 1 & 3
#define PID_SHRTFT24 0x08 // Short Term Fuel Trim - Banks 2 & 4
#define PID_LONGFT24 0x09 // Long Term Fuel Trim - Banks 2 & 4
#define PID_FP 0x0A // Fuel Pressure (gauge)
#define PID_MAP 0x0B // Intake Manifold Absolute Pressure
#define PID_RPM 0x0C // Engine RPM
#define PID_SPEED 0x0D // Vehicle Speed Sensor
#define PID_SPARKADV 0x0E // Ignition Timing Advance for #1 Cylinder
#define PID_IAT 0x0F // Intake Air Temperature
#define PID_MAF 0x10 // Air Flow Rate from Mass Air Flow Sensor
#define PID_TP 0x11 // Absolute Throttle Position
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

#define PID_RUNTM 0x1F
#define PID_FLI 0x2F
#define PID_CLR_DIST 0x31
#define PID_BARO 0x33

#define PID_LOAD_ABS 0x43 // Absolute Load Value
#define PID_LAMBDA 0x44 // Fuel/Air Commanded Equivalence Ratio
#define PID_TP_R 0x45 // Relative Throttle Position
#define PID_AAT 0x46 // Ambient air temperature
#define PID_TP_B 0x47 // Absolute Throttle Position B
#define PID_TP_C 0x48 // Absolute Throttle Position C
#define PID_APP_D 0x49 // Accelerator Pedal Position D
#define PID_APP_E 0x4A // Accelerator Pedal Position E
#define PID_APP_F 0x4B // Accelerator Pedal Position F
#define PID_TAC_PCT 0x4C // Commanded Throttle Actuator Control

#define PID_FUEL_TYP 0x51 // Type of fuel currently being utilized by the vehicle
#define PID_ALCH_PCT 0x52 // Alcohol Fuel Percentage

#define PID_APP_R 0x5A // Relative Accelerator Pedal Position
#define PID_BAT_PWR 0x5B // Hybrid/EV Battery Pack Remaining Charge

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

// SAE J1979, Appendix E
// Unit and Scaling Definition for Service $06

class WWH_OBD
{
  public:
    WWH_OBD();
    byte decodePID(byte* data, char* text);
    byte* encodeQuery(unsigned char sid, unsigned char pid);

  private:
};

#endif // _wwh_obd_h_

