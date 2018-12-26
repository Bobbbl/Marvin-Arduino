using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Win32;
using System.IO;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;

/*06.10.2015   F.Eglsoer
 *  -fixed bug in ConvHexStringToBytes()
    -changed bit order in BitArray.ToString()
 */
namespace Elatec
{
    public class GlobalBuffer : IDisposable
    {
        /************************************************************/
        /****** Globally Allocated Memory Buffer ********************/
        /************************************************************/

        public IntPtr Handle;
        bool Disposed;
        public GlobalBuffer(int Size)
        {
            Handle = Marshal.AllocHGlobal(Size);
            Disposed = false;
        }
        public void Dispose()
        {
            Dispose(true);
        }
        protected virtual void Dispose(bool disposing)
        {
            if (Disposed)
                return;
            if (!disposing)
                return;
            Marshal.FreeHGlobal(Handle);
            Disposed = true;
        }
    }

    public class Tools
    {
        /************************************************************/
        /****** File Read/Write Functions ***************************/
        /************************************************************/

        public static byte[] ReadFile(string Name, long MaxSize)
        {
            FileStream fs = new FileStream(Name, FileMode.Open);
            if (fs.Length > MaxSize)
                throw new ApplicationException("file \"" + Path.GetFileName(Name) + "\" appears somewhat big");
            BinaryReader br = new BinaryReader(fs);
            byte[] Data = br.ReadBytes((int)fs.Length);
            br.Close();
            return Data;
        }

        public static void WriteFile(string Name, byte[] Data)
        {
            FileStream fs = new FileStream(Name, FileMode.Create);
            BinaryWriter bw = new BinaryWriter(fs);
            bw.Write(Data);
            bw.Close();
        }

        /************************************************************/
        /****** Registry Functions **********************************/
        /************************************************************/

        public static string RegQuerySZ(string SubKeyName, string ValueName)
        {
            RegistryKey Key = Registry.LocalMachine.OpenSubKey(SubKeyName);
            if (Key == null)
                return null;
            object Value = Key.GetValue(ValueName);
            Key.Close();
            if (Value == null)
                return null;
            return Value.ToString();
        }
        public static string FindUSBDevice(string Driver, string DevicePath)
        {
            int PortIndex = 0;
            while (true)
            {
                string Device = RegQuerySZ("SYSTEM\\CurrentControlSet\\Services\\" + Driver + "\\Enum", PortIndex.ToString());
                // Never check this index again
                PortIndex++;
                // We did not found this index, therefore we tested all existing index
                if (Device == null)
                    return null;
                // Test, if this is, what we are interested in
                if (Device.ToUpper().StartsWith(DevicePath))
                    return Device;
            }
        }
        public static string ReadPortName(string Device)
        {
            return RegQuerySZ("SYSTEM\\CurrentControlSet\\Enum\\" + Device + "\\Device Parameters", "PortName");
        }

        /************************************************************/
        /****** Byte Manipulation Functions *************************/
        /************************************************************/

        public static void CopyBytes(byte[] Dest, int DestOffset, byte[] Source, int SourceOffset, int ByteCount)
        {
            for (int i = 0; i < ByteCount; i++)
                Dest[DestOffset + i] = Source[SourceOffset + i];
        }


        public static byte[] CopyBytesFrom(byte[] Source, int SourceOffset, int ByteCount)
        {
            byte[] Dest = new byte[ByteCount];
            CopyBytes(Dest, 0, Source, SourceOffset, ByteCount);
            return Dest;
        }
        public static byte[] CopyBytesFrom(byte[] Source, int SourceOffset)
        {
            return CopyBytesFrom(Source, SourceOffset, Source.Length - SourceOffset);
        }
        public static byte[] CopyBytesFrom(byte[] Source)
        {
            return CopyBytesFrom(Source, 0, Source.Length);
        }

