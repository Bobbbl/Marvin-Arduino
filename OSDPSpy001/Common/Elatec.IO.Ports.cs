using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using Microsoft.Win32;
using Microsoft.Win32.SafeHandles;
using System.Threading;
using System.Net;
using System.Net.Sockets;
using Elatec;

namespace Elatec.IO.Ports
{
    public enum Parity { None, Odd, Even, Mark, Space };

    public enum StopBits { One, OnePointFive, Two };

    public class SerialPort
    {
        IntPtr Handle;
        
        public string PortName;
        public int BaudRate;
        public Parity Parity;
        public int DataBits;
        public StopBits StopBits;
        public string NewLine;
        public int WriteTimeoutValue;
        public int ReadTimeoutValue;
        public bool RtsValue;
        public bool DtrValue;

        // ****** Constructor ***************************************

        public void Init(string PortName, int BaudRate, Parity Parity, int DataBits, StopBits StopBits,string NewLine,int WriteTimeout,int ReadTimeout)
        {
            int i = 0;
            try
            {
                i = 1;
                Handle = new IntPtr(-1);
                i = 2;
                this.PortName = PortName;
                i = 3;
                this.BaudRate = BaudRate;
                i = 4;
                this.Parity = Parity;
                i = 5;
                this.DataBits = DataBits;
                i = 6;
                this.StopBits = StopBits;
                i = 7;
                this.NewLine = NewLine;
                i = 8;
                this.WriteTimeoutValue = WriteTimeout;
                i = 9;
                this.ReadTimeoutValue = ReadTimeout;
                i = 10;
            }
            catch (Exception ex)
            {
                throw new ApplicationException(i.ToString() + " " + ex.Message);
            }
        }
        public SerialPort(string PortName, int BaudRate, Parity Parity, int DataBits, StopBits StopBits)
        {
            Init(PortName, BaudRate, Parity, DataBits, StopBits, "", 1000, 1000);
        }
        public SerialPort()
        {
            Init("COM1", 9600, Parity.None, 8, StopBits.One, "", 1000, 1000);
        }

        // ****** Member Functions **********************************

        public void Open()
        {
            try
            {
                Close();

                string Win32PortName = PortName;
                if (Win32PortName.Length <= 4)
                    Win32PortName = Win32PortName + ":";
                else
                    Win32PortName = @"\\.\" + Win32PortName;

                Handle = FileIOApiDeclarations.CreateFile(
                    Win32PortName,
                    FileIOApiDeclarations.GENERIC_READ | FileIOApiDeclarations.GENERIC_WRITE,
                    0,
                    null,
                    FileIOApiDeclarations.OPEN_EXISTING,
                    0,
                    IntPtr.Zero);
                if (Handle == new IntPtr(-1))
                    throw new ApplicationException("Unable to open port");

                if (!WinComm.SetupComm(Handle, 10000, 10000))
                    throw new ApplicationException("SetupComm failed");

                WinComm.DCB DCB = new WinComm.DCB();
                DCB.init(Parity != Parity.None,
                         false, false,
                         DtrValue ? WinComm.DTR_CONTROL_ENABLE : WinComm.DTR_CONTROL_DISABLE,
                         false, false, false, false,
                         RtsValue ? WinComm.RTS_CONTROL_ENABLE : WinComm.RTS_CONTROL_DISABLE);
                       
                DCB.BaudRate = BaudRate;
                DCB.wReserved = (short)0;
                DCB.XonLim = (short)0;
                DCB.XoffLim = (short)0;
                DCB.ByteSize = (byte)DataBits;
                DCB.Parity = (byte)Parity;
                DCB.StopBits = (byte)StopBits;
                DCB.XonChar = (byte)0;
                DCB.XoffChar = (byte)0;
                DCB.ErrorChar = (byte)0;
                DCB.EofChar = (byte)0;
                DCB.EvtChar = (byte)0;
                DCB.wReserved1 = (short)0;
                if (!WinComm.SetCommState(Handle, ref DCB))
                    throw new ApplicationException("SetCommState failed");

                SetCommTimeouts();

                if (!WinComm.PurgeComm(Handle, WinComm.PURGE_RXCLEAR | WinComm.PURGE_TXCLEAR))
                    throw new ApplicationException("PurgeComm failed");
            }
            catch (Exception ex)
            {
                Close();
                throw ex;
            }
        }
        public void Close()
        {
            if (!IsOpen)
                return;
            FileIOApiDeclarations.CloseHandle(Handle);
            Handle = new IntPtr(-1);
        }

        // ****** Read Functions ************************************

