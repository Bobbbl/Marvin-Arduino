using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Elatec;

namespace OSDPServer_v02
{


    public partial class Form1 : Form
    {
        private PortScanner pscanner;
        private string[] PortList;
        OSDPLogger logger;
        Elatec.IO.Ports.SerialPort SerialPort;

    /***************************************************************************************
     *                                    Konstruktor                                      *
     ***************************************************************************************/

        public Form1()
        {
            InitializeComponent();

            // Port Scanner Setup
            this.pscanner = new PortScanner();
            this.pscanner.Interval = 500;
            this.pscanner.NewPortListEvent += Pscanner_NewPortListEvent;
            this.pscanner.Start();
            this.FormClosed += Form1_FormClosed;

            // ComboBoxPorts Setup
            this.comboBoxPorts.MouseEnter += ComboBoxPorts_Enter;

            // Logger Setup
            this.logger = null; 

        }

        private void ComboBoxPorts_Enter(object sender, EventArgs e)
        {
            this.comboBoxPorts.Items.Clear();

            for (int i = 0; i < this.PortList.Length; i++)
            {
                if(this.PortList[i] != null)
                {
                    this.comboBoxPorts.Items.Add(this.PortList[i]);
                }
            }
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            this.pscanner.Abort();
            this.logger.Abort();
        }


        private void Pscanner_NewPortListEvent()
        {
            this.PortList = new string[this.pscanner._ports.Count];

            for (int i = 0; i < this.pscanner._ports.Count; i++)
            {
                this.pscanner._ports.TryDequeue( out this.PortList[i] );
            }
        }

        private void buttonConnect_Click(object sender, EventArgs e)
        {
            try
            {
                string pname = this.comboBoxPorts.GetItemText(this.comboBoxPorts.SelectedItem);
                string bratestring = this.comboBoxBaudRate.GetItemText(this.comboBoxBaudRate.SelectedItem);
                int brate = Convert.ToInt32(bratestring);

                this.logger = new OSDPLogger(textBoxLogger, pname, brate, Elatec.IO.Ports.Parity.None, 8, Elatec.IO.Ports.StopBits.One);

                this.logger.Start();
                
                
            }
            catch (Exception ex)
            {
                this.textBoxStatus.AppendText(ex.Message);

            }
        }

        private void comboBoxPorts_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void comboBoxBaudRate_SelectedIndexChanged(object sender, EventArgs e)
        {

        }
    }
}