        public static byte[] CopyBytesFrom(List<byte> Source, int SourceOffset, int ByteCount)
        {
            byte[] Dest = new byte[ByteCount];
            for (int i = 0; i < ByteCount; i++)
                Dest[i] = Source[SourceOffset + i];
            return Dest;
        }
        public static byte[] CopyBytesFrom(List<byte> Source, int SourceOffset)
        {
            return CopyBytesFrom(Source, SourceOffset, Source.Count - SourceOffset);
        }
        public static byte[] CopyBytesFrom(List<byte> Source)
        {
            return CopyBytesFrom(Source, 0, Source.Count);
        }

        public static void FillBytes(byte[] Dest, int DestOffset, byte Value, int ByteCount)
        {
            for (int i = 0; i < ByteCount; i++)
                Dest[DestOffset + i] = Value;
        }
        public static byte[] FillBytes(byte Value, int ByteCount)
        {
            byte[] Bytes = new byte[ByteCount];
            FillBytes(Bytes, 0, Value, ByteCount);
            return Bytes;
        }

        public static bool CompBytes(byte[] Source1, int Source1Offset, byte[] Source2, int Source2Offset, int ByteCount)
        {
            for (int i = 0; i < ByteCount; i++)
                if (Source1[Source1Offset + i] != Source2[Source2Offset + i])
                    return false;
            return true;
        }
        public static bool CompBytes(byte[] Source1,byte[] Source2)
        {
            if (Source1.Length != Source2.Length)
                return false;
            return CompBytes(Source1, 0, Source2, 0, Source1.Length);
        }

        public static int FindByte(byte[] Bytes, byte Byte)
        {
            for (int i = 0; i < Bytes.Length; i++)
                if (Bytes[i] == Byte)
                    return i;
            return -1;
        }

        /************************************************************/
        /****** Conversion Functions ********************************/
        /************************************************************/

        public static string ConvBytesToHexString(byte[] Bytes, int Offset, int Length)
        {
            if (Bytes == null)
                return "null";
            string HexString = "";
            for (int i = 0; i < Length; i++)
            {
                if (i > 0)
                    HexString += " ";
                HexString += Bytes[Offset + i].ToString("X2");
            }
            return HexString;
        }
        public static string ConvBytesToHexString(byte[] Bytes)
        {
            if (Bytes == null)
                return "null";
            return ConvBytesToHexString(Bytes, 0, Bytes.Length);
        }

        public static string ConvBytesToHexString2(byte[] Bytes, int Offset, int Length)
        {
            if (Bytes == null)
                return "null";
            string HexString = "";
            for (int i = 0; i < Length; i++)
                HexString += Bytes[Offset + i].ToString("X2");
            return HexString;
        }
        public static string ConvBytesToHexString2(byte[] Bytes)
        {
            if (Bytes == null)
                return "null";
            return ConvBytesToHexString2(Bytes, 0, Bytes.Length);
        }

        public static string ConvBytesToCHexString(byte[] Bytes, int Offset, int Length)
        {
            string String = "";
            for (int i = 0; i < Bytes.Length; i++)
            {
                String += "0x" + Bytes[i].ToString("X2");
                if (i < Bytes.Length - 1)
                    String += ",";
            }
            return String;
        }
        public static string ConvBytesToCHexString(byte[] Bytes)
        {
            if (Bytes == null)
                return "null";
            return ConvBytesToCHexString(Bytes, 0, Bytes.Length);
        }

        public static int ConvHexCharToByte(char Char)
        {
            if (Char >= '0' && Char <= '9')
                return (byte)(Char - '0');
            if (Char >= 'A' && Char <= 'F')
                return (byte)(Char - 'A' + 10);
            if (Char >= 'a' && Char <= 'f')
                return (byte)(Char - 'a' + 10);
            return -1;
        }

