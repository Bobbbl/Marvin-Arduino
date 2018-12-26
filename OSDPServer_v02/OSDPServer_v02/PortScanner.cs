using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Collections.Concurrent;
using System.IO.Ports;

namespace OSDPServer_v02
{

    public delegate void NewPortListHandle();

    public class PortScanner
    {
        public event NewPortListHandle NewPortListEvent;

        Thread thread;
        protected int _interval;
        public ConcurrentQueue<string> _ports;


        public int Interval
        {
            get { return _interval; }
            set { this._interval = value; }
        }

        public PortScanner()
        {
            this.thread = new Thread(this.Run);
            this.Interval = 1000;
            this._ports = new ConcurrentQueue<string>();

        }

        public void Abort()
        {
            this.thread.Abort();
        }
        
        public virtual void Start()
        {
            this.thread.Start();
        }

        public virtual void Start(int Interval)
        {
            this.Interval = Interval;
            this.thread.Start();
        }

        protected virtual void Run()
        {
            while (true)
            {
                
                string[] ports = SerialPort.GetPortNames();

                foreach (string p in ports)
                {
                    this._ports.Enqueue(p);
                }
                               
                if(this._ports.Count != 0)
                {
                    NewPortListEvent();
                }
                Thread.Sleep(this._interval);
            }
        }

    }
}
