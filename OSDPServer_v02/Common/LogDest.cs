using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Text;
using System.IO;

namespace Elatec
{
    public abstract class LogDest
    {
        public string NewLine;
        public abstract void Write(string Text);
    }
}
