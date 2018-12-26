using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;
using System.IO;
using Microsoft.VisualBasic;
using Microsoft.Win32;
using Microsoft.Win32.SafeHandles;

namespace Elatec
{
    public class Guids
    {
        /************************************************************/
        /****** Well known Guids ************************************/
        /************************************************************/

        public static Guid Null = new Guid("{00000000-0000-0000-0000-000000000000}");

        public static Guid OSR_DEVICE_INTERFACE = new Guid("{f18a0e88-c30c-11d0-8815-00a0c906bed8}");
        public static Guid DeviceSetupClassUSBDevice = new Guid("{88BAE032-5A81-49f0-BC3D-A4FF138216D6}");
        public static Guid DeviceSetupClassHIDDevice = new Guid("{745a17a0-74d3-11d0-b6fe-00a0c90f57da}");
        public static Guid DeviceSetupClassUnknownCCID = new Guid(0x12d0b03a, 0x1e17, 0x42d9, 0xab, 0x77, 0xed, 0xe8, 0xa3, 0x52, 0x1c, 0xc7); // CCID? No.

        public static Guid PropKeyHardwareID = new Guid(0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0);
        public static Guid PropKeyBusReportedDeviceDesc = new Guid(0x540b947e, 0x8b40, 0x45bc, 0xa8, 0xa2, 0x6a, 0x0b, 0x89, 0x4c, 0xbd, 0xa2);
        public static Guid DevInterfaceUSBDevice = new Guid("{a5dcbf10-6530-11d2-901f-00c04fb951ed}");
        public static Guid DevInterfaceUSBHub = new Guid("{F18A0E88-C30C-11D0-8815-00A0C906BED8}");
    };
    public class Win32
    {
        //******************************************************************
        //****** General Definition                                   ******
        //******************************************************************

        public const string WinDLL = "kernel32.dll";

        internal const UInt32 MAXDWORD = 0xffffffff;

        //******************************************************************
        //****** General Functions                                    ******
        //******************************************************************

        [DllImport(WinDLL)]
        internal static extern void Sleep(UInt32 Time);

        //******************************************************************
        //****** Error Functions                                      ******
        //******************************************************************

        internal const UInt32 ERROR_SUCCESS = 0;
        internal const UInt32 ERROR_FILE_NOT_FOUND = 2;
        internal const UInt32 ERROR_ACCESS_DENIED = 5;
        internal const UInt32 ERROR_INVALID_HANDLE = 6;
        internal const UInt32 ERROR_GEN_FAILURE = 31;
        internal const UInt32 ERROR_INVALID_PARAMETER = 87;
        internal const UInt32 ERROR_INSUFFICIENT_BUFFER = 122;
        internal const UInt32 ERROR_INVALID_NAME = 123;
        internal const UInt32 ERROR_NO_MORE_ITEMS = 259;
        internal const UInt32 ERROR_IO_INCOMPLETE = 996;
        internal const UInt32 ERROR_IO_PENDING = 997;
        internal const UInt32 ERROR_INVALID_HANDLE_STATE = 1609;

        [DllImport(WinDLL)]
        internal static extern UInt32 GetLastError();
    }
    public sealed class DeviceManagementApiDeclarations
    {

        // ******************************************************************************
        // API constants
        // ******************************************************************************

        // from dbt.h
        public const int DBT_DEVICEARRIVAL = 0x8000;
        public const int DBT_DEVICEREMOVECOMPLETE = 0x8004;
        public const int DBT_DEVTYP_DEVICEINTERFACE = 5;
        public const int DBT_DEVTYP_HANDLE = 6;
        public const int DEVICE_NOTIFY_ALL_INTERFACE_CLASSES = 4;
        public const int DEVICE_NOTIFY_SERVICE_HANDLE = 1;
        public const int DEVICE_NOTIFY_WINDOW_HANDLE = 0;
        public const int WM_DEVICECHANGE = 0x219;

        // from setupapi.h
        public const short DIGCF_DEFAULT = 0x00000001;  // only valid with DIGCF_DEVICEINTERFACE
        public const short DIGCF_PRESENT = 0x00000002;
        public const short DIGCF_ALLCLASSES = 0x00000004;
        public const short DIGCF_PROFILE = 0x00000008;
        public const short DIGCF_DEVICEINTERFACE = 0x00000010;

        public const int SPINT_ACTIVE = 0x00000001;
        public const int SPINT_DEFAULT = 0x00000002;
        public const int SPINT_REMOVED = 0x00000004;

