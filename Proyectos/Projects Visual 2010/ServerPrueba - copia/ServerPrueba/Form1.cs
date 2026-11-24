using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;


namespace ServerPrueba
{
    public partial class Form1 : Form
    {
        System.Windows.Forms.Timer clockTablaIP;
        MyServerUDP server = new MyServerUDP();
        List<IPDate> lista = new List<IPDate>();
        public Form1()
        {
            InitializeComponent();

            clockTablaIP = new System.Windows.Forms.Timer();
            // Set to 30 seconds.
            clockTablaIP.Interval = 30000;
            clockTablaIP.Tick += new EventHandler(clock_Tick);

            // Enable timer.
            clockTablaIP.Enabled = true;

            Thread serverThread = new Thread(server.DoWork);

            // Start the worker thread.
            serverThread.Start();
            //Console.WriteLine("main thread: Starting worker thread...");

            // Loop until worker thread activates.
            while (!serverThread.IsAlive) ;

            // Put the main thread to sleep for 1 millisecond to
            // allow the worker thread to do some work:
            Thread.Sleep(1);

            // Request that the worker thread stop itself:
            //serverThread.RequestStop();

            // Use the Join method to block the current thread 
            // until the object's thread terminates.
            //serverThread.Join();
            //Console.WriteLine("main thread: Worker thread has terminated.");
        }

        private void clock_Tick(object Sender, EventArgs e)
        {
            // Set the caption to the current time.
            server.RefrescarTablaIPs();
            lista = server.GetTablaIP();
            listBox1.Items.Clear();
            foreach (IPDate a in lista)
            {
                listBox1.Items.Add(a.IP.ToString() + " - " + a.Date.ToLongTimeString());
            }
        }
    }
}
