using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.Threading;
using System.Diagnostics;
using System.Windows.Forms.DataVisualization.Charting;
using System.IO;

namespace FFTvisor
{
    public partial class Form1 : Form
    {
        static SerialPort _serialPort;
        delegate void SetTextCallback(Double text);
        public Form1()
        {
            InitializeComponent();

            LoadPortName();
            LoadPortBaudRate();
            LoadPortDataBits();
            LoadPortParity();
            LoadPortStopBits();
            LoadPortHandshake();
            listBox_eventos.Items.Add("--- Default Settings: ---");
            listBox_eventos.Items.Add("BaudRate: 9600");
            listBox_eventos.Items.Add("DataBits: 8");
            listBox_eventos.Items.Add("Parity: None");
            listBox_eventos.Items.Add("StopBits: 1");
            listBox_eventos.Items.Add("Handshake: None");
            listBox_eventos.Items.Add("");

            // Create a new SerialPort object with default settings.
            _serialPort = new SerialPort();
            _serialPort.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);

            if (cmbBox_puerto.Items.Count > 0)
            {
                btn_conectar.Enabled = true;
                _serialPort.PortName = cmbBox_puerto.Items[0].ToString();
            }

            // Set the read/write timeouts
            _serialPort.ReadTimeout = 500;
            _serialPort.WriteTimeout = 500;

            chart_fft.Series["FFT"].Color = Color.Blue;

            // Set automatic zooming
            chart_fft.ChartAreas[0].AxisX.ScaleView.Zoomable = true;
            chart_fft.ChartAreas[0].AxisY.ScaleView.Zoomable = true;

            // Set automatic scrolling 
            chart_fft.ChartAreas[0].CursorX.AutoScroll = true;
            chart_fft.ChartAreas[0].CursorY.AutoScroll = true;

            // Allow user selection for Zoom
            chart_fft.ChartAreas[0].CursorX.IsUserSelectionEnabled = true;
            chart_fft.ChartAreas[0].CursorY.IsUserSelectionEnabled = true;
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void DataReceivedHandler(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                System.Threading.Thread.Sleep(2000);
                int n = _serialPort.BytesToRead;
                if (n > 0 && (n % 4 == 0))
                {
                    Byte[] buffer = new Byte[n];
                    //Single[] datos = new Single[n / 4];
                    _serialPort.Read(buffer, 0, n);
                    for (int i = 0, j = 0; i < n; i += 4, j++)
                    {
                        //datos[j] = BitConverter.ToSingle(buffer, i);
                        Single datos = BitConverter.ToSingle(buffer, i);
                        Double aux = Convert.ToDouble(datos);
                        this.BeginInvoke(new SetTextCallback(SetText), new object[] { aux });
                    }
                    //foreach (Single a in datos)
                    //{
                    //    Double aux = Convert.ToDouble(a);
                    //    this.BeginInvoke(new SetTextCallback(SetText), new object[] { aux });
                    //}
                }
            }
            catch (Exception ex) { MessageBox.Show("Error en DataReceive: " + ex.Message); }
        }

        private void SetText(Double text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            this.chart_fft.Series["FFT"].Points.AddY(text);
            this.listBox_datos.Items.Add("Dato número " + this.listBox_datos.Items.Count + ": " + text);
            if (this.btn_save.Enabled == false)
                this.btn_save.Enabled = true;
        }

        #region LoadPortSettings
        // Funciones Setting puerto serie
        // Display Port values and prompt user to enter a port. 
        private void LoadPortName()
        {
            foreach (string s in SerialPort.GetPortNames())
            {
                cmbBox_puerto.Items.Add(s);
            }

        }
        // Display BaudRate values and prompt user to enter a value. 
        private void LoadPortBaudRate()
        {
            cmbBox_baudRate.Items.Add("600");
            cmbBox_baudRate.Items.Add("1200");
            cmbBox_baudRate.Items.Add("2400");
            cmbBox_baudRate.Items.Add("4800");
            cmbBox_baudRate.Items.Add("9600");
            cmbBox_baudRate.Items.Add("14400");
            cmbBox_baudRate.Items.Add("19200");
            cmbBox_baudRate.Items.Add("28800");
            cmbBox_baudRate.Items.Add("38400");
            cmbBox_baudRate.Items.Add("56000");
            cmbBox_baudRate.Items.Add("57600");
            cmbBox_baudRate.Items.Add("115200");
            cmbBox_baudRate.Items.Add("128000");
            cmbBox_baudRate.Items.Add("256000");
        }

        // Display PortParity values and prompt user to enter a value. 
        private void LoadPortParity()
        {
            foreach (string s in Enum.GetNames(typeof(Parity)))
            {
                cmbBox_parity.Items.Add(s);
            }
        }

        // Display DataBits values and prompt user to enter a value. 
        private void LoadPortDataBits()
        {
            cmbBox_dataBits.Items.Add("5");
            cmbBox_dataBits.Items.Add("6");
            cmbBox_dataBits.Items.Add("7");
            cmbBox_dataBits.Items.Add("8");
        }

        // Display StopBits values and prompt user to enter a value. 
        private void LoadPortStopBits()
        {
            foreach (string s in Enum.GetNames(typeof(StopBits)))
            {
                cmbBox_stopBits.Items.Add(s);
            }

            //Console.Write("Enter StopBits value (None is not supported and \n" +
            // "raises an ArgumentOutOfRangeException. \n (Default: {0}):", defaultPortStopBits.ToString());
        }