        // ******************************************************************************
        // Structures and classes for API calls, listed alphabetically
        // ******************************************************************************

        // There are two declarations for the DEV_BROADCAST_DEVICEINTERFACE structure.

        // Use this in the call to RegisterDeviceNotification() and
        // in checking dbch_devicetype in a DEV_BROADCAST_HDR structure.
        [StructLayout(LayoutKind.Sequential)]
        public class DEV_BROADCAST_DEVICEINTERFACE
        {
            public int dbcc_size;
            public int dbcc_devicetype;
            public int dbcc_reserved;
            public Guid dbcc_classguid;
            public short dbcc_name;
        }

        // Use this to read the dbcc_name string and classguid.
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public class DEV_BROADCAST_DEVICEINTERFACE_1
        {
            public int dbcc_size;
            public int dbcc_devicetype;
            public int dbcc_reserved;
            [MarshalAs(UnmanagedType.ByValArray, ArraySubType = UnmanagedType.U1, SizeConst = 16)]
            public byte[] dbcc_classguid;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 255)]
            public char[] dbcc_name;
        }

        [StructLayout(LayoutKind.Sequential)]
        public class DEV_BROADCAST_HANDLE
        {
            public int dbch_size;
            public int dbch_devicetype;
            public int dbch_reserved;
            public int dbch_handle;
            public int dbch_hdevnotify;
        }

        [StructLayout(LayoutKind.Sequential)]
        public class DEV_BROADCAST_HDR
        {
            public int dbch_size;
            public int dbch_devicetype;
            public int dbch_reserved;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct SP_DEVICE_INTERFACE_DATA
        {
            public int cbSize;
            public Guid InterfaceClassGuid;
            public int Flags;
            public IntPtr Reserved;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct SP_DEVICE_INTERFACE_DETAIL_DATA
        {
            public int cbSize;
            public string DevicePath;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct SP_DEVINFO_DATA
        {
            public int cbSize;
            public Guid ClassGuid;
            public int DevInst;
            public IntPtr Reserved;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct DEVPROPKEY
        {
            public Guid fmtid;
            public int pid;         // was UInt32
            public DEVPROPKEY(Guid fmtid, int pid)
            {
                this.fmtid = fmtid;
                this.pid = pid;
            }
        }

        // ******************************************************************************
        // API functions, listed alphabetically
        // ******************************************************************************

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        static public extern IntPtr RegisterDeviceNotification(IntPtr hRecipient, IntPtr NotificationFilter, int Flags);

        [DllImport("setupapi.dll")]
        static public extern bool SetupDiCreateDeviceInfoList(ref Guid ClassGuid, IntPtr hwndParent);

        [DllImport("setupapi.dll")]
        static public extern bool SetupDiDestroyDeviceInfoList(IntPtr DeviceInfoSet);

        [DllImport("setupapi.dll")]
        static public extern bool SetupDiEnumDeviceInterfaces(IntPtr DeviceInfoSet, IntPtr DeviceInfoData, ref Guid InterfaceClassGuid, int MemberIndex, ref SP_DEVICE_INTERFACE_DATA DeviceInterfaceData);

        [DllImport("setupapi.dll", CharSet = CharSet.Auto)]
        static public extern IntPtr SetupDiGetClassDevs(ref Guid ClassGuid, string Enumerator, IntPtr hwndParent, int Flags);

        [DllImport("setupapi.dll", CharSet = CharSet.Auto, SetLastError = true)]
        static public extern bool SetupDiGetDeviceInterfaceDetail(IntPtr DeviceInfoSet, ref SP_DEVICE_INTERFACE_DATA DeviceInterfaceData, IntPtr DeviceInterfaceDetailData, int DeviceInterfaceDetailDataSize, ref int RequiredSize, IntPtr DeviceInfoData);

        [DllImport("setupapi.dll", CharSet = CharSet.Auto, SetLastError = true)]
        static public extern bool SetupDiGetDeviceProperty(IntPtr deviceInfoSet, ref SP_DEVINFO_DATA DeviceInfoData, ref DEVPROPKEY propertyKey, out UInt32 propertyType, IntPtr propertyBuffer, int propertyBufferSize, out int requiredSize, UInt32 flags);

        [DllImport("setupapi.dll")]
        static public extern bool SetupDiEnumDeviceInfo(IntPtr DeviceInfoSet, int MemberIndex, ref SP_DEVINFO_DATA DeviceInfoData);

        [DllImport("user32.dll")]
        static public extern bool UnregisterDeviceNotification(IntPtr Handle);
    }
    public sealed class HidApiDeclarations
    {

        // API Declarations for communicating with HID-class devices.

        // ******************************************************************************
        // API constants
        // ******************************************************************************

        // from hidpi.h
        // Typedef enum defines a set of integer constants for HidP_Report_Type
        public const short HidP_Input = 0;
        public const short HidP_Output = 1;
        public const short HidP_Feature = 2;

        public const int HIDP_STATUS_SUCCESS = 0x00110000;

        // ******************************************************************************
        // Structures and classes for API calls, listed alphabetically
        // ******************************************************************************

        [StructLayout(LayoutKind.Sequential)]
        public class HIDD_ATTRIBUTES
        {
            public int Size;
            public ushort VendorID;
            public ushort ProductID;
            public ushort VersionNumber;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct HIDP_CAPS
        {
            public ushort Usage;
            public ushort UsagePage;
            public ushort InputReportByteLength;
            public ushort OutputReportByteLength;
            public ushort FeatureReportByteLength;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 17)]
            public ushort[] Reserved;
            public ushort NumberLinkCollectionNodes;
            public ushort NumberInputButtonCaps;
            public ushort NumberInputValueCaps;
            public ushort NumberInputDataIndices;
            public ushort NumberOutputButtonCaps;
            public ushort NumberOutputValueCaps;
            public ushort NumberOutputDataIndices;
            public ushort NumberFeatureButtonCaps;
            public ushort NumberFeatureValueCaps;
            public ushort NumberFeatureDataIndices;
        }

        // If IsRange is false, UsageMin is the Usage and UsageMax is unused.
        // If IsStringRange is false, StringMin is the string index and StringMax is unused.
        // If IsDesignatorRange is false, DesignatorMin is the designator index and DesignatorMax is unused.

        [StructLayout(LayoutKind.Sequential)]
        public class HidP_Value_Caps
        {
            public ushort UsagePage;
            public byte ReportID;
            public int IsAlias;
            public ushort BitField;
            public ushort LinkCollection;
            public ushort LinkUsage;
            public ushort LinkUsagePage;
            public int IsRange;
            public int IsStringRange;
            public int IsDesignatorRange;
            public int IsAbsolute;
            public int HasNull;
            public byte Reserved;
            public ushort BitSize;
            public ushort ReportCount;
            public ushort Reserved2;
            public ushort Reserved3;
            public ushort Reserved4;
            public ushort Reserved5;
            public ushort Reserved6;
            public int LogicalMin;
            public int LogicalMax;
            public int PhysicalMin;
            public int PhysicalMax;
            public ushort UsageMin;
            public ushort UsageMax;
            public ushort StringMin;
            public ushort StringMax;
            public ushort DesignatorMin;
            public ushort DesignatorMax;
            public ushort DataIndexMin;
            public ushort DataIndexMax;
        }

        // ******************************************************************************
        // API functions, listed alphabetically
        // ******************************************************************************

        [DllImport("hid.dll", SetLastError = true)]
        static public extern unsafe bool HidD_FlushQueue(IntPtr HidDeviceObject);

        [DllImport("hid.dll", SetLastError = true)]
        static public extern unsafe bool HidD_FreePreparsedData(ref IntPtr PreparsedData);

        [DllImport("hid.dll", SetLastError = true)]
        static public extern unsafe int HidD_GetAttributes(IntPtr HidDeviceObject, HIDD_ATTRIBUTES Attributes);

        [DllImport("hid.dll", SetLastError = true)]
        static public extern unsafe bool HidD_GetFeature(IntPtr HidDeviceObject, void* lpReportBuffer, int ReportBufferLength);

        [DllImport("hid.dll", SetLastError = true)]
        static public extern unsafe bool HidD_GetInputReport(IntPtr HidDeviceObject, void* lpReportBuffer, int ReportBufferLength);

        [DllImport("hid.dll", SetLastError = true)]
        static public extern unsafe void HidD_GetHidGuid(ref Guid HidGuid);

        [DllImport("hid.dll", SetLastError = true)]
        static public extern unsafe bool HidD_GetNumInputBuffers(IntPtr HidDeviceObject, ref int NumberBuffers);

        [DllImport("hid.dll", SetLastError = true)]
        static public extern unsafe bool HidD_GetPreparsedData(IntPtr HidDeviceObject, out IntPtr PreparsedData);

        [DllImport("hid.dll", SetLastError = true)]
        static public extern unsafe bool HidD_SetFeature(IntPtr HidDeviceObject, void* lpReportBuffer, int ReportBufferLength);

        [DllImport("hid.dll", SetLastError = true)]
        static public extern unsafe bool HidD_SetNumInputBuffers(IntPtr HidDeviceObject, int NumberBuffers);

        [DllImport("hid.dll", SetLastError = true)]
        static public extern unsafe bool HidD_SetOutputReport(IntPtr HidDeviceObject, void* lpReportBuffer, int ReportBufferLength);

        [DllImport("hid.dll", SetLastError = true)]
        static public extern unsafe int HidP_GetCaps(IntPtr PreparsedData, ref HIDP_CAPS Capabilities);

        [DllImport("hid.dll", SetLastError = true)]
        static public extern unsafe int HidP_GetValueCaps(int ReportType, byte* ValueCaps, ref ushort ValueCapsLength, IntPtr PreparsedData);
    }
    public class FileIOApiDeclarations
    {

        // API declarations relating to file I/O.

        // ******************************************************************************
        // API constants
        // ******************************************************************************

        // public const uint INVALID_HANDLE_VALUE = 0xFFFFFFFF;  Use IntPtr instead
        public const uint FILE_FLAG_OVERLAPPED = 0x40000000;
        public const uint OPEN_EXISTING = 3;
        public const uint GENERIC_READ = 0x80000000;
        public const uint GENERIC_WRITE = 0x40000000;
        public const uint FILE_SHARE_READ = 0x00000001;
        public const uint FILE_SHARE_WRITE = 0x00000002;
        public const uint WAIT_TIMEOUT = 0x102;
        public const uint WAIT_OBJECT_0 = 0;

        // ******************************************************************************
        // Structures and classes for API calls, listed alphabetically
        // ******************************************************************************

        /*
		typedef struct _OVERLAPPED
        {                                   32      64
            ULONG_PTR Internal;             4       8
            ULONG_PTR InternalHigh;         4       8
            union {
            struct
            {
                DWORD Offset;               4       4
                DWORD OffsetHigh;           4       4
            } DUMMYSTRUCTNAME;
            PVOID Pointer;                  4       8
        } DUMMYUNIONNAME;
        HANDLE  hEvent;                     4       8
        } OVERLAPPED, *LPOVERLAPPED;
        */

        [StructLayout(LayoutKind.Sequential)]
        public class OVERLAPPED
        {
            public IntPtr Internal;
            public IntPtr InternalHigh;
            public int Offset;
            public int OffsetHigh;
            public IntPtr hEvent;
        }

        /*
        typedef struct _SECURITY_ATTRIBUTES
        {                                   32      64
            DWORD nLength;                  4       4
            LPVOID lpSecurityDescriptor;    4       8
            BOOL bInheritHandle;            4       4
        } SECURITY_ATTRIBUTES;
        */

        [StructLayout(LayoutKind.Sequential)]
        public class SECURITY_ATTRIBUTES
        {
            public int nLength;
            public IntPtr lpSecurityDescriptor;
            public int bInheritHandle;
        }

        // ******************************************************************************
        // API functions, listed alphabetically
        // ******************************************************************************

        [DllImport("kernel32.dll", SetLastError = true)]
        static public extern unsafe bool CancelIo
        (
            IntPtr hFile
        );

        [DllImport("kernel32.dll", SetLastError = true)]
        static public extern unsafe bool CloseHandle
        (
            IntPtr hObject
        );

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        static public extern unsafe IntPtr CreateEvent
        (
            SECURITY_ATTRIBUTES SecurityAttributes,
            bool bManualReset,
            bool bInitialState,
            string lpName
        );

        [DllImport("kernel32", SetLastError = true)]
        static public extern unsafe IntPtr CreateFile
        (
            string FileName,
            uint DesiredAccess,
            uint ShareMode,
            SECURITY_ATTRIBUTES SecurityAttributes,
            uint CreationDisposition,
            uint FlagsAndAttributes,
            IntPtr hTemplateFile
        );

        [DllImport("kernel32", SetLastError = true)]
        static public extern unsafe bool DeviceIoControl
        (
          [In] IntPtr hDevice,
          [In] uint dwIoControlCode,
          [In] void* lpInBuffer,                // Optional
          [In] uint nInBufferSize,
          [Out] void* lpOutBuffer,              // Optional
          [In] uint nOutBufferSize,
          [Out] uint* lpBytesReturned,          // Optional
          [In, Out] OVERLAPPED lpOverlapped     // Optional
        );

        [DllImport("kernel32", SetLastError = true)]
        static public extern unsafe bool ReadFile
        (
            IntPtr hFile,
            void* lpBuffer,
            int NumberOfBytesToRead,
            int* pNumberOfBytesRead,
            OVERLAPPED lpOverlapped
        );

        [DllImport("kernel32.dll", SetLastError = true)]
        static public extern unsafe uint WaitForSingleObject
        (
            IntPtr hHandle,
            int dwMilliseconds
        );

        [DllImport("kernel32", SetLastError = true)]
        static public extern unsafe bool WriteFile
        (
            IntPtr hFile,
            void* lpBuffer,
            int nNumberOfBytesToWrite,
            int* lpNumberOfBytesWritten,
            OVERLAPPED lpOverlapped
        );
    }
    public class WinUSB
    {
        public const int SHORT_PACKET_TERMINATE = 0x01;
        public const int AUTO_CLEAR_STALL = 0x02;
        public const int PIPE_TRANSFER_TIMEOUT = 0x03;
        public const int IGNORE_SHORT_PACKETS = 0x04;
        public const int ALLOW_PARTIAL_READS = 0x05;
        public const int AUTO_FLUSH = 0x06;
        public const int RAW_IO = 0x07;
        public const int MAXIMUM_TRANSFER_SIZE = 0x08;
        public const int RESET_PIPE_ON_RESUME = 0x09;

        public enum USBD_PIPE_TYPE { UsbdPipeTypeControl, UsbdPipeTypeIsochronous, UsbdPipeTypeBulk, UsbdPipeTypeInterrupt };

        [StructLayout(LayoutKind.Sequential)]
        public class WINUSB_PIPE_INFORMATION
        {
            public USBD_PIPE_TYPE PipeType;
            public byte PipeId;
            public ushort MaximumPacketSize;
            public byte Interval;
        }
        [StructLayout(LayoutKind.Sequential)]
        public class WINUSB_PIPE_INFORMATION_EX
        {
            public USBD_PIPE_TYPE PipeType;
            public byte PipeId;
            public ushort MaximumPacketSize;
            public byte Interval;
            public uint MaximumBytesPerInterval;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct WINUSB_SETUP_PACKET
        {
            public byte RequestType;
            public byte Request;
            public ushort Value;
            public ushort Index;
            public ushort Length;
        }

        [DllImport("winusb", SetLastError = true)]
        static public extern unsafe bool WinUsb_Initialize
        (
            IntPtr DeviceHandle,
            IntPtr* InterfaceHandle
        );
        [DllImport("winusb", SetLastError = true)]
        static public extern unsafe bool WinUsb_Free
        (
            IntPtr InterfaceHandle
        );
        [DllImport("winusb", SetLastError = true)]
        static public extern unsafe bool WinUsb_WritePipe
        (
            IntPtr InterfaceHandle,
            byte PipeID,
            void* Buffer,
            int BufferLength,
            int *LengthTransferred,
            FileIOApiDeclarations.OVERLAPPED lpOverlapped
        );
        [DllImport("winusb", SetLastError = true)]
        static public extern unsafe bool WinUsb_ReadPipe
        (
            IntPtr InterfaceHandle,
            byte PipeID,
            void* Buffer,
            int BufferLength,
            int *LengthTransferred,
            FileIOApiDeclarations.OVERLAPPED lpOverlapped
        );
        [DllImport("winusb", SetLastError = true)]
        static public extern unsafe bool WinUsb_GetOverlappedResult
        (
            IntPtr InterfaceHandle,
            FileIOApiDeclarations.OVERLAPPED lpOverlapped,
            int* LengthTransferred,
            bool bWait
        );
        [DllImport("winusb", SetLastError = true)]
        static public extern unsafe bool WinUsb_QueryPipe
        (
            IntPtr InterfaceHandle,
            byte AlternateInterfaceNumber,
            byte PipeIndex,
            WINUSB_PIPE_INFORMATION PipeInformation
        );
        [DllImport("winusb", SetLastError = true)]
        static public extern unsafe bool WinUsb_QueryPipeEx
        (
            IntPtr InterfaceHandle,
            byte AlternateInterfaceNumber,
            byte PipeIndex,
            WINUSB_PIPE_INFORMATION_EX PipeInformationEx
        );
        [DllImport("winusb", SetLastError = true)]
        static public extern unsafe bool WinUsb_ControlTransfer
        (
            IntPtr InterfaceHandle,
            WINUSB_SETUP_PACKET SetupPacket,    // No pointer!
            void* Buffer,
            int BufferLength,
            int *LengthTransferred,
            FileIOApiDeclarations.OVERLAPPED lpOverlapped
        );
        [DllImport("winusb", SetLastError = true)]
        static public extern unsafe bool WinUsb_SetPipePolicy
        (
            IntPtr InterfaceHandle,
            byte PipeID,
            int PolicyType,
            int ValueLength,
            void* Buffer
        );
        [DllImport("winusb", SetLastError = true)]
        static public extern unsafe bool WinUsb_AbortPipe
        (
            IntPtr InterfaceHandle,
            byte PipeID
        );
    }
    public class WinComm
    {
        //******************************************************************
        //****** Communication Functions                              ******
        //******************************************************************

        [DllImport("kernel32", SetLastError = true)]
        internal static extern Boolean GetCommState(IntPtr hFile, ref DCB lpDCB);

        [DllImport("kernel32", SetLastError = true)]
        internal static extern Boolean GetCommTimeouts(IntPtr hFile, out COMMTIMEOUTS lpCommTimeouts);

        [DllImport("kernel32", SetLastError = true)]
        internal static extern Boolean BuildCommDCBAndTimeouts(String lpDef, ref DCB lpDCB, ref COMMTIMEOUTS lpCommTimeouts);

        [DllImport("kernel32", SetLastError = true)]
        internal static extern Boolean SetCommState(IntPtr hFile, [In] ref DCB lpDCB);

        [DllImport("kernel32", SetLastError = true)]
        internal static extern Boolean SetCommTimeouts(IntPtr hFile, [In] ref COMMTIMEOUTS lpCommTimeouts);

        [DllImport("kernel32", SetLastError = true)]
        internal static extern Boolean PurgeComm(IntPtr hFile, UInt32 dwFlags);

        public const UInt32 PURGE_TXABORT = 0x01;
        public const UInt32 PURGE_RXABORT = 0x02;
        public const UInt32 PURGE_TXCLEAR = 0x04;
        public const UInt32 PURGE_RXCLEAR = 0x08;

        [DllImport("kernel32", SetLastError = true)]
        internal static extern Boolean SetupComm(IntPtr hFile, UInt32 dwInQueue, UInt32 dwOutQueue);

        [StructLayout(LayoutKind.Sequential)]
        internal struct COMMTIMEOUTS
        {
            internal UInt32 ReadIntervalTimeout;
            internal UInt32 ReadTotalTimeoutMultiplier;
            internal UInt32 ReadTotalTimeoutConstant;
            internal UInt32 WriteTotalTimeoutMultiplier;
            internal UInt32 WriteTotalTimeoutConstant;
        }

        public const int DTR_CONTROL_DISABLE = 0x00;
        public const int DTR_CONTROL_ENABLE = 0x01;
        public const int DTR_CONTROL_HANDSHAKE = 0x02;
        public const int RTS_CONTROL_DISABLE = 0x00;
        public const int RTS_CONTROL_ENABLE = 0x01;
        public const int RTS_CONTROL_HANDSHAKE = 0x02;
        public const int RTS_CONTROL_TOGGLE = 0x03;

        public const int NOPARITY = 0;
        public const int ODDPARITY = 1;
        public const int EVENPARITY = 2;
        public const int MARKPARITY = 3;
        public const int SPACEPARITY = 4;

        public const int ONESTOPBIT = 0;
        public const int ONE5STOPBITS = 1;
        public const int TWOSTOPBITS = 2;

        [StructLayout(LayoutKind.Sequential)]
        internal struct DCB
        {
            /*
            9600,8,EVENPARITY,ONESTOPBIT
            typedef struct _DCB 
            {
                DWORD DCBlength;                 28
                DWORD BaudRate;                  9600
            
                DWORD fBinary: 1;                1 
                DWORD fParity: 1;                1
                DWORD fOutxCtsFlow:1;            0
                DWORD fOutxDsrFlow:1;            0
            
                DWORD fDtrControl:2;             0  (2 Bits!)
                DWORD fDsrSensitivity:1;         0
                DWORD fTXContinueOnXoff: 1;      1

                DWORD fOutX: 1;                  0
                DWORD fInX: 1;                   0
                DWORD fErrorChar: 1;             0
                DWORD fNull: 1;                  0
            
                DWORD fRtsControl:2;             0
                DWORD fAbortOnError:1;           0
                DWORD fDummy2:17;                0    -> 0x00000083
            
                WORD wReserved;                  0x0000
                WORD XonLim;                     0x0000
                WORD XoffLim;                    0x0000
                BYTE ByteSize;                   8
                BYTE Parity;                     2
                BYTE StopBits;                   0
                char XonChar;                    0x00
                char XoffChar;                   0x00
                char ErrorChar;                  0x00
                char EofChar;                    0x00 
                char EvtChar;                    0x00
                WORD wReserved1;                 0x0000
            } DCB, *LPDCB;
            */

            internal Int32 DCBlength;
            internal Int32 BaudRate;
            internal Int32 PackedValues;
            internal Int16 wReserved;
            internal Int16 XonLim;
            internal Int16 XoffLim;
            internal Byte ByteSize;
            internal Byte Parity;
            internal Byte StopBits;
            internal Byte XonChar;
            internal Byte XoffChar;
            internal Byte ErrorChar;
            internal Byte EofChar;
            internal Byte EvtChar;
            internal Int16 wReserved1;

            internal void init(bool parity, bool outCTS, bool outDSR, int dtr, bool inDSR, bool txc, bool xOut,
                bool xIn, int rts)
            {
                //JH 1.3: Was 0x8001 ans so not setting fAbortOnError - Thanks Larry Delby!
                DCBlength = 28; PackedValues = 0x4001;
                if (parity) PackedValues |= 0x0002;
                if (outCTS) PackedValues |= 0x0004;
                if (outDSR) PackedValues |= 0x0008;
                PackedValues |= ((dtr & 0x0003) << 4);
                if (inDSR) PackedValues |= 0x0040;
                if (txc) PackedValues |= 0x0080;
                if (xOut) PackedValues |= 0x0100;
                if (xIn) PackedValues |= 0x0200;
                PackedValues |= ((rts & 0x0003) << 12);

            }
        }

        [DllImport("kernel32", SetLastError = true)]
        internal static extern Boolean EscapeCommFunction(IntPtr hFile, UInt32 dwFunc);

        // Constants for dwFunc:
        internal const UInt32 SETXOFF = 1;
        internal const UInt32 SETXON = 2;
        internal const UInt32 SETRTS = 3;
        internal const UInt32 CLRRTS = 4;
        internal const UInt32 SETDTR = 5;
        internal const UInt32 CLRDTR = 6;
        internal const UInt32 RESETDEV = 7;
        internal const UInt32 SETBREAK = 8;
        internal const UInt32 CLRBREAK = 9;

        [DllImport("kernel32", SetLastError = true)]
        internal static extern Boolean GetCommModemStatus(IntPtr hFile, out UInt32 lpModemStat);

        // Constants for lpModemStat:
        internal const UInt32 MS_CTS_ON = 0x0010;
        internal const UInt32 MS_DSR_ON = 0x0020;
        internal const UInt32 MS_RING_ON = 0x0040;
        internal const UInt32 MS_RLSD_ON = 0x0080;

        [DllImport("kernel32", SetLastError = true)]
        internal static extern Boolean ClearCommError(IntPtr hFile, out UInt32 lpErrors, IntPtr lpStat);
        [DllImport("kernel32", SetLastError = true)]
        internal static extern Boolean ClearCommError(IntPtr hFile, out UInt32 lpErrors, out COMSTAT cs);

        //Constants for lpErrors:
        internal const UInt32 CE_RXOVER = 0x0001;
        internal const UInt32 CE_OVERRUN = 0x0002;
        internal const UInt32 CE_RXPARITY = 0x0004;
        internal const UInt32 CE_FRAME = 0x0008;
        internal const UInt32 CE_BREAK = 0x0010;
        internal const UInt32 CE_TXFULL = 0x0100;
        internal const UInt32 CE_PTO = 0x0200;
        internal const UInt32 CE_IOE = 0x0400;
        internal const UInt32 CE_DNS = 0x0800;
        internal const UInt32 CE_OOP = 0x1000;
        internal const UInt32 CE_MODE = 0x8000;

        [StructLayout(LayoutKind.Sequential)]
        internal struct COMSTAT
        {
            internal const uint fCtsHold = 0x1;
            internal const uint fDsrHold = 0x2;
            internal const uint fRlsdHold = 0x4;
            internal const uint fXoffHold = 0x8;
            internal const uint fXoffSent = 0x10;
            internal const uint fEof = 0x20;
            internal const uint fTxim = 0x40;
            internal UInt32 Flags;
            internal UInt32 cbInQue;
            internal UInt32 cbOutQue;
        }
    }
    public class WinSCard
    {
        public enum ResourceManagerContext
        {
            User = 0,
            System = 2
        }

        [DllImport("winscard.dll", CharSet = CharSet.Auto)]
        public static extern uint SCardConnect(
            [In] IntPtr hContext,
            [In] [MarshalAs(UnmanagedType.LPWStr)] string szReader,
            [In] Int32 dwShareMode,
            [In] Int32 dwPreferredProtocols,
            [Out] out IntPtr phCard,
            [Out] out Int32 pdwActiveProtocol);

        [StructLayout(LayoutKind.Sequential)]
        public struct SCARD_IO_REQUEST
        {
            public int dwProtocol;
            public int cbPciLength;
        }

        [DllImport("winscard.dll", CharSet = CharSet.Auto)]
        public static extern uint SCardTransmit(
            [In] IntPtr hCard,
            [In] IntPtr pioSendPci,
            [In] byte[] pbSendBuffer,
            [In] Int32 cbSendLength,
            [In, Out] IntPtr pioRecvPci,
            [Out] byte[] pbRecvBuffer,
            [In, Out] ref Int32 pcbRecvLength);

        [DllImport("winscard.dll", CharSet = CharSet.Unicode)]
        public static extern uint SCardEstablishContext(
            ResourceManagerContext scope,
            IntPtr reserved1, IntPtr reserved2,
            ref IntPtr context);

        [DllImport("winscard.dll", CharSet = CharSet.Unicode)]
        public static extern uint SCardReleaseContext(IntPtr context);

        [DllImport("winscard.dll", CharSet = CharSet.Unicode)]
        public static extern uint SCardListReaderGroups(IntPtr context,
            string groups, ref int size);

        [DllImport("winscard.dll", CharSet = CharSet.Unicode)]
        public static extern uint SCardListReaders(IntPtr context,
            string groups, string readers, ref int size);

        public const int SCARD_SCOPE_USER = 0;
        public const int SCARD_SCOPE_TERMINAL = 1;
        public const int SCARD_SCOPE_SYSTEM = 2;
        public const int SCARD_SHARE_SHARED = 2;
        public const int SCARD_SHARE_EXCLUSIVE = 1;
        public const int SCARD_SHARE_DIRECT = 3;
        public const int SCARD_PROTOCOL_UNSET = 0;
        public const int SCARD_PROTOCOL_T0 = 1;
        public const int SCARD_PROTOCOL_T1 = 2;

        public const uint SCARD_S_SUCCESS = 0;
        public const uint SCARD_W_REMOVED_CARD = 0x80100069;

        /*
        #define FILE_DEVICE_SMARTCARD                               0x00000031
        #define METHOD_BUFFERED                                     0
        #define FILE_ANY_ACCESS                                     0
        #define CTL_CODE(DeviceType, Function, Method, Access)      (((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))
        #define SCARD_CTL_CODE(code)                                CTL_CODE(FILE_DEVICE_SMARTCARD, (code), METHOD_BUFFERED, FILE_ANY_ACCESS)
        #define IOCTL_CODE_WRITE_DATA                               SCARD_CTL_CODE(2049)
        #define IOCTL_CODE_ESCAPE_WIN                               SCARD_CTL_CODE(3500)
        */

        public const uint FILE_DEVICE_SMARTCARD = 0x00000031;
        public const uint METHOD_BUFFERED = 0;
        public const uint FILE_ANY_ACCESS = 0;
        public static uint CTL_CODE(uint DeviceType, uint Function, uint Method, uint Access)
        {
            return ((DeviceType << 16) | (Access << 14) | (Function << 2) | Method);
        }
        public static uint SCARD_CTL_CODE(uint Code)
        {
            return CTL_CODE(FILE_DEVICE_SMARTCARD, Code, METHOD_BUFFERED, FILE_ANY_ACCESS);
        }
    }
}
