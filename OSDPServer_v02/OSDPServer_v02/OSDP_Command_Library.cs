using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSDPServer_v02
{
    class OSDP_Command_Library
    {
        public static Dictionary<string, int> Commands = new Dictionary<string, int>
        {
            { "Poll", 0x60},
            { "ID Report", 0x61},
            { "PD Capablities Report", 0x62},
            { "Diagnostic Function Report", 0x63},
            { "Local Status Report Reques", 0x64},
            { "Input Status Report Request", 0x65},
            { "Output Status Report Request", 0x66},
            { "Reader Status Report Request", 0x67},
            { "Output Control Command", 0x68},
            { "Reader LED Control Command", 0x69},
            { "Reader Buzzer Control Command", 0x6A},
            { "Text Output Command", 0x6B},
            { "Time and Date Command", 0x6D},
            { "PD Communication Configuration Command", 0x6E},
            { "Data Transfer Command", 0x6F},
            { "Set Automatic Reader Prompt Strings", 0x71},
            { "Scan and Send Biometric Data", 0x73},
            { "Scan and Match Biometric Template", 0x74},
            { "Encryption Key Set Command", 0x75},
            { "Challenge and Secure Session Initialization Rq.", 0x76},
            { "Server Cryptogramm", 0x77},
            { "Stop Multi Part Message", 0x7A},
            { "Maximum Acceptable Reply Size", 0x7B},
            { "Manufacturer Specific Command 1", 0x80},
            { "Manufacturer Specific Command 2", 0x81},
            { "Manufacturer Specific Command 3", 0x82},
            { "Manufacturer Specific Command 4", 0x83},
            { "Manufacturer Specific Command 5", 0x84},
            { "Manufacturer Specific Command 6", 0x85},
            { "Manufacturer Specific Command 7", 0x86}
        };

        public static Dictionary<int, string> Replies = new Dictionary<int, string>
        {
            {0x53, "Start Of Sequence"},
            {0x40, "Command Accepted, Nothing Else To Report"},
            {0x41, "Command Not Processed"},
            {0x45, "PD ID Report"},
            {0x46, "PD Capabilities Report"},
            {0x48, "Local Status Report"},
            {0x49, "Input Status Report"},
            {0x4A, "Output Status Report"},
            {0x4B, "Reader Status Report"},
            {0x50, "Reader Data - Ras Bit Image Of Card Data"},
            {0x51, "Reader Data - Formatted Character Stream"},
            {0x53, "Keypad Data"},
            {0x54, "PD Communications Configuration Report"},
            {0x57, "Biometric Data"},
            {0x58, "Biometric Match Result"},
            {0x76, "Clients ID, Random Number and Cryptogram"},
            {0x78, "Initial R-MAC"},
            {0x90, "Manufacturer Specific Reply"},
            {0x79, "PD is Busy Reply"},
            {0xB1, "See Application Specific Messages"}

        };
    }
}
