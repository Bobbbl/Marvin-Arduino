using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Elatec;

namespace OSDPSpy
{
    class Program
    {
        class Message
        {
            public int Address;
            public int Control;
            public byte[] RawData;

            public Message()
            {
                Address = 0;
                Control = 0;
                RawData = null;
            }

            public class SecurityBlock
            {
                int Type;
                byte[] Data;
                public SecurityBlock(byte[] RawData)
                {
                    if (RawData.Length < 2)
                        throw new ApplicationException("RawData too short for SecurityBlock");
                    Type = RawData[1];
                    Data = Tools.CopyBytesFrom(RawData, 2);
                }
                public int GetRawLength()
                {
                    return 2 + Data.Length;
                }
                public override string ToString()
                {
                    return string.Format("({0} {1})", Type, Tools.ConvBytesToHexString(Data));
                }
            }

            public enum Types { COMMAND, RESPONSE };

            public Types Type;
            public SecurityBlock SecBlock;
            public int Command;
            public byte[] Data;

            public void Decode()
            {
                Type = (Address & 0x80) == 0x00 ? Types.COMMAND : Types.RESPONSE;
                SecBlock = null;
                Command = 0;
                Data = null;

                int Pos = 0;
                if ((Control & 0x08) != 0)
                {
                    SecBlock = new SecurityBlock(RawData);
                    Pos = SecBlock.GetRawLength();
                }
                if (Pos >= RawData.Length)
                    throw new ApplicationException("RawData too short");
                Command = RawData[Pos++];
                if (Pos < RawData.Length)
                    Data = Tools.CopyBytesFrom(RawData, Pos);
            }

            public string GetCommandString()
            {
                switch (Type)
                {
                    case Types.COMMAND:
                        switch (Command)
                        {
                            case 0x60: return "POLL";
                            case 0x61: return "ID";
                            case 0x62: return "CAP";
                            case 0x63: return "DIAG";
                            case 0x64: return "LSTAT";
                            case 0x65: return "ISTAT";
                            case 0x66: return "OSTAT";
                            case 0x67: return "RSTAT";
                            case 0x68: return "OUT";
                            case 0x69: return "LED";
                            case 0x6A: return "BUZ";
                            case 0x6B: return "TEXT";
                            case 0x6D: return "TDSET";
                            case 0x6E: return "COMSET";
                            case 0x6F: return "DATA";
                            case 0x71: return "PROMPT";
                            case 0x73: return "BIOREAD";
                            case 0x74: return "BIOMATCH";
                            case 0x75: return "KEYSET";
                            case 0x76: return "CHLNG";
                            case 0x77: return "SCRYPT";
                            case 0x7A: return "ABORT";
                            case 0x7B: return "MAXREPLY";
                            default:   return string.Format("({0})",Command.ToString("X2"));
                        }
                    case Types.RESPONSE:
                        switch (Command)
                        {
                            case 0x40: return "ACK";
                            case 0x41: return "NAK";
                            case 0x45: return "PDID";
                            case 0x46: return "PDCAP";
                            case 0x48: return "LSTATR";
                            case 0x49: return "ISTATR";
                            case 0x4A: return "OSTATR";
                            case 0x4B: return "RSTATR";
                            case 0x50: return "RAW";
                            case 0x51: return "FMT";
                            case 0x53: return "KEYPPAD";
                            case 0x54: return "COM";
                            case 0x57: return "BIOREADR";
                            case 0x58: return "BIOMATCHR";
                            case 0x76: return "CCRYPT";
                            case 0x78: return "RMAC_I";
                            case 0x90: return "MFGREP";
                            case 0x79: return "BUSY";
                            case 0xB1: return "XRD";
                            default:   return string.Format("({0})",Command.ToString("X2"));
                        }
                }
                return "?";
            }

            public string ToString(string Format)
            {
                switch (Format)
                {
                    case "":
                    case "R":
                        return string.Format("A{0} L{1} R{2}", Address.ToString("X2"), Control.ToString("X2"), Tools.ConvBytesToHexString(RawData));
                    case "D":
                        string String = string.Format("A{0} L{1} {2}", Address.ToString("X2"), Control.ToString("X2"), GetCommandString());
                        if (SecBlock != null)
                            String += string.Format(" S({0})", SecBlock);
                        if (Data != null)
                            String += string.Format(" D({0})", Tools.ConvBytesToHexString(Data));
                        return String;
                }
                return "Illegal format";
            }

            public override string ToString()
            {
                return ToString("R");
            }
        }

        class MessageReceiver
        {
            enum RXStates { WAIT_SOM, READ_ADDRESS, READ_LEN_LSB, READ_LEN_MSB, READ_CTRL, READ_DATA, READ_CKSUM, READ_CRC_LSB, READ_CRC_MSB };

            Elatec.IO.Ports.SerialPort Port;
            RXStates State;
            Message Message;
            int Length;
            int Pos;
            public Logger Log;

            public MessageReceiver(Elatec.IO.Ports.SerialPort Port)
            {
                this.Port = Port;
                this.State = RXStates.WAIT_SOM;
                this.Message = new Message();
                this.Pos = 0;
                this.Log = new Logger();
            }

