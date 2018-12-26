using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Text;
using System.IO;

namespace Elatec
{
    public class FileLogger : LogDest
    {
        string FileName;
        public FileLogger(string FileName)
        {
            this.NewLine = "\r\n";
            this.FileName = FileName;
        }
        public override void Write(string Text)
        {
            try
            {
                StreamWriter w = File.AppendText(FileName);
                w.Write(Text);
                w.Close();
            }
            catch
            {
            }
        }
    }
}

