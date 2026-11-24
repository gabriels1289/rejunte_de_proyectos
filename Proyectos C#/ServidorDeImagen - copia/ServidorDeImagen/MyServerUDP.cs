using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Windows.Forms;

namespace ServidorDeImagen
{
    public class MyServerUDP
    {
        int recv;
        Byte[] data = new Byte[1024];
        Byte[] respuesta = { 0x52, 0x4f, 0x47, 0x45, 0x52 }; // ROGER
        List<IPDate> miTablaIP = new List<IPDate>();
        Byte[] inicio = new Byte[2];
        Byte[] middle = new Byte[2];
        //A server - la clase deberia correr en su propio hilo

        //Set UDP port number       
        IPEndPoint ipep = new IPEndPoint(IPAddress.Any, 1200);
        Socket newsock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
        Socket socketEnvio = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
        IPEndPoint sender = new IPEndPoint(IPAddress.Any, 1200);
        EndPoint Remote;// = (EndPoint)(sender);
        // Volatile is used as hint to the compiler that this data
        // member will be accessed by multiple threads.
        private volatile bool _shouldStop;

        public MyServerUDP()
        {
            newsock.Bind(ipep);
            Remote = (EndPoint)(sender);
            inicio[0] = 0x46;//'F'
            inicio[1] = 0x44;//'D'
            middle[0] = 0x4d;//'M'
            middle[1] = 0x44;//'D'
        }

        public void RequestStop()
        {
            _shouldStop = true;
        }

        public void DoWork()
        {
            while (!_shouldStop)
            {
                IPDate variable = new IPDate();
                //Receive message.
                recv = newsock.ReceiveFrom(data, ref Remote);
                ProcesarKeepAlive();
                variable.IP = Remote;
                variable.Date = DateTime.Now;
                if (!miTablaIP.Exists(x => x.IP == variable.IP))// no existe agregarla
                {
                    miTablaIP.Add(variable);
                }
                else
                {//refrescar el tiempo de esa IP en la tabla
                    miTablaIP.Remove(miTablaIP.Find(x => x.IP.Equals(variable.IP)));
                    miTablaIP.Add(variable);
                }

                //aca verifico si el paquete es un keep alive compara data con "ESTOY VIVO" y despues hago data.clear()??
                //Console.WriteLine(Encoding.ASCII.GetString(data, 0, recv));
                //newsock.SendTo(data, recv, SocketFlags.None, Remote);
                //validar cada un cierto tiempo para saber cuales estan vivas
            }
        }

        public void SendImagen(String imagen)
        {
            try
            {
                int cantidadLeidos = 0;
                int j = 0;
                BMP bmp;
                Byte[] header = new Byte[54];
                FileStream stream = new FileStream(imagen, FileMode.Open, FileAccess.Read);
                BinaryReader reader = new BinaryReader(stream);

                cantidadLeidos = reader.Read(header, 0, 54);
                bmp = new BMP(header);
                Byte[] buffer = new Byte[bmp.AltoEnPixels() * bmp.AnchoEnPixels() * 3]; //obtengo el tamaño del arreglo
                //cantidadLeidos = reader.Read(buffer, 54, buffer.Length-1);    //leo despues del encabezado buffer.lenght bytes
                buffer = reader.ReadBytes(buffer.Length);

                reader.Close();
                stream.Close();
                if (miTablaIP.Count >= 0)
                {
                    foreach (IPDate a in miTablaIP)
                    {
                        data[0] = inicio[0];
                        data[0] = inicio[1];
                        for (int i = 2; i < 1024 && j < buffer.Length; i++)
                        {
                            data[i] = buffer[j];
                            j++;
                        }
                        socketEnvio.SendTo(data, 0, SocketFlags.None, a.IP);
                        System.Threading.Thread.Sleep(50);
                        data[0] = middle[0];
                        data[0] = middle[1];
                        while (j < buffer.Length)
                        {
                            for (int i = 2; i < 1024 && j < buffer.Length; i++)
                            {
                                data[i] = buffer[j];
                                j++;
                            }
                            socketEnvio.SendTo(data, 0, SocketFlags.None, a.IP);
                            System.Threading.Thread.Sleep(50);
                        }
                    }
                }

                //aca envio a cada IP viva, la imagen de acada 1018 bytes con los 4 primeros bytes indicando las 2 dimensiones de la imagen
                //+ 2 bytes q indican si es el primer paquete
                /*FORMATO mensajes:
                 *MENSAJE KEEP ALIVE DESDE LA DISCOVERY AL SERVER:
                 *  "LIVE"
                 *MENSAJE KEEP ALIVE DESDE EL SERVER A LA DISCOVERY:
                 *  "ROGER"
                 *MENSAJE DE IMAGENES DESDE EL SERVIDOR:
                 *  "FD" + alto(4 bytes) + ancho(4 bytes) + data(1014 bytes) //FD: FIRST DATAGRAM
                 *  "MD" + data(1022 bytes) //MD: MIDDLE DATAGRAM
                 *si el modulo recive un fd, llama a inicializar el tamaño de la tft
                 *Byte[] prueba = BitConverter.GetBytes(272); -> al parecer deja en el 1º byte la parte menos significativa
                 */         
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message, "MyServerUDP/SendImagen");
            }
        }

        private void ProcesarKeepAlive()
        {
            if (data[0]=='L' && data[1]=='I' && data[2]=='V' && data[3]=='E')
            {
                socketEnvio.SendTo(respuesta, recv, SocketFlags.None, Remote);
            }
        }

        public String GetHostName()
        {
            return "Este es un Servidor, su host name es: " + Dns.GetHostName();
        }
        
        public void RefrescarTablaIPs()
        {
            try
            {
                if (miTablaIP.Count >= 0)
                {
                    for (int i = miTablaIP.Count - 1; i >= 0; i--)
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
            catch (Exception ex) { MessageBox.Show(ex.Message); }
        }

        public List<IPDate> GetTablaIP()
        {
            return miTablaIP;
        }
               
    }

    public class IPDate
    {
        public EndPoint IP { get; set; }
        public DateTime Date { get; set; }
    }
}
