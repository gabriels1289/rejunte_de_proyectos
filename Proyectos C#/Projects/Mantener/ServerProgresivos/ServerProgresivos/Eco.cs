using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace ServerProgresivos
{
    /*
     * Clase para el manejo del Hello de los módulos
     * Version 1.0.0 
     */
    public class Eco
    {
        private Int32 _puerto;
        private Semaforo _semaforo;
        private volatile bool _shouldStop;
        private volatile bool _deboParar;

        private Boolean _encriptacionActivada;
        UdpClient receivingUdpClient;
        private IREco servicio;
        Hashtable _tragamonedas;
        
        private Boolean _multipleHilosContadores;
        //private static ILog logEcoAdmin;
        EventLog myLog;

        public Eco(Int32 puerto, Hashtable tragamonedas, Semaforo sema, Boolean encriptacionActivada, Boolean multipleHilosContadores, IREco service)
        {
            _shouldStop = false;
            _deboParar = false;
            _puerto = puerto;
            _tragamonedas = tragamonedas;
            _semaforo = sema;
            //_oMiConexion = oMiConexion;
            _encriptacionActivada = encriptacionActivada;
            _multipleHilosContadores = multipleHilosContadores;
            servicio = service;

            if (!EventLog.SourceExists("ServerProgresivos"))
            {
                //An event log source should not be created and immediately used.
                //There is a latency time to enable the source, it should be created
                //prior to executing the application that uses the source.
                //Execute this sample a second time to use the new source.
                EventLog.CreateEventSource("ServerProgresivos", "MyNewLog");
                Console.WriteLine("CreatedEventSource");
                Console.WriteLine("Exiting, execute the application a second time to use the source.");
                // The source is created.  Exit the application to allow it to be registered.
                return;
            }

            // Create an EventLog instance and assign its source.
            EventLog myLog = new EventLog();
            myLog.Source = "ServerProgresivos";
        }

        /// <summary>
        /// Metodo principal del Thread. Se ejecuta hasta que se indique explicitamente que el hilo debe parar.
        /// Se encarga de crear un Endpoint para leer paquetes UDP en un puerto predeterminado,
        /// controlar el formato de los paquetes y el origen de los mismos, y en caso
        /// de ser valido lo procesa. Caso contrario lo descarta.
        /// Para leer Datagramas UDP se bloquea hasta recibir un paquete(BUSY WAITING).
        /// </summary>
        public void DoWork()
        {
            //logEcoAdmin = LogManager.GetLogger(typeof(EcoAdmin));
            Boolean puertoOK = true;

            _shouldStop = false;
            //logEcoAdmin.Info("ECO SERVER UDP - Iniciando.");
            // Write an informational entry to the event log.    
            myLog.WriteEntry("ECO SERVER UDP - Iniciando.", EventLogEntryType.Information);

            try
            {
                Thread.Sleep(3000);
                receivingUdpClient = new UdpClient(_puerto);

                puertoOK = true;
            }
            catch (Exception ex)
            {
                //logEcoAdmin.Error("HILO ECO UDP - NO INICIADO. Puerto:" + _puerto + " EN USO.", ex);
                myLog.WriteEntry("HILO ECO UDP - NO INICIADO. Puerto:" + _puerto + " EN USO. ... " + ex, EventLogEntryType.Error);
                servicio.Stop();
                puertoOK = false;

            }
            //Creo un IPEndPoint para establecer el IP Address y el port del remitente. 
            // Este IPEndPoint permite leer datagramas enviados por cualquier fuente.
            if (puertoOK)
            {
                IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, _puerto);

                //logEcoAdmin.Info("HILO ECO SERVER UDP - INICIADO OK. Puerto:" + _puerto);
                myLog.WriteEntry("HILO ECO SERVER UDP - INICIADO OK. Puerto:" + _puerto, EventLogEntryType.Information);
                String ip;

                while (!_shouldStop)
                {

                    ip = "";
                    try
                    {

                        try
                        {
                            ip = RemoteIpEndPoint.Address.ToString();

                        }
                        catch (Exception ex)
                        {
                            //logEcoAdmin.Error("ECO IP NULA", ex);
                            myLog.WriteEntry("ECO IP NULA ... "+ ex);
                        }
                        Thread.Sleep(20);
                        Byte[] receiveBytes = receivingUdpClient.Receive(ref RemoteIpEndPoint);

                        // Si el datagrama es valido lo proceso, sino lo descarto

                        if ((_encriptacionActivada) & (receiveBytes.Length == 30)) receiveBytes = Utiles.encrypt(receiveBytes);

                        if ((_encriptacionActivada) & (receiveBytes.Length == 36)) receiveBytes = Utiles.encrypt(receiveBytes);

                        if (validarFormatoDatagrama(receiveBytes))
                        {
                            // Formato Valido.
                            // SI IP Valido procesar paquete
                            if (validarHost(RemoteIpEndPoint))
                            {
                                procesarEco(receiveBytes, RemoteIpEndPoint);

                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        int code = System.Runtime.InteropServices.Marshal.GetExceptionCode();

                       // logEcoAdmin.Error("ECO  while (!_shouldStop) IP:" + ip + " Error Cod:" + code, ex);
                        myLog.WriteEntry("ECO  while (!_shouldStop) IP:" + ip + " Error Cod:" + code +" ... "+ ex, EventLogEntryType.Error);
                    }

                }
            }


            //logEcoAdmin.Info("ECO UDP - Cerrado...");
            myLog.WriteEntry("ECO UDP - Cerrado...", EventLogEntryType.Information);
        }

        /// <summary>
        /// Esta función se llama para finalizar la ejecución del hilo.
        /// </summary>
        public void RequestStop()
        {
            _shouldStop = true;
            _deboParar = true;
        }


        /// <summary>
        /// Procesa el mensaje recibido responde el mensaje.
        /// En caso de una actualización de versiones en el modulo BB2
        /// se actualizara automaticamente el padron de maquinas.
        /// </summary>
        /// <param name="msg"></param>
        /// <param name="ipEndpoint"></param>
        private void procesarEco(Byte[] msg, IPEndPoint ipEndpoint)
        {

            //Tragamoneda slot = (Tragamoneda)_tragamonedas[ipEndpoint.Address.ToString()];
            if (cabeceraValida(msg) & msg.Length == 36)
            {

                // Concateno Fecha y Hora Actual - Formato de 24hs - Ver Protocolo UDP Anexo
                // String fecha = DateTime.Now.ToString("dd/MM/yyyy HH:m:s");
                // Byte[] bFecha = System.Text.Encoding.ASCII.GetBytes(fecha.ToCharArray());

                Byte[] mensaje;
                /* Si la longitud del mensaje enviado por el módulo es 30 quiere decir que esta mandando el hello+version del modulo
                 * sino solamente esta enviando hello
                 * Si envia hello+version del modulo se responde el mensaje con la hora+carga maxima unitaria del vendedor
                 * si la version del modulo es menor a 5, si es mayor a 5 se envia la respuesta con la opcionde enviar mensajes en el paquete
                 * sino se responde el mensaje con la hora
                 */
                //slot.FechaTransmision = DateTime.Now;
                
                //RECUPERO LA VERSION DEL MODULO 16383
                mensaje = new Byte[100];
                String versionUDP = "";
                String vcc = "";
                int indice = 0;
                Int32 jumper = msg[32] & 0x03;  // cont_hello = msg[32] >> 2;
                //if (slot.JumperModulo != jumper) slot.actualizarJumperModulo(slot.tragamonedaID, jumper, _oMiConexion);
                while (Convert.ToInt16(msg[indice + 6]) > 0)
                {
                    versionUDP = versionUDP + Convert.ToChar(msg[indice + 6]);
                    indice++;
                }
                //if (versionUDP.Equals("INIT")) slot.actualizarReiniciosModulo(slot.tragamonedaID, slot.tragamonedaUID, slot.tragamonedaIP, _oMiConexion);
                 //logEcoAdmin.Warn(versionUDP);
                myLog.WriteEntry(versionUDP, EventLogEntryType.Warning);
                Decimal version = 0;
                try
                {
                    version = Convert.ToDecimal(versionUDP.Substring(0, versionUDP.IndexOf('.')));
                }
                catch (Exception ex)
                {
                    version = 0;
                }
                if (version >= 6)
                {
                    /*if (!(versionUDP.Trim().ToUpper().Equals(slot.ModuloBB2Version)))
                    {   // Si cambio la versión del modulo actualizo el padron.
                        String versionAnterior = slot.ModuloBB2Version;
                        slot.ModuloBB2Version = versionUDP.Trim().ToUpper();
                        slot.actualizarVersionModuloBB2(slot.tragamonedaID, versionAnterior, versionUDP.Trim().ToUpper(), _oMiConexion);
                    }*/
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
                        //logEcoAdmin.Warn("Error. Slot:" + slot.tragamonedaUID + " - Version Mensaje:" + versionUDP, ex);
                        myLog.WriteEntry("Error. Slot:" + slot.tragamonedaUID + " - Version Mensaje:" + versionUDP+" ... "+ex, EventLogEntryType.Warning);
                    }

                    vcc = vcc.Replace('.', ParametrosGlobales.separadorDeDecimales);
                    Decimal tension = Convert.ToDecimal(vcc);
                    String tok = "3.15";
                    tok = tok.Replace('.', ParametrosGlobales.separadorDeDecimales);
                    Decimal tensionOK = (Convert.ToDecimal(tok));
                    if ((slot.Vcc >= tensionOK) & (tension < tensionOK))
                    {
                        slot.Vcc = tension;
                        slot.guardarTension(_oMiConexion);
                    }
                    if ((slot.Vcc < tensionOK) & (tension >= tensionOK))
                    {
                        slot.Vcc = tension;
                        slot.guardarTension(_oMiConexion);
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

                    if (_multipleHilosContadores)
                    {
                        mensaje[25] = 1;
                    }
                    else
                    {
                        mensaje[25] = 0;
                    }


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
                    if ((_encriptacionActivada) & (mensaje.Length == 100)) mensaje = Utiles.encrypt(mensaje);
                    receivingUdpClient.Send(mensaje, mensaje.Length, ipEndpoint.Address.ToString(), ipEndpoint.Port);
                }
                else
                {

                    // logEcoAdmin.Warn("IP:" +slot.tragamonedaIP+" - UID:"+slot.tragamonedaUID +" CON VERSION:"+versionUDP);
                    myLog.WriteEntry("IP:" + slot.tragamonedaIP + " - UID:" + slot.tragamonedaUID + " CON VERSION:" + versionUDP, EventLogEntryType.Warning);
                }
            }

        }

        /// <summary>
        /// Valida que el datagrama este correctamente formado.
        /// </summary>
        /// <param name="datagrama"></param>
        /// <returns></returns>
        private Boolean validarFormatoDatagrama(Byte[] datagrama)
        {

            switch (datagrama.Length)
            {
                case 36:
                    {//De la version 5.00 en adelante los modulos bb2 tienen una longitud de paquete de 30 bytes.
                        Byte tmp = 0xFF; // Recorro todo el codigo de la tarjeta. 
                        for (int i = 0; i < 11; i++)
                            if (datagrama[i] != 0xFF) tmp = 0;

                        if (tmp == 0xFF) return false;
                        else return true;
                    }
                default: return false;
            }
        }

        /// <summary>
        /// Valida que el IP corresponda a una tragamoneda activa.
        /// </summary>
        /// <param name="remoteIP"></param>
        /// <returns></returns>
        private Boolean validarHost(IPEndPoint remoteIP)
        {
            return _tragamonedas.Contains(remoteIP.Address.ToString());
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
