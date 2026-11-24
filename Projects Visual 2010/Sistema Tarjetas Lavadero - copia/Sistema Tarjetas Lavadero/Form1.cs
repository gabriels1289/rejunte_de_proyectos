using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.IO;

namespace Sistema_Tarjetas_Lavadero
{
    public partial class Form1 : Form
    {
        static SerialPort _serialPort;
        delegate void SetTextCallback(String text);
        String path = Directory.GetCurrentDirectory();
        FileStream fsSource;
        BinaryReader br;
        Byte[] tar = new Byte[8];
        byte letter = 0;
        byte aux = 0;
        byte aux2 = 0;
        int n = 0;

        public Form1()
        {
            InitializeComponent();

            LoadPortName();
            _serialPort = new SerialPort();
            // Set the read/write timeouts
            _serialPort.ReadTimeout = 5000;
            _serialPort.WriteTimeout = 5000;
            _serialPort.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);

            if (cmbBox_puertos.Items.Count > 0)
            {
                btn_conectar.Enabled = true;
                _serialPort.PortName = cmbBox_puertos.Items[0].ToString();
            }
            
        }

        #region metodos
        // Funciones Setting puerto serie
        // Display Port values and prompt user to enter a port. 
        private void LoadPortName()
        {
            foreach (string s in SerialPort.GetPortNames())
            {
                cmbBox_puertos.Items.Add(s);
            }
        }