        private void LoadPortHandshake()
        {
            foreach (string s in Enum.GetNames(typeof(Handshake)))
            {
                cmbBox_handshake.Items.Add(s);
            }
        }
        #endregion LoadPortSettings

        private void btn_escanear_Click(object sender, EventArgs e)
        {
            //Limpio los comboBox
            cmbBox_puerto.Items.Clear();
            //Cargo los comboBox con los nuevos valores; recargar los valores de los combobox que no sean PortName no tiene sentido
            LoadPortName();
            if (cmbBox_puerto.Items.Count > 0)
                btn_conectar.Enabled = true;
            else
                btn_conectar.Enabled = false;
        }

        private void btn_conectar_Click(object sender, EventArgs e)
        {
            if (_serialPort.IsOpen) // el puerto serie esta abierto
            {
                _serialPort.Close();    // lo cierro
                listBox_eventos.Items.Add("--- Port Close ---");
                listBox_eventos.Items.Add("");
                btn_conectar.Text = "Conectar"; // cambio la leyenda del boton
            }
            else
            {
                // seteo las propiedades del puerto
                if (cmbBox_puerto.SelectedIndex >= 0)   // me fijo si hay puertos disponibles
                    _serialPort.PortName = cmbBox_puerto.SelectedItem.ToString();
                else
                    _serialPort.PortName = cmbBox_puerto.Items[0].ToString();   // Primer puerto por defecto
                if (cmbBox_baudRate.SelectedIndex >= 0)  // seteo BaudRate
                    _serialPort.BaudRate = int.Parse(cmbBox_baudRate.SelectedItem.ToString());
                else
                    _serialPort.BaudRate = 9600;    //BaudRate por defecto
                if (cmbBox_dataBits.SelectedIndex >= 0)  // seteo DataBits
                    _serialPort.DataBits = int.Parse(cmbBox_dataBits.SelectedItem.ToString());
                else
                    _serialPort.DataBits = 8;
                if (cmbBox_parity.SelectedIndex >= 0)   // seteo paridad
                    _serialPort.Parity = (Parity)Enum.Parse(typeof(Parity), cmbBox_parity.SelectedItem.ToString(), true);
                else
                    _serialPort.Parity = Parity.None;  // Paridad por defecto
                if (cmbBox_stopBits.SelectedIndex >= 0)    // seteo StopBits
                    _serialPort.StopBits = (StopBits)Enum.Parse(typeof(StopBits), cmbBox_stopBits.SelectedItem.ToString(), true);
                else
                    _serialPort.StopBits = StopBits.One;    // StopBits por defecto
                if (cmbBox_handshake.SelectedIndex >= 0)    // seteo Handshake
                    _serialPort.Handshake = (Handshake)Enum.Parse(typeof(Handshake), cmbBox_handshake.SelectedItem.ToString(), true);
                else
                    _serialPort.Handshake = Handshake.None; //Handshake por defecto
                //Abro el puerto y muestro configuración en el listBox de eventos
                _serialPort.Open();

                btn_conectar.Text = "Desconectar";
                listBox_eventos.Items.Add("--- Port Open ---");
                listBox_eventos.Items.Add("--- Port Settings: ---");
                listBox_eventos.Items.Add("Port:        " + _serialPort.PortName.ToString());
                listBox_eventos.Items.Add("BaudRate:    " + _serialPort.BaudRate.ToString());
                listBox_eventos.Items.Add("DataBits:    " + _serialPort.DataBits.ToString());
                listBox_eventos.Items.Add("Parity:      " + _serialPort.Parity.ToString());
                listBox_eventos.Items.Add("StopBits:    " + _serialPort.StopBits.ToString());
                listBox_eventos.Items.Add("Handshake:   " + _serialPort.Handshake.ToString());
                listBox_eventos.Items.Add("");
            }
        }

        private void cmbBox_puerto_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (cmbBox_puerto.SelectedIndex >= 0)
                btn_conectar.Enabled = true;
            else
                btn_conectar.Enabled = false;
        }

        private void btn_borrar_Click(object sender, EventArgs e)
        {
            listBox_datos.Items.Clear();
            chart_fft.Series["FFT"].Points.Clear();
            btn_save.Enabled = false;
        }

        private void btn_save_Click(object sender, EventArgs e)
        {
            string path = Directory.GetCurrentDirectory();
            StreamWriter objWriter = new StreamWriter(path + @"\FFT.csv");
            for (int i = 0; i < listBox_datos.Items.Count; i++)
            {
                String aux = listBox_datos.Items[i].ToString();
                int foundS1 = aux.IndexOf(":");
                objWriter.WriteLine(aux.Remove(0, foundS1+2));
            }
            objWriter.Close();
            //StreamReader objReader = new StreamReader(path + @"\fft.txt");
            this.chart_fft.SaveImage(path + @"\chart.jpg", ChartImageFormat.Jpeg);
            this.chart_fft.SaveImage(path + @"\chart.png", ChartImageFormat.Png);
            this.chart_fft.SaveImage(path + @"\chart.bmp", ChartImageFormat.Bmp);
        }
    }
}
