using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ServerProgresivos
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            Thread thdUDPServer = new Thread(new ThreadStart(serverThread));
            thdUDPServer.Start();
        }
        //variables globales para los mensajes en los listsbox
        String textoIPs = "";
        String textoMsg = "";
        Boolean _encriptacionActivada = false;
        UdpClient udpClient;


        // Borra el contenido del TextBox de mensajes
        private void button_Clear_BoxMsg_Click(object sender, EventArgs e)
        {
            listBox_Msg.Items.Clear();
        }

        // Borra el contenido del TextBox de IPs
        private void button_Clear_BoxIPs_Click(object sender, EventArgs e)
        {
            listBox_IPs.Items.Clear();
        }

        delegate void SetTextIPCallback(string text);

        private void SetTextIP(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.listBox_IPs.InvokeRequired)
            {
                SetTextIPCallback d = new SetTextIPCallback(SetTextIP);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.listBox_IPs.Items.Add(text);
            }
        }

        delegate void SetTextMsgCallback(string text);

        private void SetTextMsg(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.listBox_Msg.InvokeRequired)
            {
                SetTextMsgCallback d = new SetTextMsgCallback(SetTextMsg);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.listBox_Msg.Items.Add(text);
            }
        }

        public void serverThread()
        {
            udpClient = new UdpClient(1200);
            while (true)
            {
                IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);
                Byte[] receiveBytes = udpClient.Receive(ref RemoteIpEndPoint);
                string returnData = Encoding.ASCII.GetString(receiveBytes);
                ProcesarEco(receiveBytes, RemoteIpEndPoint);
                //this.SetText(RemoteIpEndPoint.Address.ToString());
                this.SetTextIP(textoIPs);
                this.SetTextMsg(textoMsg);
            }
        }

        public void ProcesarEco(Byte[] msg, IPEndPoint ipEndpoint)
        {
            textoIPs = "";

            if ((_encriptacionActivada) & (msg.Length == 49)) msg = Utiles.encrypt(msg);

            // verifico la longitud correcta del Hello (49 bytes) y su encabezado
            if (cabeceraValida(msg) & msg.Length == 49)
            {
                DateTime FechaTransmision = DateTime.Now;
                UInt16 uid = 0;
                String versionUDP = "";
                String vcc = "";
                int indice = 0;
                Int32 jumper = msg[32] & 0x03;  // cont_hello = msg[32] >> 2;
                while (Convert.ToInt16(msg[indice + 6]) > 0)
                {
                    versionUDP = versionUDP + Convert.ToChar(msg[indice + 6]);
                    indice++;
                }
                if (versionUDP.Equals("INIT"))
                {
                    textoIPs = ipEndpoint.Address.ToString() + " - " + versionUDP;
                }
                Decimal version = 0;
                try
                {
                    version = Convert.ToDecimal(versionUDP.Substring(0, versionUDP.IndexOf('.')));
                }
                catch (Exception ex)
                {
                    version = 0;
                }
                if (version >= 1)
                {
                    indice = 0;
                    try
                    {
                        while (Convert.ToInt16(msg[indice + 26]) > 0)
                        {
                            vcc = vcc + Convert.ToChar(msg[indice + 26]);
                            indice++;
                        }
                    }
                    catch (Exception ex)
                    {
                        vcc = "0";
                        textoMsg = ipEndpoint.Address.ToString() + " - Error..." + " - Version Mensaje:" + versionUDP + ex;
                    }

                    vcc = vcc.Replace('.', '.');
                    Decimal tension = Convert.ToDecimal(vcc);
                    String tok = "3.15";
                    tok = tok.Replace('.', '.');
                    Decimal tensionOK = (Convert.ToDecimal(tok));
                    if (tension < tensionOK)
                    {
                        textoMsg = ipEndpoint.Address.ToString() + " - tension baja (" + vcc + " V)";
                    }
                    if (tension >= tensionOK)
                    {
                        textoMsg = ipEndpoint.Address.ToString() + " - tension OK (" + vcc + " V)";
                    }

                    try
                    {
                        //uid = Encoding.ASCII.GetString(msg, 33, 2);
                        uid = BitConverter.ToUInt16(msg, 33);
                    }
                    catch (Exception ex)
                    {
                        uid = 0;
                    }

                    DateTime fechaActual = DateTime.Now;

                    int annio = fechaActual.Year;
                    int anioLSB = annio % 10; annio = annio / 10;
                    int anioLSB2 = annio % 10; annio = annio / 10;
                    int anioMSB2 = annio % 10; annio = annio / 10;
                    int anioMSB = annio % 10;

                    int mes = fechaActual.Month;
                    int mesLSB = mes % 10; mes = mes / 10;
                    int mesMSB = mes % 10;

                    int dia = fechaActual.Day;
                    int diaLSB = dia % 10; dia = dia / 10;
                    int diaMSB = dia % 10;

                    int hora = fechaActual.Hour;
                    int horaLSB = hora % 10; hora = hora / 10;
                    int horaMSB = hora % 10;

                    int minutos = fechaActual.Minute;
                    int minutosLSB = minutos % 10; minutos = minutos / 10;
                    int minutosMSB = minutos % 10;

                    int segundos = fechaActual.Second;
                    int segundosLSB = segundos % 10; segundos = segundos / 10;
                    int segundosMSB = segundos % 10;
                    //-------------------------------------------------------------------------------
                    // Preparo Respuesta

                    // Header
                    Byte[] mensaje = new Byte[100];
                    mensaje[0] = 70; //F  
                    mensaje[1] = 72; //H  
                    // Año
                    mensaje[2] = Convert.ToByte(anioMSB + 48);
                    mensaje[3] = Convert.ToByte(anioMSB2 + 48);
                    mensaje[4] = Convert.ToByte(anioLSB2 + 48);
                    mensaje[5] = Convert.ToByte(anioLSB + 48);
                    // Mes
                    mensaje[6] = Convert.ToByte(mesMSB + 48);
                    mensaje[7] = Convert.ToByte(mesLSB + 48);
                    // Dia
                    mensaje[8] = Convert.ToByte(diaMSB + 48);
                    mensaje[9] = Convert.ToByte(diaLSB + 48);
                    // Horas
                    mensaje[10] = Convert.ToByte(horaMSB + 48);
                    mensaje[11] = Convert.ToByte(horaLSB + 48);
                    // Minutos
                    mensaje[12] = Convert.ToByte(minutosMSB + 48);
                    mensaje[13] = Convert.ToByte(minutosLSB + 48);
                    // Segundos
                    mensaje[14] = Convert.ToByte(segundosMSB + 48);
                    mensaje[15] = Convert.ToByte(segundosLSB + 48);

                    int x = 0;
                    while (x < 10)
                    {
                        mensaje[x + 16] = msg[x + 6];//pongo los 10 primeros bytes donde el modulo manda la version en la rta, para la version 6
                        x++;
                    }

                    mensaje[25] = 0; //LPmultipuerto

                    mensaje[26] = Convert.ToByte(32);//Tipo Mensaje
                    mensaje[27] = Convert.ToByte(32);//Opciones Mensaje

                    for (int i = 0; i < 70; i++) mensaje[i + 28] = Convert.ToByte(32);//Blancos
                    mensaje[44] = 0;//Nulos
                    mensaje[61] = 0;
                    mensaje[78] = 0;
                    mensaje[95] = 0;

                    Random r = new Random();
                    mensaje[96] = Convert.ToByte(r.Next(0, 200));
                    mensaje[97] = Convert.ToByte(r.Next(0, 200));

                    //CRC
                    UInt16 _crc = Utiles.CRC(mensaje, 98, 0);
                    mensaje[98] = Convert.ToByte(_crc & 0xFF);
                    mensaje[99] = Convert.ToByte(_crc >> 8);


                    //Respondo el ECO. 
                    //if ((_encriptacionActivada) & (mensaje.Length == 100)) mensaje = Utiles.encrypt(mensaje);
                    //udpClient.Send(mensaje, mensaje.Length, ipEndpoint.Address.ToString(), ipEndpoint.Port);
                }
                else
                {

                    textoMsg = "IP:" + ipEndpoint.Address.ToString() + " - UID:" + uid.ToString() + " CON VERSION:" + versionUDP;

                }
            }
            else
            {
                MessageBox.Show("Error de longitud o encabezado, recibidos: " + msg.Length + " bytes");
            }
        }

        /// <summary>
        /// Valida la cabecera del mensaje enviado por el modulo
        /// </summary>
        /// <param name="datagrama"></param>
        /// <returns></returns>
        private Boolean cabeceraValida(Byte[] datagrama)
        {
            //VALIDO QUE SEA UN "HELLO"
            return ((Convert.ToInt16(datagrama[0]) == 72) & (Convert.ToInt16(datagrama[1]) == 69) &
                    (Convert.ToInt16(datagrama[2]) == 76) & (Convert.ToInt16(datagrama[3]) == 76) &
                    (Convert.ToInt16(datagrama[4]) == 79));
        }

    }
}
