using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ClienteUDP
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        #region variablesGlobales
        UInt16 n = 0;
        Boolean _encriptacionActivada = false;
        #endregion

        //modificar para controlar que la ip del textbox sea valida
        public static IPAddress ValidaIP(string direccionIP)
        {
            IPAddress ip = new IPAddress(new byte[] { 0, 0, 0, 0 });
            IPAddress.TryParse(direccionIP, out ip);
            return ip;
        }

        private void button_Enviar_Click(object sender, EventArgs e)
        {
            UdpClient udpClient = new UdpClient();

            udpClient.Connect(textBox_Msg.Text, 1200);
            //Byte[] senddata = Encoding.ASCII.GetBytes("Hello World");
            Byte[] senddata = CrearHello();
            udpClient.Send(senddata, senddata.Length);
        }

        private Byte[] CrearHello()
        {
            //Byte = unsigned char ---- SByte = char ---- UInt16= unsigned int (el micro del módulo es de 16 bits, por lo tanto tambien lo son los int)
            Byte[] hello = new Byte[6];         // Id. del mensaje --> "HELLO" (char)
            Byte[] version = new Byte[20];      // Hasta 19 ASCII + null (char)
            Byte[] vcc = new Byte[6];           // 5 dígitos + null - Ej.: 3.258 (char)
            Byte vgmProto = 0x00;               // SAS - DACOM - GAMMA - EIBE (unsigned char)
            UInt16 uid = 1234;                   // UID de la VGM (unsigned int)
            UInt16[] mac = new UInt16[3];       // dirección MAC del módulo (unsigned int)
            UInt16[] IpMistery = new UInt16[2]; // dirección IP del controlador del Mistery (0.0.0.0 --> no hay) (unsigned int)
            UInt16 hash;                        // Hash (unsigned int)
            UInt16 crc;                         // Crc (unsigned int)

            // asigno valores iniciales
            hello = Encoding.ASCII.GetBytes("HELLO#");

            Random random2 = new Random();
            UInt16 selVersion = (UInt16)random2.Next(6);
            if (n == 0)
            {
                version = Encoding.ASCII.GetBytes("INIT################");
            }
            else
            {
                switch (selVersion)
                {
                    case 0:
                    // version = Encoding.ASCII.GetBytes("INIT################");
                    // break;
                    case 1:
                        version = Encoding.ASCII.GetBytes("1.00################");
                        break;
                    case 2:
                        version = Encoding.ASCII.GetBytes("WDT INIT############");
                        break;
                    case 3:
                    // version = Encoding.ASCII.GetBytes("MD5 ERRR############");
                    // break;
                    case 4:
                    // version = Encoding.ASCII.GetBytes("UID 192.168.1.111###");
                    // break;
                    case 5:
                    // version = Encoding.ASCII.GetBytes("DUP 192.168.1.111###");
                    // break;
                    default:
                        version = Encoding.ASCII.GetBytes("1.00################");
                        break;
                }
            }

            vcc = Encoding.ASCII.GetBytes("3.2582");
            mac[0] = 1;
            mac[1] = 2;
            mac[2] = 3;
            IpMistery[0] = 0;
            IpMistery[1] = 0;
            Random random = new Random();
            UInt16 randomNumber = (UInt16)random.Next(65536);
            hash = (UInt16)((randomNumber & 0xff00) | (n++ & 0x00ff));

            // Creo un arreglo de bytes unificando todos los campos
            Byte[] mensaje = new Byte[49];
            // relleno el arreglo a enviar
            mensaje[0] = hello[0];  // hello
            mensaje[1] = hello[1];
            mensaje[2] = hello[2];
            mensaje[3] = hello[3];
            mensaje[4] = hello[4];
            mensaje[5] = hello[5];
            mensaje[6] = version[0];    // version
            mensaje[7] = version[1];
            mensaje[8] = version[2];
            mensaje[9] = version[3];
            mensaje[10] = version[4];
            mensaje[11] = version[5];
            mensaje[12] = version[6];
            mensaje[13] = version[7];
            mensaje[14] = version[8];
            mensaje[15] = version[9];
            mensaje[16] = version[10];
            mensaje[17] = version[11];
            mensaje[18] = version[12];
            mensaje[19] = version[13];
            mensaje[20] = version[14];
            mensaje[21] = version[15];
            mensaje[22] = version[16];
            mensaje[23] = version[17];
            mensaje[24] = version[18];
            mensaje[25] = version[19];
            mensaje[26] = vcc[0];   // vcc
            mensaje[27] = vcc[1];
            mensaje[28] = vcc[2];
            mensaje[29] = vcc[3];
            mensaje[30] = vcc[4];
            mensaje[31] = vcc[5];
            mensaje[32] = vgmProto; // proto
            mensaje[33] = Convert.ToByte(uid & 0xFF);  // uid
            mensaje[34] = Convert.ToByte(uid >> 8);
            mensaje[35] = Convert.ToByte(mac[0] & 0xFF);   // mac
            mensaje[36] = Convert.ToByte(mac[0] >> 8);
            mensaje[37] = Convert.ToByte(mac[1] & 0xFF);
            mensaje[38] = Convert.ToByte(mac[1] >> 8);
            mensaje[39] = Convert.ToByte(mac[2] & 0xFF);
            mensaje[40] = Convert.ToByte(mac[2] >> 8);
            mensaje[41] = Convert.ToByte(IpMistery[0] & 0xFF);   // ipmistery
            mensaje[42] = Convert.ToByte(IpMistery[0] >> 8);
            mensaje[43] = Convert.ToByte(IpMistery[1] & 0xFF);
            mensaje[44] = Convert.ToByte(IpMistery[1] >> 8);
            mensaje[45] = Convert.ToByte(hash & 0xFF);  // hash
            mensaje[46] = Convert.ToByte(hash >> 8);


            //CRC
            crc = Utiles.CRC(mensaje, 47, 0);
            mensaje[47] = Convert.ToByte(crc & 0xFF);
            mensaje[48] = Convert.ToByte(crc >> 8);

            return mensaje;
        }
    }
}
