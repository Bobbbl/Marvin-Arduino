using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Text;
using System.IO;

namespace Elatec
{
    public class ConsoleLogger : LogDest
    {
        public ConsoleLogger()
        {
            this.NewLine = "\r\n";
        }
        public override void Write(string Text)
        {
            Console.Write(Text);
        }
    }
}