        private void DataReceivedHandler(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                String datos = _serialPort.ReadLine();
                this.BeginInvoke(new SetTextCallback(SetText), new object[] { datos });
                if (datos.Equals("INICIO"))
                {
                    path += @"\tarjetas.txt";
                    fsSource = new FileStream(path, FileMode.Open, FileAccess.Read);
                    br = new BinaryReader(fsSource);
                    n = 0;
                    ObtenerTarjeta();
                    _serialPort.Write(tar, 0, 8);
                }
                else if (datos.Equals("FIN"))
                {
                    //_serialPort.Write(finMemoria, 0, 8);
                    br.Close();
                    fsSource.Close();
                }
                else if (datos.Equals("ACK"))
                {
                    ObtenerTarjeta();
                    _serialPort.Write(tar, 0, 8);
                }
                else if (datos.Equals("ERROR"))
                    _serialPort.Write(tar, 0, 8);
                else if (datos.StartsWith("repet."))
                {
                    ObtenerTarjeta();
                    _serialPort.Write(tar, 0, 8);
                }
            }
            catch (Exception ex) { MessageBox.Show("Error en DataReceive: " + ex.Message); }
        }

        private void SetText(String text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            this.listBox_eventos.Items.Add("Recibido: " + text);
            if (this.btn_guardar.Enabled == false)
                this.btn_guardar.Enabled = true;
        }

        private byte Asigna(byte caracter)
        {
            byte ret = caracter;
            if (ret == 0x41 || ret == 0x61)
                ret = 0x0a;
            else if (ret == 0x42 || ret == 0x62)
                ret = 0x0b;
            else if (ret == 0x43 || ret == 0x63)
                ret = 0x0c;
            else if (ret == 0x44 || ret == 0x64)
                ret = 0x0d;
            else if (ret == 0x45 || ret == 0x65)
                ret = 0x0e;
            else if (ret == 0x46 || ret == 0x66)
                ret = 0x0f;
            return ret;
        }

        private void ObtenerTarjeta()
        {
            try
            {
                if (n < fsSource.Length)
                {
                    //ID[0]
                    letter = br.ReadByte();
                    letter = Asigna(letter);
                    aux = (byte)(letter << 4);
                    letter = br.ReadByte();
                    letter = Asigna(letter);
                    aux2 = (byte)(letter << 4);
                    aux = (byte)((byte)aux + (byte)((aux2 & 0xf0) >> 4));
                    tar[0] = aux;
                    n += 2;
                    //ID[1]
                    letter = br.ReadByte();
                    letter = Asigna(letter);
                    aux = (byte)(letter << 4);
                    letter = br.ReadByte();
                    letter = Asigna(letter);
                    aux2 = (byte)(letter << 4);
                    aux = (byte)((byte)aux + (byte)((aux2 & 0xf0) >> 4));
                    tar[1] = aux;
                    n += 2;
                    //ID[2]
                    letter = br.ReadByte();
                    letter = Asigna(letter);
                    aux = (byte)(letter << 4);
                    letter = br.ReadByte();
                    letter = Asigna(letter);
                    aux2 = (byte)(letter << 4);
                    aux = (byte)((byte)aux + (byte)((aux2 & 0xf0) >> 4));
                    tar[2] = aux;
                    n += 2;
                    //ID[3]
                    letter = br.ReadByte();
                    letter = Asigna(letter);
                    aux = (byte)(letter << 4);
                    letter = br.ReadByte();
                    letter = Asigna(letter);
                    aux2 = (byte)(letter << 4);
                    aux = (byte)((byte)aux + (byte)((aux2 & 0xf0) >> 4));
                    tar[3] = aux;
                    n += 2;
                    //ID[4]
                    letter = br.ReadByte();
                    letter = Asigna(letter);
                    aux = (byte)(letter << 4);
                    letter = br.ReadByte();
                    letter = Asigna(letter);
                    aux2 = (byte)(letter << 4);
                    aux = (byte)((byte)aux + (byte)((aux2 & 0xf0) >> 4));
                    tar[4] = aux;
                    n += 2;
                    //TIPO
                    letter = br.ReadByte();
                    letter = Asigna(letter);
                    aux = (byte)(letter << 4);
                    letter = br.ReadByte();
                    letter = Asigna(letter);
                    aux2 = (byte)(letter << 4);
                    aux = (byte)((byte)aux + (byte)((aux2 & 0xf0) >> 4));
                    tar[5] = aux;
                    n += 2;
                    //DURACION
                    letter = br.ReadByte();
                    letter = Asigna(letter);
                    aux = (byte)(letter << 4);
                    letter = br.ReadByte();
                    letter = Asigna(letter);
                    aux2 = (byte)(letter << 4);
                    aux = (byte)((byte)aux + (byte)((aux2 & 0xf0) >> 4));
                    tar[6] = aux;
                    n += 2;
                    //DISPONIBLE
                    letter = br.ReadByte();
                    letter = Asigna(letter);
                    aux = (byte)(letter << 4);
                    letter = br.ReadByte();
                    letter = Asigna(letter);
                    aux2 = (byte)(letter << 4);
                    aux = (byte)((byte)aux + (byte)((aux2 & 0xf0) >> 4));
                    tar[7] = aux;
                    n += 2;
                    //CARRIAGE RETURN + LINE FEED
                    letter = br.ReadByte();
                    letter = Asigna(letter);
                    aux = (byte)(letter << 4);
                    letter = br.ReadByte();
                    letter = Asigna(letter);
                    aux2 = (byte)(letter << 4);
                    aux = (byte)((byte)aux + (byte)((aux2 & 0xf0) >> 4));
                    n += 2;
                }
                else
                {
                    tar[0] = 0x54;// T
                    tar[1] = 0x41;// A
                    tar[2] = 0x52;// R
                    tar[3] = 0x4a;// J
                    tar[4] = 0x45;// E
                    tar[5] = 0x54;// T
                    tar[6] = 0x41;// A
                    tar[7] = 0x53;// S
                }
            }
            catch (Exception ex) { MessageBox.Show("Error ObtenerTarjeta: " + ex.Message); }
        }

        #endregion metodos

        private void btn_escanear_Click(object sender, EventArgs e)
        {
            //Limpio los comboBox
            cmbBox_puertos.Items.Clear();
            //Cargo los comboBox con los nuevos valores; recargar los valores de los combobox que no sean PortName no tiene sentido
            LoadPortName();
            if (cmbBox_puertos.Items.Count > 0)
                btn_conectar.Enabled = true;
            else
                btn_conectar.Enabled = false;
        }

        private void btn_conectar_Click(object sender, EventArgs e)
        {
            if (_serialPort.IsOpen) // el puerto serie esta abierto
            {
                _serialPort.Close();    // lo cierro
                listBox_info.Items.Add("--- Port Close ---");
                btn_conectar.Text = "Conectar"; // cambio la leyenda del boton
                listBox_info.Items.Clear();
                btn_guardar.Enabled = false;
            }
            else
            {
                // seteo las propiedades del puerto
                if (cmbBox_puertos.SelectedIndex >= 0)   // me fijo si hay puertos disponibles
                    _serialPort.PortName = cmbBox_puertos.SelectedItem.ToString();
                else
                    _serialPort.PortName = cmbBox_puertos.Items[0].ToString();   // Primer puerto por defecto
                _serialPort.BaudRate = 9600;    //BaudRate por defecto
                _serialPort.DataBits = 8;
                _serialPort.Parity = Parity.None;  // Paridad por defecto
                _serialPort.StopBits = StopBits.One;    // StopBits por defecto
                _serialPort.Handshake = Handshake.None; //Handshake por defecto
                //Abro el puerto y muestro configuración en el listBox de eventos
                _serialPort.Open();
                btn_conectar.Text = "Desconectar";
                listBox_info.Items.Add("--- Port Open ---");
                listBox_info.Items.Add("--- Port Settings: ---");
                listBox_info.Items.Add("Port:        " + _serialPort.PortName.ToString());
                listBox_info.Items.Add("BaudRate:    " + _serialPort.BaudRate.ToString());
                listBox_info.Items.Add("DataBits:    " + _serialPort.DataBits.ToString());
                listBox_info.Items.Add("Parity:      " + _serialPort.Parity.ToString());
                listBox_info.Items.Add("StopBits:    " + _serialPort.StopBits.ToString());
                listBox_info.Items.Add("Handshake:   " + _serialPort.Handshake.ToString());
            }
        }

        private void cmbBox_puertos_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (cmbBox_puertos.SelectedIndex >= 0)
                btn_conectar.Enabled = true;
            else
                btn_conectar.Enabled = false;
        }

        private void btn_Borrar_Click(object sender, EventArgs e)
        {
            listBox_eventos.Items.Clear();
            listBox_info.Items.Clear();
        }

        private void btn_guardar_Click(object sender, EventArgs e)
        {
            try
            {
                string paths = Directory.GetCurrentDirectory();
                StreamWriter objWriter = new StreamWriter(paths + @"\Datos_Ejecucion.txt");
                for (int i = 0; i < listBox_info.Items.Count; i++)
                {
                    objWriter.WriteLine(listBox_info.Items[i].ToString());
                }
                for (int i = 0; i < listBox_eventos.Items.Count; i++)
                {
                    objWriter.WriteLine(listBox_eventos.Items[i].ToString());
                }
                objWriter.Close();
            }
            catch (Exception ex) { MessageBox.Show("Error en guardar: " + ex.Message);}
        }
    }
}