        // Bytes are represented by two characters each
        public static byte[] ConvHexStringToBytes1(string HexString)
        {
            List<byte> Bytes = new List<byte>();
            int i = 0;
            while (true)
            {
                int Nibble, Value;

                if (i == HexString.Length)
                    break;
                Nibble = ConvHexCharToByte(HexString[i++]);
                if (Nibble < 0)
                    return null;
                Value = Nibble << 4;

                if (i == HexString.Length)
                    return null;
                Nibble = ConvHexCharToByte(HexString[i++]);
                if (Nibble < 0)
                    return null;
                Value += Nibble;

                Bytes.Add((byte)Value);
            }
            return Tools.CopyBytesFrom(Bytes);
        }

        // Bytes are separated by spaces
        public static byte[] ConvHexStringToBytes2(string HexString)
        {
            List<byte> Bytes = new List<byte>();
            int i = 0;
            while (true)
            {
                if (i == HexString.Length)
                    break;
                if (HexString[i] == ' ')
                {
                    i++;
                    continue;
                }
                int Value = 0;
                while (true)
                {
                    int Nibble = ConvHexCharToByte(HexString[i]);
                    if (Nibble < 0)
                        return null;
                    Value <<= 4;
                    Value += Nibble;

                    i++;
                    if (i == HexString.Length)
                        break;
                    if (HexString[i] == ' ')
                        break;
                }
                if (Value > 255)
                    return null;
                Bytes.Add((byte)Value);
            }
            return Tools.CopyBytesFrom(Bytes);
        }
    }

    // ********************************************************************
    // ****** Data Conversion Functions ***********************************
    // ********************************************************************

    public abstract class ByteReader
    {
        public abstract byte ReadByte();
        public bool ReadBool()
        {
            return ReadByte() != 0;
        }
        public short ReadShort()
        {
            return (short)(ReadByte() + (ReadByte() << 8));
        }
        public ushort ReadUShort()
        {
            return (ushort)(ReadByte() + (ReadByte() << 8));
        }
        public int ReadInt()
        {
            int Int = ReadUShort() + (ReadUShort() << 16);
            return Int;
        }
        public uint ReadUInt()
        {
            return (uint)(ReadUShort() + (ReadUShort() << 16));
        }
        public byte[] ReadBytes(int ByteCount)
        {
            byte[] Bytes = new byte[ByteCount];
            for (int i = 0; i < ByteCount; i++)
                Bytes[i] = (byte)ReadByte();
            return Bytes;
        }
        public int[] ReadInts(int IntCount)
        {
            int[] Ints = new int[IntCount];
            for (int i = 0; i < IntCount; i++)
                Ints[i] = ReadInt();
            return Ints;
        }
        public string ReadString()
        {
            return Encoding.ASCII.GetString(ReadBytes(ReadInt()));
        }
        public string ReadNullString()
        {
            return ReadBool() ? ReadString() : null;
        }
    }

    public abstract class ByteWriter
    {
        public abstract void WriteByte(int Byte);
        public void WriteBool(bool Bool)
        {
            WriteByte(Bool ? 1 : 0);
        }
        public void WriteShort(int Short)
        {
            WriteByte(Short);
            WriteByte(Short >> 8);
        }
        public void WriteUShort(uint UShort)
        {
            WriteByte((byte)UShort);
            WriteByte((byte)(UShort >> 8));
        }
        public void WriteInt(int Int)
        {
            WriteShort(Int);
            WriteShort(Int >> 16);
        }
        public void WriteUInt(uint UInt)
        {
            WriteUShort(UInt);
            WriteUShort(UInt >> 16);
        }
        public void WriteBytes(List<byte> Bytes)
        {
            foreach (byte Byte in Bytes)
                WriteByte(Byte);
        }
        public void WriteBytes(byte[] Bytes)
        {
            foreach (byte Byte in Bytes)
                WriteByte(Byte);
        }
        public void WriteBytes(byte[] Bytes, int Offset, int Count)
        {
            int Index = Offset;
            for (int i = 0; i < Count; i++)
                WriteByte(Bytes[Index++]);
        }
        public void WriteString(string String)
        {
            byte[] Bytes = Encoding.ASCII.GetBytes(String);
            WriteInt(Bytes.Length);
            WriteBytes(Bytes);
        }
        public void WriteNullString(string String)
        {
            WriteBool(String != null);
            if (String == null)
                return;
            WriteString(String);
        }
    }

