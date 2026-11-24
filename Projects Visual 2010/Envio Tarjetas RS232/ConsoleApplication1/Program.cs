using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.IO.Ports;
using System.Threading;

namespace ConsoleApplication1
{
    class Program
    {

        static SerialPort _serialPort;
        static bool _continue;
        Thread readThread = new Thread(Read);

        static Byte[] tarjetas = {   0x69,0x00,0x87,0x34,0xF5,0x00,0x00,0x00,
                                    0x69,0x00,0x87,0x43,0xD4,0x01,0x01,0x00,
                                    0x69,0x00,0x87,0x29,0xE3,0x01,0x01,0x00,
                                    0x69,0x00,0x87,0x64,0x8F,0x01,0x01,0x00,
                                    0x69,0x00,0x87,0x8A,0x09,0x02,0x02,0x00,
                                    0x69,0x00,0x87,0x54,0x9E,0x02,0x02,0x00,
                                    0x69,0x00,0x87,0x3A,0x36,0x03,0x03,0x00,
                                    0x69,0x00,0x87,0x46,0x82,0x01,0x01,0x00,
                                    0x69,0x00,0x87,0x41,0x77,0x01,0x01,0x00,
                                    0x69,0x00,0x87,0x2F,0xD0,0x03,0x03,0x00,
                                    0x69,0x00,0x87,0x7E,0xA6,0x02,0x02,0x00,
                                    0x69,0x00,0x87,0x78,0xEC,0x01,0x01,0x00,
                                    0x69,0x00,0x87,0x59,0x06,0x02,0x02,0x00,
                                    0x69,0x00,0x87,0x50,0xCE,0x03,0x03,0x00,
                                    0x69,0x00,0x87,0x86,0x6E,0x01,0x01,0x00,
                                    0x69,0x00,0x87,0x3B,0xBC,0x02,0x02,0x00};

        static void Main(string[] args)
        {
            StringComparer stringComparer = StringComparer.OrdinalIgnoreCase;
            //Thread readThread = new Thread(Read);
            // Create a new SerialPort object with default settings.
            _serialPort = new SerialPort();

            // Allow the user to set the appropriate properties.
            _serialPort.PortName = SetPortName(_serialPort.PortName);
            _serialPort.BaudRate = 9600; //SetPortBaudRate(_serialPort.BaudRate);
            _serialPort.Parity = Parity.None; //SetPortParity(_serialPort.Parity);
            _serialPort.DataBits = 8; //SetPortDataBits(_serialPort.DataBits);
            _serialPort.StopBits = StopBits.One; //SetPortStopBits(_serialPort.StopBits);
            _serialPort.Handshake = Handshake.None; //SetPortHandshake(_serialPort.Handshake);

            // Set the read/write timeouts
            _serialPort.ReadTimeout = 500;
            _serialPort.WriteTimeout = 500;

            _serialPort.Open();
            //_continue = true;
            //readThread.Start();
            // Get the current directory.
            //string path = Directory.GetCurrentDirectory();
            //StreamReader objReader = new StreamReader(path+@"\fft.txt");

            //string sLine = "";
            //List<float> buffer = new List<float>();

            //while (sLine != null)
            //{
            //    sLine = objReader.ReadLine();
            //    if (sLine != null)
            //    {
            //        float aux = float.Parse(sLine);
            //        buffer.Add(aux);
            //    }
            //}
            //objReader.Close();
            Console.WriteLine("PUERTO ABIERTO COMIENZO EL ENVIO:" + System.Environment.NewLine);
            for (int i = 0; i < 96; i += 8)
            {
                Console.WriteLine("Envio Tarjeta {0}", i);
                _serialPort.Write(tarjetas, i, 8);
                Console.ReadLine();
            }
            //readThread.Join();
            _serialPort.Close();
            Console.WriteLine("FIN DEL ENVIO");
            Console.ReadLine();
        }

        public static void Read()
        {
            while (_continue)
            {
                try
                {
                    string message = _serialPort.ReadLine();
                    Console.WriteLine(message);
                }
                catch (TimeoutException) { }
            }
        }

        #region settings
        // Display Port values and prompt user to enter a port. 
        public static string SetPortName(string defaultPortName)
        {
            string portName;

            Console.WriteLine("Available Ports:");
            foreach (string s in SerialPort.GetPortNames())
            {
                Console.WriteLine("   {0}", s);
            }

            Console.Write("Enter COM port value (Default: {0}): ", defaultPortName);
            portName = Console.ReadLine();

            if (portName == "" || !(portName.ToLower()).StartsWith("com"))
            {
                portName = defaultPortName;
            }
            return portName;
        }
        // Display BaudRate values and prompt user to enter a value. 
        public static int SetPortBaudRate(int defaultPortBaudRate)
        {
            string baudRate;

            Console.Write("Baud Rate(default:{0}): ", defaultPortBaudRate);
            baudRate = Console.ReadLine();

            if (baudRate == "")
            {
                baudRate = defaultPortBaudRate.ToString();
            }

            return int.Parse(baudRate);
        }

        // Display PortParity values and prompt user to enter a value. 
        public static Parity SetPortParity(Parity defaultPortParity)
        {
            string parity;

            Console.WriteLine("Available Parity options:");
            foreach (string s in Enum.GetNames(typeof(Parity)))
            {
                Console.WriteLine("   {0}", s);
            }

            Console.Write("Enter Parity value (Default: {0}):", defaultPortParity.ToString(), true);
            parity = Console.ReadLine();

            if (parity == "")
            {
                parity = defaultPortParity.ToString();
            }

            return (Parity)Enum.Parse(typeof(Parity), parity, true);
        }
        // Display DataBits values and prompt user to enter a value. 
        public static int SetPortDataBits(int defaultPortDataBits)
        {
            string dataBits;

            Console.Write("Enter DataBits value (Default: {0}): ", defaultPortDataBits);
            dataBits = Console.ReadLine();

            if (dataBits == "")
            {
                dataBits = defaultPortDataBits.ToString();
            }

            return int.Parse(dataBits.ToUpperInvariant());
        }

        // Display StopBits values and prompt user to enter a value. 
        public static StopBits SetPortStopBits(StopBits defaultPortStopBits)
        {
            string stopBits;

            Console.WriteLine("Available StopBits options:");
            foreach (string s in Enum.GetNames(typeof(StopBits)))
            {
                Console.WriteLine("   {0}", s);
            }

            Console.Write("Enter StopBits value (None is not supported and \n" +
             "raises an ArgumentOutOfRangeException. \n (Default: {0}):", defaultPortStopBits.ToString());
            stopBits = Console.ReadLine();

            if (stopBits == "")
            {
                stopBits = defaultPortStopBits.ToString();
            }

            return (StopBits)Enum.Parse(typeof(StopBits), stopBits, true);
        }
        public static Handshake SetPortHandshake(Handshake defaultPortHandshake)
        {
            string handshake;

            Console.WriteLine("Available Handshake options:");
            foreach (string s in Enum.GetNames(typeof(Handshake)))
            {
                Console.WriteLine("   {0}", s);
            }

            Console.Write("End Handshake value (Default: {0}):", defaultPortHandshake.ToString());
            handshake = Console.ReadLine();

            if (handshake == "")
            {
                handshake = defaultPortHandshake.ToString();
            }

            return (Handshake)Enum.Parse(typeof(Handshake), handshake, true);
        }
        #endregion settings
    }
}
