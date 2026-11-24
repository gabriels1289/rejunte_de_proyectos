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
        Byte[] data2 = new Byte[1024];
        Byte[] respuesta = { 0x52, 0x4f, 0x47, 0x45, 0x52 }; // ROGER
        List<IPDate> miTablaIP = new List<IPDate>();
        Byte[] inicio = new Byte[2];
        Byte[] middle = new Byte[2];
        Byte[] fin = new Byte[2];
        //A server - la clase deberia correr en su propio hilo

        //Set UDP port number       
        IPEndPoint ipep = new IPEndPoint(IPAddress.Any, 1500);
        Socket newsock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
        Socket socketEnvio = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
        //IPEndPoint sender = new IPEndPoint(IPAddress.Any, 1200);
        IPEndPoint broadcast = new IPEndPoint(IPAddress.Parse("192.168.2.25"), 1200);
        EndPoint epBroadcast;// = (EndPoint)(sender);
        EndPoint Remote;// = (EndPoint)(sender);
        // Volatile is used as hint to the compiler that this data
        // member will be accessed by multiple threads.
        private volatile bool _shouldStop = false;

        public MyServerUDP()
        {
            //newsock.Bind(ipep);
            //Remote = (EndPoint)(sender);
            epBroadcast = (EndPoint)broadcast;
            inicio[0] = 0x46;//'F'
            inicio[1] = 0x44;//'D'
            middle[0] = 0x4d;//'M'
            middle[1] = 0x44;//'D'
            fin[0] = 0x4c;//'L'
            fin[1] = 0x44;//'D'
        }

        public void RequestStop()
        {
            _shouldStop = true;
        }

        public void DoWork()
        {
            newsock.Bind(ipep);
            Remote = (EndPoint)(ipep);
            while (!_shouldStop)
            {

                IPDate variable = new IPDate();
                //Receive message.
                recv = newsock.ReceiveFrom(data2, ref Remote);
                //ProcesarKeepAlive();
                if (data2[0] == 'L' && data2[1] == 'I' && data2[2] == 'V' && data2[3] == 'E')
                {
                    newsock.SendTo(respuesta, Remote);
                }
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

            }
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
                        flujo[i] = buffer[j];                            //1014/3 bytes/pixel = 338 pixeles -> 253 palabras de 32 bits
                        j++;                                             
                    }
                    socketEnvio.SendTo(flujo, epBroadcast);
                    System.Threading.Thread.Sleep(200);
                    flujo[0] = middle[0];
                    flujo[1] = middle[1];
                    while (j < buffer.Length)
                    {
                        for (int i = 2; (i < 1020) && (j < buffer.Length); i++) //1022 -> bytes totales
                        {                                                   //1020 -> sacando los 2 del encabezado
                            flujo[i] = buffer[j];                           //1020/3 bytes/pixels = 340 pixeles -> 255 palabras de 32 bits
                            j++;                                           
                        }
                        socketEnvio.SendTo(flujo, epBroadcast);
                        System.Threading.Thread.Sleep(200);
                    }
                    flujo[0] = fin[0];
                    flujo[1] = fin[1];
                    socketEnvio.SendTo(flujo, epBroadcast);

                    /*para el caso de una imagen de 480*272 -> 130560 bytes : (130560-338)=130222 / 340 = 383,005 -> 385 paquetes por imagen
             
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
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "MyServerUDP/SendImagen");
            }
        }

        private void ProcesarKeepAlive()
        {
            if (data2[0] == 'L' && data2[1] == 'I' && data2[2] == 'V' && data2[3] == 'E')
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