    // ****** ByteListReader/Writer *********************************

    public class ByteListReader : ByteReader
    {
        List<byte> Bytes;
        int Pos;
        public ByteListReader(List<byte> Bytes)
        {
            this.Bytes = Bytes;
            Pos = 0;
        }
        public override byte ReadByte()
        {
            if (Pos >= Bytes.Count)
                throw new ApplicationException("Out of data!");
            return Bytes[Pos++];
        }
    }
    public class ByteListWriter : ByteWriter
    {
        List<byte> Bytes;
        public ByteListWriter(List<byte> Bytes)
        {
            this.Bytes = Bytes;
        }
        public override void WriteByte(int Byte)
        {
            Bytes.Add((byte)Byte);
        }
        public byte[] GetByteArray()
        {
            byte[] ByteArray = new byte[Bytes.Count];
            return Tools.CopyBytesFrom(Bytes);
        }
    }

    // ****** ByteArrayReader/Writer ********************************

    public class ByteArrayReader : ByteReader
    {
        byte[] Bytes;
        int Pos;
        public ByteArrayReader(byte[] Bytes)
        {
            this.Bytes = Bytes;
            Pos = 0;
        }
        public override byte ReadByte()
        {
            if (Pos >= Bytes.Length)
                throw new ApplicationException("Out of data!");
            return Bytes[Pos++];
        }
    }

    // ****** ByteStreamReader/Writer *******************************

    public class ByteStreamReader : ByteReader
    {
        protected Stream s;
        public ByteStreamReader()
        {
            this.s = null;
        }
        public ByteStreamReader(Stream s)
        {
            this.s = s;
        }
        public override byte ReadByte()
        {
            int Byte = s.ReadByte();
            if (Byte < 0)
                throw new ApplicationException("Out of data!");
            return (byte)Byte;
        }
    }
    public class ByteStreamWriter : ByteWriter
    {
        protected Stream s;
        public ByteStreamWriter()
        {
            this.s = null;
        }
        public ByteStreamWriter(Stream s)
        {
            this.s = s;
        }
        public override void WriteByte(int Byte)
        {
            s.WriteByte((byte)Byte);
        }
    }

    // ****** BitArray **********************************************

