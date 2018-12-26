using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Text;
using System.IO;

namespace Elatec
{
    public class Logger : TextWriter
    {
        List<Logger> Loggers;
        List<LogDest> LogDests;
        Encoding _Encoding;
        //string NewLine;
        public Logger()
        {
            this.Loggers = new List<Logger>();
            this.LogDests = new List<LogDest>();
            this._Encoding = new ASCIIEncoding();
            this.NewLine = "\r\n";
        }
        public void AddLogger(Logger Log)
        {
            Loggers.Add(Log);
        }
        public void AddLogger(LogDest Log)
        {
            LogDests.Add(Log);
        }

        // **********************************************************
        // ****** Standard Write Functions **************************
        // **********************************************************

        public override void Write(string Text)
        {
            foreach (Logger Log in Loggers)
                Log.Write(Text);
            foreach (LogDest Log in LogDests)
                // Replace 'NewLine' from 'Logger' by individual 'NewLine' of 'LogDest'
                Log.Write(Text.Replace(NewLine, Log.NewLine));
        }

        public override Encoding Encoding
        {
            get { return _Encoding; }
        }

        /*
        public override void Write(string Text, object Arg0)
        {
            Write(string.Format(Text, Arg0));
        }
        public void Write(string Text, object Arg0, object Arg1)
        {
            Write(string.Format(Text, Arg0, Arg1));
        }
        public void Write(string Text, object Arg0, object Arg1, object Arg2)
        {
            Write(string.Format(Text, Arg0, Arg1, Arg2));
        }
        public void Write(string Text, object Arg0, object Arg1, object Arg2, object Arg3)
        {
            Write(string.Format(Text, Arg0, Arg1, Arg2, Arg3));
        }
        */
        public override void WriteLine(string Text)
        {
            Write(Text + NewLine);
        }
        /*
        public void WriteLine(string Text, object Arg0)
        {
            WriteLine(string.Format(Text, Arg0));
        }
        public void WriteLine(string Text, object Arg0, object Arg1)
        {
            WriteLine(string.Format(Text, Arg0, Arg1));
        }
        public void WriteLine(string Text, object Arg0, object Arg1, object Arg2)
        {
            WriteLine(string.Format(Text, Arg0, Arg1, Arg2));
        }
        public void WriteLine(string Text, object Arg0, object Arg1, object Arg2, object Arg3)
        {
            WriteLine(string.Format(Text, Arg0, Arg1, Arg2, Arg3));
        }
        */
        public override void WriteLine()
        {
            Write(NewLine);
        }
        

        // **********************************************************
        // ****** Special Write Functions ***************************
        // **********************************************************

        public void WriteLines(List<string> Lines)
        {
            foreach (string s in Lines)
                WriteLine(s);
        }

        public void WriteLineBreak(string Line, int Width)
        {
            if (Width <= 0)
                Width = 80;
            while (Line.Length > Width)
            {
                int Pos;
                for (Pos = Width - 1; Pos >= 0; Pos--)
                    if (Line[Pos] == ' ')
                        break;
                if (Pos < 0)
                    Pos = Width;
                WriteLine(Line.Substring(0, Pos));
                Line = Line.Substring(Pos).TrimStart();
            }
            WriteLine(Line);
        }
        public void HexLine(byte[] Data, int Offset, int Count)
        {
            Write(Count.ToString() + ":");
            for (int i = 0; i < Count; i++)
                Write(" " + Data[Offset + i].ToString("X2"));
            WriteLine();
        }
        public void HexLine(int[] Data, int Offset, int Count)
        {
            Write(Count.ToString() + ":");
            for (int i = 0; i < Count; i++)
                Write(" " + Data[Offset + i].ToString("X8"));
            WriteLine();
        }
        public void HexLine(byte[] Data)
        {
            HexLine(Data, 0, Data.Length);
        }
        public void HexLine(int[] Data)
        {
            HexLine(Data, 0, Data.Length);
        }
        public void HexDump(byte[] Data, int ValuesPerLine)
        {
            int Remaining = Data.Length;
            int Offset = 0;
            while (Remaining > 0)
            {
                int ValuesThisLine = Remaining > ValuesPerLine ? ValuesPerLine : Remaining;
                HexLine(Data, Offset, ValuesThisLine);
                Remaining -= ValuesThisLine;
                Offset += ValuesThisLine;
            }
        }
    }
}
