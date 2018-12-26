using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Elatec;
using System.Windows.Forms;
using System.Collections.Concurrent;
using System.Threading;

namespace OSDPServer_v02
{
    public delegate void NewOSDPMessage();

    class GUILogger : LogDest
    {
        TextBox logbox;
        List<string> messages;
        int messagecount = 0;
        OSDPLogger ologger;
        System.Windows.Forms.Timer timer;
        List<Message> messageclasses;

        public  GUILogger(TextBox logbox, OSDPLogger ologger)
        {
            messageclasses = new List<Message>();
            this.logbox = logbox;
            this.ologger = ologger;
            this.ologger.NewOSDPMessageEvent += Ologger_NewOSDPMessageEvent;
            this.messages = new List<string>();

            this.timer = new System.Windows.Forms.Timer();
            this.timer.Interval = 50;
            this.timer.Tick += Timer_Tick;
            this.timer.Start();
        }

        private void Timer_Tick(object sender, EventArgs e)
        {

            for (; this.messagecount < this.messages.Count; this.messagecount++)
            {

                this.Write(this.messages[this.messagecount]);
            }
        }

        private void Ologger_NewOSDPMessageEvent()
        {
            string tmp;
            this.ologger._messages.TryDequeue(out tmp);
            if(tmp != null)
            {
                this.messages.Add(tmp);
            }

            for (int i = 0; i < this.ologger._messages.Count; i++)
            {
                Message m;
                this.ologger.messageclasses.TryDequeue(out m);
                this.messageclasses.Add(m);

            }

            

        }

        public override void Write(string Text)
        {
            this.logbox.AppendText(Text + "\r\n");
        }


        ~GUILogger()
        {

        }
    }
}
