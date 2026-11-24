using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.IO;
using System.Net;
using System.Windows.Forms;

namespace ServerPrueba
{
    public class MyServerUDP
    {
        int recv;
        byte[] data = new byte[1024];
        byte[] respuesta = { 0x52, 0x4f, 0x47, 0x45, 0x52 };
        List<IPDate> miTablaIP = new List<IPDate>();

        //A server - la clase deberia correr en su propio hilo

        //Set UDP port number       
        IPEndPoint ipep = new IPEndPoint(IPAddress.Any, 1200);
        Socket newsock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
        Socket socketEnvio = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
        IPEndPoint sender = new IPEndPoint(IPAddress.Any, 1200);
        EndPoint Remote;// = (EndPoint)(sender);

        public MyServerUDP()
        {
            newsock.Bind(ipep);
            Remote = (EndPoint)(sender);
        }

        public void DoWork()
        {
            while (true)
            {
                IPDate variable = new IPDate();
                //Receive message.
                recv = newsock.ReceiveFrom(data, ref Remote);
                variable.IP = Remote.ToString();
                variable.Date = DateTime.Now;
                if (!miTablaIP.Exists(x => x.IP == variable.IP))// no existe agregarla
                {
                    miTablaIP.Add(variable);
                    socketEnvio.SendTo(respuesta, Remote);
                }
                else
                {//refrescar el tiempo de esa IP en la tabla
                    miTablaIP.Remove(miTablaIP.Find(x => x.IP.Contains(variable.IP)));
                    miTablaIP.Add(variable);
                    socketEnvio.SendTo(respuesta, Remote);
                }

                //aca verifico si el paquete es un keep alive compara data con "ESTOY VIVO" y despues hago data.clear()??
                //Console.WriteLine(Encoding.ASCII.GetString(data, 0, recv));
                //newsock.SendTo(data, recv, SocketFlags.None, Remote);
                //validar cada un cierto tiempo para saber cuales estan vivas
            }
        }


        public String GetHostName()
        {
            return "Este es un Servidor, su host name es: " + Dns.GetHostName();
        }

        public void SendImagen(String imagen)
        {
            try
            {
                int cantidadLeidos = 0;
                BMP bmp;
                Byte[] header = new Byte[54];
                FileStream stream = new FileStream(imagen, FileMode.Open, FileAccess.Read);
                BinaryReader reader = new BinaryReader(stream);

                cantidadLeidos = reader.Read(header, 0, 54);
                bmp = new BMP(header);
                Byte[] buffer = new Byte[bmp.AltoEnPixels() * bmp.AnchoEnPixels() * 3]; //obtengo el tamaño del arreglo
                cantidadLeidos = reader.Read(buffer, 54, buffer.Length);    //leo despues del encabezado buffer.lenght bytes

                //aca envio a cada IP viva, la imagen de acada 1018 bytes con los 4 primeros bytes indicando las 2 dimensiones de la imagen
                //+ 2 bytes q indican si es el primer paquete

                reader.Close();
                stream.Close();
            }
            catch
            {
                Console.WriteLine("Error");
            }
        }

        public void RefrescarTablaIPs()
        {
            try
            {
                if (miTablaIP.Count >= 0)
                {
                    lock (miTablaIP)
                    {
                        for(int i = miTablaIP.Count - 1; i >= 0; i--) 
                        {

                            DateTime oldDate = miTablaIP[i].Date;
                            DateTime newDate = DateTime.Now;
                            // Difference in days, hours, and minutes.
                            TimeSpan ts = newDate - oldDate;
                            // Difference in seconds.
                            int differenceInDays = ts.Seconds;
                            if (differenceInDays > 30)//si la direccion estuvo mas de 30 segundos la elimino
                            {
                                //elimino la direccion ip de la tabla
                                miTablaIP.RemoveAt(i);
                            }
                        }
                    }
                }
            }
            catch (Exception ex) { MessageBox.Show(ex.Message); }
        }

        public List<IPDate> GetTablaIP()
        {
            return miTablaIP;
        }



    }
}
