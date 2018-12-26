using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Elatec;
using System.Threading;
using System.Windows.Forms;
using System.Collections.Concurrent;
using Elatec;


namespace OSDPServer_v02
{

    enum RXStates { WAIT_SOM, READ_ADDRESS, READ_LEN_LSB, READ_LEN_MSB, READ_CTRL, READ_DATA, READ_CKSUM, READ_CRC_LSB, READ_CRC_MSB };

    public class Message
    {
        public int Adresse;
        public int Control;
        public byte[] RawData;
        public int length;

        public Message()
        {
            Adresse = 0;
            Control = 0;
            RawData = null;

        }

    }

    public class OSDPLogger
    {
        public event NewOSDPMessage NewOSDPMessageEvent;
        Thread thread;
        GUILogger glogger;
        public ConcurrentQueue<string> _messages = new ConcurrentQueue<string>();
        public ConcurrentQueue<Message> messageclasses = new ConcurrentQueue<Message>();
        protected Elatec.IO.Ports.SerialPort serialport;

        string Name;
        int Brate;
        Elatec.IO.Ports.Parity Parity;
        int DataBits;
        Elatec.IO.Ports.StopBits StopBits;



        /***************************************************************************************
         *                                    Konstruktor                                      *
         ***************************************************************************************/

        public OSDPLogger(TextBox tbox, string name, int brate ,Elatec.IO.Ports.Parity parity, int DataBits, Elatec.IO.Ports.StopBits StopBits)
        {

            this.thread = new Thread(this.Run);
            glogger = new GUILogger(tbox, this);

            this.Brate = brate;
            this.Name = name;
            this.Parity = parity;
            this.DataBits = DataBits;
            this.StopBits = StopBits;

            this.serialport = new Elatec.IO.Ports.SerialPort(this.Name, this.Brate, this.Parity, this.DataBits, this.StopBits);
            this.serialport.Open();


        }

        public void Start()
        {
            this.thread.Start();
        }

        public void Abort()
        {
            this.thread.Abort();
        }

        protected virtual void Run()
        {
            RXStates state = RXStates.WAIT_SOM; 

            while (true)
            {

                while(this.serialport.BytesToRead > 0)
                {
                    int Byte = this.serialport.ReadByte();
                    this._messages.Enqueue(Byte.ToString("X2"));
                    Message m = new Message();
                    int pos = 0;

                    switch (state)
                    {
                        case RXStates.WAIT_SOM:
                            if ("Start Of Sequence" != OSDP_Command_Library.Replies[Byte])
                            {
                                break;
                            }
                            state = RXStates.READ_ADDRESS;
                            break;
                        case RXStates.READ_ADDRESS:
                            m.Adresse = Byte;
                            state = RXStates.READ_LEN_LSB;
                            break;
                        case RXStates.READ_LEN_LSB:
                            m.length = Byte;
                            state = RXStates.READ_LEN_MSB;
                            break;
                        case RXStates.READ_LEN_MSB:
                            m.length += Byte << 8;
                            if(m.length < 6)
                            {
                                state = RXStates.WAIT_SOM;
                                break;
                            }
                            state = RXStates.READ_CTRL;
                            break;
                        case RXStates.READ_CTRL:
                            m.Control = Byte;
                            int RawDataLength = m.length - ((m.Control & 0x04) == 0x00 ? 6 : 7);
                            if(RawDataLength < 0)
                            {
                                state = RXStates.WAIT_SOM;
                                break;
                            }
                            m.RawData = new byte[RawDataLength];
                            pos = 0;
                            if(pos == m.RawData.Length)
                            {
                                state = (m.Control & 0x04) != 0x00 ? RXStates.READ_CRC_LSB : RXStates.READ_CKSUM;
                            }
                            else
                            {
                                state = RXStates.READ_DATA;
                            }
                            break;

                        case RXStates.READ_DATA:
                            m.RawData[pos++] = (byte)Byte;
                            if(pos == m.RawData.Length)
                            {
                                state = (m.Control & 0x04) != 0x00 ? RXStates.READ_CRC_LSB : RXStates.READ_CKSUM;
                            }
                            break;
                        case RXStates.READ_CKSUM:
                            state = RXStates.WAIT_SOM;
                            this.messageclasses.Enqueue(m);
                            break;
                        case RXStates.READ_CRC_LSB:
                            state = RXStates.READ_CRC_MSB;
                            break;
                        case RXStates.READ_CRC_MSB:
                            this.messageclasses.Enqueue(m);
                            break;
                    }
                    
                    this._messages.Enqueue(string.Format("{0}", state));
                }

                this.NewOSDPMessageEvent();
                Thread.Sleep(100);
            }
        }
    }
}