    public class BitArray : List<byte>
    {
        public int BitCount;
        byte Byte;
        int Pos;
        void Init()
        {
            BitCount = 0;
            Pos = 0;
            Byte = 0;
        }
        public BitArray()
        {
            Init();
        }
        public BitArray(byte[] Bits, int BitCount)
        {
            Init();
            for (int BitIndex = 0; BitIndex < BitCount; BitIndex++)
                AddBit(ReadBit(Bits, BitIndex));
            Finish();
        }
        public void Finish()
        {
            if (Pos == 0)
                return;
            Add(Byte);
            Pos = 0;
            Byte = 0;
        }
        public void AddBit(bool Bit)
        {
            if (Pos >= 8)
                Finish();
            if (Bit)
                Byte |= (byte)(0x80 >> Pos);
            Pos++;
            BitCount++;
        }
        public void AddBits(uint Data, int BitCnt)
        {
            uint Mask = 1U << (BitCnt - 1);
            for (; BitCnt > 0; BitCnt--)
            {
                AddBit((Data & Mask) != 0);
                Mask >>= 1;
            }
        }
        public void AddBytes(byte[] Bytes)
        {
            for (int i = 0; i < Bytes.Length; i++)
                AddByte(Bytes[i]);
        }
        public void AddString(string String, int ByteCnt)
        {
            System.Text.UTF8Encoding Encoder = new System.Text.UTF8Encoding();
            byte[] Bytes;
            while (true)
            {
                Bytes = Encoder.GetBytes(String);
                if (Bytes.Length <= ByteCnt)
                    break;
                String = String.Substring(0, String.Length - 1);
            }
            AddBytes(Bytes);
            if (Bytes.Length < ByteCnt)
                AddSpareBytes(ByteCnt - Bytes.Length);
        }
        public void AddFlag(bool Flag)
        {
            AddBit(Flag);
        }
        public void AddByte(byte Byte)
        {
            AddBits(Byte, 8);
        }
        public void AddUInt16(UInt16 UInt)
        {
            // We are doing little endian
            AddByte((byte)UInt);            // LSB
            AddByte((byte)(UInt >> 8));     // MSB
        }
        public void AddUInt32(UInt32 UInt)
        {
            // We are doing little endian
            AddUInt16((byte)UInt);          // LSB
            AddUInt16((byte)(UInt >> 16));  // MSB
        }
        public void AddInt16(Int16 UInt)
        {
            // We are doing little endian
            AddByte((byte)UInt);            // LSB
            AddByte((byte)(UInt >> 8));     // MSB
        }
        public void AddInt32(Int32 UInt)
        {
            // We are doing little endian
            AddInt16((byte)UInt);           // LSB
            AddInt16((byte)(UInt >> 16));   // MSB
        }
        public void AddBool32(bool Bool)
        {
            AddInt32(Bool ? 1 : 0);
        }

        public void AddIdenticalBits(bool Bit, int Count)
        {
            for (int i = 0; i < Count; i++)
                AddBit(Bit);
        }
        public void AddIdenticalBytes(byte Byte, int Count)
        {
            for (int i = 0; i < Count; i++)
                AddByte(Byte);
        }
        public void AddSpareBits(int Count)
        {
            AddIdenticalBits(false, Count);
        }
        public void AddSpareBytes(int Count)
        {
            for (int i = 0; i < Count; i++)
                AddByte(0);
        }

        public static bool ReadBit(byte[] Bits, int BitIndex)
        {
            return ((Bits[BitIndex / 8] & (0x80 >> (BitIndex % 8))) != 0);
        }
        public static bool ReadBit(List<byte> Bits, int BitIndex)
        {
            return ((Bits[BitIndex / 8] & (0x80 >> (BitIndex % 8))) != 0);
        }
        public bool ReadBit(int BitIndex)
        {
            return ReadBit(this, BitIndex);
        }

        public override string ToString()
        {
            int Radix = 16;
            int BitCnt = BitCount;
            int FirstBit = 0;
            int LastBit = FirstBit + BitCnt - 1;
            List<int> Digits = new List<int>();
            for (int BitIndex = FirstBit; BitIndex <= LastBit; BitIndex++)
            {
                // Get the bit into carry
                int Carry = ReadBit(BitIndex) ? 1 : 0;
                // Multiply the current result by two and add carry
                int DigitIndex;
                for (DigitIndex = 0; DigitIndex < Digits.Count; DigitIndex++)
                {
                    int p = (Digits[DigitIndex] << 1) + Carry;
                    if (p >= Radix)
                    {
                        p -= Radix;
                        Carry = 1;
                    }
                    else
                        Carry = 0;
                    Digits[DigitIndex] = p;
                }
                if (Carry == 1)
                    Digits.Add(1);
            }
            string String = "";
            int MinDigits = Digits.Count;
            if (Radix == 16)
                MinDigits = (BitCnt + 7) / 8 * 2;
            for (int i = Digits.Count - 1; i >= 0; i--)
                String += Digits[i] <= 9 ? (char)('0' + Digits[i]) : (char)('A' + Digits[i] - 10);
            String = String.PadLeft(MinDigits, '0');
            return String;
        }
    }
}
