using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Net.Sockets;
using System.Windows.Forms;
using System.Net;

namespace ServidorDeImagen
{
    public class Mensajero
    {
        List<IPDate> miTablaIP = new List<IPDate>();
        Byte[] inicio = new Byte[2];
        Byte[] middle = new Byte[2];
        Byte[] fin = new Byte[2];
        IPEndPoint ipep = new IPEndPoint(IPAddress.Any, 1200);
        EndPoint Remote;// = (EndPoint)(sender);
        Socket socketEnvio = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);

        public Mensajero()
        {
            //newsock.Bind(ipep);
            Remote = (EndPoint)(ipep);
            inicio[0] = 0x46;//'F'
            inicio[1] = 0x44;//'D'
            middle[0] = 0x4d;//'M'
            middle[1] = 0x44;//'D'
            fin[0] = 0x4c;//'L'
            fin[1] = 0x44;//'D'
        }

        public void SendImagen(String imagen)
        {
            try
            {
                int cantidadLeidos = 0;
                int j = 0;
                BMP bmp;
                Byte[] flujo = new Byte[1024];
                Byte[] header = new Byte[54];
                Byte[] aux = new Byte[4];
                FileStream stream = new FileStream(imagen, FileMode.Open, FileAccess.Read);
                BinaryReader reader = new BinaryReader(stream);

                cantidadLeidos = reader.Read(header, 0, 54);
                bmp = new BMP(header);
                Byte[] buffer = new Byte[bmp.AltoEnPixels() * bmp.AnchoEnPixels() * 3]; //obtengo el tamaño del arreglo

                //cantidadLeidos = reader.Read(buffer, 54, buffer.Length-1);    //leo despues del encabezado buffer.lenght bytes
                buffer = reader.ReadBytes(buffer.Length);

                reader.Close();
                stream.Close();
               if (miTablaIP.Count > 0)
               {
                    foreach (IPDate a in miTablaIP)
                    {
                        flujo[0] = inicio[0];
                        flujo[1] = inicio[1];
                        aux = BitConverter.GetBytes(bmp.AnchoEnPixels());
                        flujo[2] = aux[0];
                        flujo[3] = aux[1];
                        flujo[4] = aux[2];
                        flujo[5] = aux[3];
                        aux = BitConverter.GetBytes(bmp.AltoEnPixels());
                        flujo[6] = aux[0];
                        flujo[7] = aux[1];
                        flujo[8] = aux[2];
                        flujo[9] = aux[3];
                        for (int i = 10; i < 1024 && j < buffer.Length; i++) //1024 -> bytes totales
                        {                                                    //1014 -> sacando el encabezado (2 + 8 del tamaño)
                            flujo[i] = buffer[j];                            //1014/3 bytes/pixel = 338 pixeles
                            j++;
                        }
                        socketEnvio.SendTo(flujo, a.IP);
                        System.Threading.Thread.Sleep(200);
                        //socketEnvio.ReceiveFrom(flujo, ref Remote);
                        
                        flujo[0] = middle[0];
                        flujo[1] = middle[1];
                        while (j < buffer.Length)
                        {
                            for (int i = 2; i < 1022 && j < buffer.Length; i++) //1022 -> bytes totales
                            {                                                   //1020 -> sacando los 2 del encabezado
                                flujo[i] = buffer[j];                           //1020/3 bytes/pixels = 340 pixeles
                                j++;
                            }
                            socketEnvio.SendTo(flujo, a.IP);
                            System.Threading.Thread.Sleep(200);
                            //socketEnvio.ReceiveFrom(flujo, ref Remote);
                        }
                        flujo[0] = fin[0];
                        flujo[1] = fin[1];
                        socketEnvio.SendTo(flujo, a.IP);
                        //System.Threading.Thread.Sleep(500);
                        //socketEnvio.ReceiveFrom(flujo, ref Remote);
                    }
                }
                /*para el caso de una imagen de 480*272 -> 130560 bytes : (130560-338)=130222 / 340 = 383,005 -> 385 paquetes por imagen
               
                 */
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message, "Mensajero/SendImagen");
            }
        }
        public void SetTablaIP(List<IPDate> _miTablaIP)
        {
            miTablaIP.Clear();
            //miTablaIP.Add(_miTablaIP);
            miTablaIP = _miTablaIP;
        }
    }
}