            public Message Test()
            {
                while (Port.BytesToRead > 0)
                {
                    int Byte = Port.ReadByte();
                    Log.Write("{0} {1} -> ", Byte.ToString("X2"), State);
                    switch (State)
                    {
                        case RXStates.WAIT_SOM:
                            if (Byte != 0x53)
                                break;
                            State = RXStates.READ_ADDRESS;
                            break;
                        case RXStates.READ_ADDRESS:
                            Message.Address = Byte;
                            State = RXStates.READ_LEN_LSB;
                            break;
                        case RXStates.READ_LEN_LSB:
                            Length = Byte;
                            State = RXStates.READ_LEN_MSB;
                            break;
                        case RXStates.READ_LEN_MSB:
                            Length += Byte << 8;
                            // Minimum length = SOM(1)+ADR(1)+LEN(2)+CTRL(1)+CKSUM(1) = 6
                            if (Length < 6)
                            {
                                State = RXStates.WAIT_SOM;
                                break;
                            }
                            State = RXStates.READ_CTRL;
                            break;
                        case RXStates.READ_CTRL:
                            Message.Control = Byte;
                            int RawDataLength = Length - ((Message.Control & 0x04) == 0x00 ? 6 : 7);
                            if (RawDataLength < 0)
                            {
                                State = RXStates.WAIT_SOM;
                                break;
                            }
                            Message.RawData = new byte[RawDataLength];
                            Pos = 0;
                            if (Pos == Message.RawData.Length)
                                State = (Message.Control & 0x04) != 0x00 ? RXStates.READ_CRC_LSB : RXStates.READ_CKSUM;
                            else
                                State = RXStates.READ_DATA;
                            break;
                        case RXStates.READ_DATA:
                            Message.RawData[Pos++] = (byte)Byte;
                            if (Pos == Message.RawData.Length)
                                State = (Message.Control & 0x04) != 0x00 ? RXStates.READ_CRC_LSB : RXStates.READ_CKSUM;
                            break;
                        case RXStates.READ_CKSUM:
                            Log.Write(":-) ");
                            State = RXStates.WAIT_SOM;
                            Message CompleteMessage1 = Message;
                            Message = new Message();
                            CompleteMessage1.Decode();
                            return CompleteMessage1;
                        case RXStates.READ_CRC_LSB:
                            State = RXStates.READ_CRC_MSB;
                            break;
                        case RXStates.READ_CRC_MSB:
                            Log.Write(":-)) ");
                            Message CompleteMessage2 = Message;
                            Message = new Message();
                            CompleteMessage2.Decode();
                            return CompleteMessage2;
                    }
                    Log.WriteLine("{0}", State);
                }
                return null;
            }
        }

        enum TrackStates { EXPECT_COMMAND, EXPECT_RESPONSE };

        static void Main(string[] args)
        {
            Logger Log = new Logger();
            Log.AddLogger(new ConsoleLogger());

            try
            {
                if (args.Length < 1 || args.Length > 2)
                    throw new ApplicationException("USAGE: OSDPSpy <Port> [<BaudRate>]");
                string PortName = args[0];
                int BaudRate = 19200;
                if (args.Length >= 2)
                    BaudRate = Convert.ToInt32(args[1]);
                Elatec.IO.Ports.SerialPort Port = new Elatec.IO.Ports.SerialPort(PortName, BaudRate, Elatec.IO.Ports.Parity.None, 8, Elatec.IO.Ports.StopBits.One);
                MessageReceiver RX = new MessageReceiver(Port);
                Log.Write("Open {0}, {1} baud: ", PortName, BaudRate);
                Port.Open();
                Log.WriteLine("OK");

                TrackStates TrackState = TrackStates.EXPECT_COMMAND;
                int ExpectedAddress = 0;
                const string Format = "D";

                List<int> AddressFilter = new List<int>();
                for (int Address = 0; Address <= 127; Address++)
                    AddressFilter.Add(Address);

                while (true)
                {
                    if (Console.KeyAvailable)
                    {
                        ConsoleKeyInfo Key = Console.ReadKey();
                        switch (Key.KeyChar)
                        {
                            case '\r':
                                throw new ApplicationException("Quit");
                            case '0':
                            case '1':
                                int ToggleAddress = Key.KeyChar - '0';
                                if (AddressFilter.Contains(ToggleAddress))
                                    AddressFilter.Remove(ToggleAddress);
                                else
                                    AddressFilter.Add(ToggleAddress);
                                Log.WriteLine(" -> {0}", AddressFilter.Contains(ToggleAddress) ? "On" : "Off");
                                break;
                            default:
                                Log.WriteLine("Unknown command");
                                break;
                        }
                    }
                    Message Message = RX.Test();
                    if (Message != null)
                    {
                        switch (TrackState)
                        {
                            case TrackStates.EXPECT_COMMAND:
                                if (AddressFilter.Contains(Message.Address & 0x7F))
                                {
                                    if (Message.Type != Message.Types.COMMAND)
                                    {
                                        Log.WriteLine(" ??? ({0})", Message.ToString(Format));
                                        break;
                                    }
                                    Log.Write(Message.ToString(Format));
                                }
                                ExpectedAddress = Message.Address;
                                TrackState = TrackStates.EXPECT_RESPONSE;
                                break;

                            case TrackStates.EXPECT_RESPONSE:
                                if (AddressFilter.Contains(Message.Address & 0x7F))
                                {
                                    if (Message.Type != Message.Types.RESPONSE)
                                    {
                                        Log.WriteLine(" ??? ");
                                        Log.Write(Message.ToString(Format));
                                        ExpectedAddress = Message.Address;
                                        TrackState = TrackStates.EXPECT_RESPONSE;
                                        break;
                                    }
                                    Log.WriteLine(" {0} {1}", (Message.Address & 0x7F) != ExpectedAddress ? "???" : "<->", Message.ToString(Format));
                                }
                                TrackState = TrackStates.EXPECT_COMMAND;
                                break;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Log.WriteLine(ex.Message);
                Console.ReadKey();
            }
        }
    }
}