        public unsafe int Read(byte[] Bytes, int Offset, int Count)
        {
            if (!IsOpen)
                throw new ApplicationException("Port is closed");
            ClearCommError();
            byte[] ReadBuffer = new byte[Count];
            int BytesRead = 0;
            fixed (void* p = ReadBuffer)
            {
                if (!FileIOApiDeclarations.ReadFile(Handle, p, Count, &BytesRead, null))
                    throw new ApplicationException("ReadFile failed");
            }
            Tools.CopyBytes(Bytes, Offset, ReadBuffer, 0, (int)BytesRead);
            if (BytesRead < Count)
                throw new ApplicationException("Timeout occured");
            return (int)BytesRead;
        }
        public int ReadByte()
        {
            byte[] ReadBuffer = new byte[1];
            Read(ReadBuffer, 0, 1);
            return ReadBuffer[0];
        }
        public string ReadLine()
        {
            List<byte> LineBytes = new List<byte>();
            ASCIIEncoding Enc = new ASCIIEncoding();
            byte[] NewLineBytes = Enc.GetBytes(NewLine);
            int NewLinePos = 0;
            while (NewLinePos < NewLineBytes.Length)
            {
                byte Byte = (byte)ReadByte();
                if (Byte == NewLineBytes[NewLinePos])
                    NewLinePos++;
                else
                {
                    NewLinePos = 0;
                    LineBytes.Add(Byte);
                }
            }
            byte[] LineBytesArray = Tools.CopyBytesFrom(LineBytes);
            return Enc.GetString(LineBytesArray, 0, LineBytesArray.Length);
        }

        // ****** Write Functions ***********************************

        public unsafe int Write(byte[] Bytes, int Offset, int Count)
        {
            if (!IsOpen)
                throw new ApplicationException("Port is closed");
            ClearCommError();
            byte[] WriteBuffer = new byte[Count];
            Tools.CopyBytes(WriteBuffer,0, Bytes, Offset, Count);
            int BytesWritten;
            fixed (void* p = WriteBuffer)
            {
                if (!FileIOApiDeclarations.WriteFile(Handle, p, Count, &BytesWritten, null))
                    throw new ApplicationException("WriteFile failed");
            }
            if (BytesWritten < Count)
                throw new ApplicationException("Timeout occured");
            return (int)BytesWritten;
        }
        public void Write(string Line)
        {
            ASCIIEncoding Enc = new ASCIIEncoding();
            byte[] Bytes = Enc.GetBytes(Line);
            Write(Bytes, 0, Bytes.Length);
        }
        public void WriteLine(string Line)
        {
            ASCIIEncoding Enc = new ASCIIEncoding();
            byte[] Bytes = Enc.GetBytes(Line + NewLine);
            Write(Bytes, 0, Bytes.Length);
        }

        // ****** Properties ****************************************

        private WinComm.COMSTAT GetComStat()
        {
            uint CommError;
            WinComm.COMSTAT ComStat;
            if (!WinComm.ClearCommError(Handle, out CommError, out ComStat))
                throw new ApplicationException("ClearCommError failed (" + Win32.GetLastError().ToString()+")");
            return ComStat;
        }

        public bool IsOpen
        {
            get
            {
                return Handle != new IntPtr(-1);
            }
        }
        public int BytesToRead
        {
            get
            {
                return (int)GetComStat().cbInQue;
            }
        }
        public int BytesToWrite
        {
            get
            {
                return (int)GetComStat().cbOutQue;
            }
        }
        public int WriteTimeout
        {
            set
            {
                WriteTimeoutValue = value;
                SetCommTimeouts();
            }
            get
            {
                return WriteTimeoutValue;
            }
        }
        public int ReadTimeout
        {
            set
            {
                ReadTimeoutValue = value;
                SetCommTimeouts();
            }
            get
            {
                return ReadTimeoutValue;
            }
        }
        public bool RtsEnable
        {
            set
            {
                RtsValue = value;
                if (!IsOpen)
                    throw new ApplicationException("Port is closed");
                if (!WinComm.EscapeCommFunction(Handle, RtsValue ? WinComm.SETRTS : WinComm.CLRRTS))
                    throw new ApplicationException("EscapeCommFunction failed");
            }
            get
            {
                return RtsValue;
            }
        }
        public bool DtrEnable
        {
            set
            {
                DtrValue = value;
                if (!IsOpen)
                    throw new ApplicationException("Port is closed");
                if (!WinComm.EscapeCommFunction(Handle, DtrValue ? WinComm.SETDTR : WinComm.CLRDTR))
                    throw new ApplicationException("EscapeCommFunction failed");
            }
            get
            {
                return DtrValue;
            }
        }

        // ****** Helper Functions **********************************

        public void SetCommTimeouts()
        {
            if (!IsOpen)
                return;
            WinComm.COMMTIMEOUTS CommTimeouts = new WinComm.COMMTIMEOUTS();
            CommTimeouts.ReadIntervalTimeout = 0;
            CommTimeouts.ReadTotalTimeoutMultiplier = 1;
            CommTimeouts.ReadTotalTimeoutConstant = (uint)ReadTimeoutValue;
            CommTimeouts.WriteTotalTimeoutMultiplier = 1;
            CommTimeouts.WriteTotalTimeoutConstant = (uint)WriteTimeoutValue;
            if (!WinComm.SetCommTimeouts(Handle, ref CommTimeouts))
                throw new ApplicationException("SetCommTimeouts failed");
        }
        public void ClearCommError()
        {
            uint CommError;
            WinComm.COMSTAT ComStat;
            if (!WinComm.ClearCommError(Handle, out CommError, out ComStat))
                throw new ApplicationException("ClearCommError failed");
            /*
            if (CommError != 0)
                throw new ApplicationException("Communication error occured");
            */
        }
    }
}