using Microsoft.Win32;
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration;
using System.Data;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.ServiceProcess;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using log4net;
using System.Xml;
//[assembly: log4net.Config.XmlConfigurator(Watch = true)]

namespace WService
{
    public partial class Service1 : ServiceBase
    {
        #region atributos
        private Thread misteryServerThread = null;

        private Thread misteryUDPServerThread = null;
        private Thread misteryUDPSenderThread = null;
        private Thread misteryBackUpThread = null;
        private Thread misteryAuditorPremiosSenderThread = null;

        private Thread sorteadorPozo0Thread = null;
        private Thread sorteadorPozo1Thread = null;
        private Thread sorteadorPozo2Thread = null;

        private MisteryUDPServer misteryServerUDP;
        private MisterySender misterySender;
        private BackUp misteryBackUp;
        //    private BackUpBB2 misteryBackUpBB2;
        private MisteryAuditorPremiosSender misteryAuditorPremiosSender;

        private SorteadorPozo sorteadorPozo0;
        private SorteadorPozo sorteadorPozo1;
        private SorteadorPozo sorteadorPozo2;

        private Hashtable tragamonedasDelMistery = new Hashtable();

        private Mistery mistery;

        private static ILog log;

        #endregion atributos

        public Service1()
        {
            InitializeComponent();
            Assembly assem = Assembly.GetEntryAssembly();
            AssemblyName assemName = assem.GetName();
            Version ver = assemName.Version;
            log = LogManager.GetLogger(typeof(Service1));
            log4net.Config.XmlConfigurator.Configure();
        }

        #region Servicio
        protected override void OnStart(string[] args)
        {
            try
            {
                mistery = new Mistery();
                //cargar configuración desde XML y BD's                

                if (setearMystery())
                {
                    log.Info("MYSTERY IP: " + ParametrosGlobales.MISTERY_IP + " IRMisteryService iniciando...");
                    registrarConfiguración();

                    log.Info("CREANDO misteryBackup...IP: " + ParametrosGlobales.MISTERY_IP);
                    misteryBackUp = new BackUp(mistery);
                    misteryBackUpThread = new Thread(misteryBackUp.DoWork);
                    misteryBackUpThread.IsBackground = true;
                    misteryBackUpThread.Start();

                    log.Info("CREANDO misteryServerUDP...IP: " + ParametrosGlobales.MISTERY_IP);
                    misteryServerUDP = new MisteryUDPServer(mistery, tragamonedasDelMistery);
                    misteryUDPServerThread = new Thread(misteryServerUDP.DoWork);
                    misteryUDPServerThread.IsBackground = true;
                    misteryUDPServerThread.Start();

                    log.Info("CREANDO misterySender...IP: " + ParametrosGlobales.MISTERY_IP);
                    misterySender = new MisterySender(mistery, tragamonedasDelMistery);
                    misteryUDPSenderThread = new Thread(misterySender.DoWork);
                    misteryUDPSenderThread.IsBackground = true;
                    misteryUDPSenderThread.Start();

                    log.Info(" CREANDO MisteryAuditorPremiosSender...IP: " + ParametrosGlobales.MISTERY_IP);
                    misteryAuditorPremiosSender = new MisteryAuditorPremiosSender();
                    misteryAuditorPremiosSenderThread = new Thread(misteryAuditorPremiosSender.DoWork);
                    misteryAuditorPremiosSenderThread.IsBackground = true;
                    misteryAuditorPremiosSenderThread.Start();
                    log.Info("INICIANDO SORTEADORES....IP: " + ParametrosGlobales.MISTERY_IP);
                    sorteadorPozo0 = new SorteadorPozo(mistery, tragamonedasDelMistery, 0, mistery.sorteosRealizadosPozo0, mistery.probabilidadPozo0,
                                                       mistery.seed0Pozo0, mistery.seed1Pozo0, mistery.seed2Pozo0, mistery.seed3Pozo0, mistery.valorMininoSorteoPozo0,
                                                       ParametrosGlobales.obtenerValorInicialIntervaloHastaNuevoSorteo(0), ParametrosGlobales.obtenerValorFinalIntervaloHastaNuevoSorteo(0),
                                                       ParametrosGlobales.multiplicadorHastaNuevoSorteo);
                    sorteadorPozo1 = new SorteadorPozo(mistery, tragamonedasDelMistery, 1, mistery.sorteosRealizadosPozo1, mistery.probabilidadPozo1,
                                                       mistery.seed0Pozo1, mistery.seed1Pozo1, mistery.seed2Pozo1, mistery.seed3Pozo1, mistery.valorMininoSorteoPozo1,
                                                       ParametrosGlobales.obtenerValorInicialIntervaloHastaNuevoSorteo(1), ParametrosGlobales.obtenerValorFinalIntervaloHastaNuevoSorteo(1),
                                                       ParametrosGlobales.multiplicadorHastaNuevoSorteo);
                    sorteadorPozo2 = new SorteadorPozo(mistery, tragamonedasDelMistery, 2, mistery.sorteosRealizadosPozo2, mistery.probabilidadPozo2,
                                                       mistery.seed0Pozo2, mistery.seed1Pozo2, mistery.seed2Pozo2, mistery.seed3Pozo2, mistery.valorMininoSorteoPozo2,
                                                       ParametrosGlobales.obtenerValorInicialIntervaloHastaNuevoSorteo(2), ParametrosGlobales.obtenerValorFinalIntervaloHastaNuevoSorteo(2),
                                                       ParametrosGlobales.multiplicadorHastaNuevoSorteo);

                    sorteadorPozo0Thread = new Thread(sorteadorPozo0.DoWork);
                    sorteadorPozo0Thread.IsBackground = true;
                    sorteadorPozo0Thread.Start();
                    sorteadorPozo1Thread = new Thread(sorteadorPozo1.DoWork);
                    sorteadorPozo1Thread.IsBackground = true;
                    sorteadorPozo1Thread.Start();
                    sorteadorPozo2Thread = new Thread(sorteadorPozo2.DoWork);
                    sorteadorPozo2Thread.IsBackground = true;
                    sorteadorPozo2Thread.Start();
                    /*
                    //Comunicacion con S.Piper
                    misteryServer = new MisteryServer();
                    misteryServerThread = new Thread(misteryServer.DoWork);
                    misteryServerThread.Start();
                    */
                    log.Info("IP: " + ParametrosGlobales.MISTERY_IP + " - SERVICIO INICIADO!!!");
                }
                else { log.Info("IP: " + ParametrosGlobales.MISTERY_IP + ". Error en OnStart/setearMystery(). No se pudo inicializar el Servicio."); Stop(); }
            }
            catch (Exception ex) { log.Error("ERROR CRÍTICO AL INICIAR EL SERVICIO!!!! IP: " + ParametrosGlobales.MISTERY_IP, ex); Stop(); }
        }

        protected override void OnStop()
        {
            try
            {
                log.Info("DETENIENDO SERVICIO MYSTERY....IP: " + ParametrosGlobales.MISTERY_IP);
                if (misteryServerThread != null && misteryServerThread.ThreadState != System.Threading.ThreadState.Unstarted) //misteryServer.RequestStop();
                    if (misteryBackUpThread != null && misteryBackUpThread.ThreadState != System.Threading.ThreadState.Unstarted) misteryBackUp.RequestStop();
                if (misteryUDPServerThread != null && misteryUDPServerThread.ThreadState != System.Threading.ThreadState.Unstarted) misteryServerUDP.RequestStop();
                if (misteryUDPSenderThread != null && misteryUDPSenderThread.ThreadState != System.Threading.ThreadState.Unstarted) misterySender.RequestStop();
                if (misteryAuditorPremiosSenderThread != null && misteryAuditorPremiosSenderThread.ThreadState != System.Threading.ThreadState.Unstarted)
                    misteryAuditorPremiosSender.RequestStop();
                if (sorteadorPozo0Thread != null && sorteadorPozo0Thread.ThreadState != System.Threading.ThreadState.Unstarted) sorteadorPozo0.RequestStop();
                if (sorteadorPozo1Thread != null && sorteadorPozo1Thread.ThreadState != System.Threading.ThreadState.Unstarted) sorteadorPozo1.RequestStop();
                if (sorteadorPozo2Thread != null && sorteadorPozo2Thread.ThreadState != System.Threading.ThreadState.Unstarted) sorteadorPozo2.RequestStop();
                log.Warn("SERVICIO MYSTERY DETENIDO IP: " + ParametrosGlobales.MISTERY_IP);
                Thread.Sleep(5000);
            }
            catch (Exception e)
            { log.Error("Error al detener el Servicio IP: " + ParametrosGlobales.MISTERY_IP, e); }
        }
        #endregion Servicio

        #region setear Mystery
        private void registrarConfiguración()
        {
            log.Info("IRMisteryService Configuración Cargada. Mystery IP: " + ParametrosGlobales.MISTERY_IP);

            //Pozos
            log.Info("Pozo0: " + mistery.Pozo0);
            log.Info("Pozo1: " + mistery.Pozo1);
            log.Info("Pozo2: " + mistery.Pozo2);
            //Probabilidades
            log.Info("Probabilidad P0: " + mistery.probabilidadPozo0);
            log.Info("Probabilidad P1: " + mistery.probabilidadPozo1);
            log.Info("Probabilidad P2: " + mistery.probabilidadPozo2);
            //Iniciales de Pozos
            log.Info("Valor Inicial P0: " + mistery.inicialPozo0);
            log.Info("Valor Inicial P1: " + mistery.inicialPozo1);
            log.Info("Valor Inicial P2: " + mistery.inicialPozo2);
            //Umbrales minimos para sortear
            log.Info("Umbral Mínimo P0: " + mistery.valorMininoSorteoPozo0);
            log.Info("Umbral Mínimo P1: " + mistery.valorMininoSorteoPozo1);
            log.Info("Umbral Mínimo P2: " + mistery.valorMininoSorteoPozo2);
            //umbral para incremento de probabilidades
            log.Info("Umbral Incremento Prob. P0: " + mistery.umbralIncrementoProbabilidadPozo0);
            log.Info("Umbral Incremento Prob. P1: " + mistery.umbralIncrementoProbabilidadPozo1);
            log.Info("Umbral Incremento Prob. P2: " + mistery.umbralIncrementoProbabilidadPozo2);
            //incremento de probabilidad
            log.Info("Porcentaje de Incremento de Probabilidad del P0: " + mistery.porcentajeAIncrementarProbabilidadPozo0);
            log.Info("Porcentaje de Incremento de Probabilidad del P1: " + mistery.porcentajeAIncrementarProbabilidadPozo1);
            log.Info("Porcentaje de Incremento de Probabilidad del P2: " + mistery.porcentajeAIncrementarProbabilidadPozo2);
            //IP
            log.Info("IP Online:" + ParametrosGlobales.IP_ONLINE + " Puerto: " + ParametrosGlobales.PORT_MYSTERY + "  IP Backup:" + ParametrosGlobales.IP_BACKUP +
                "  Puerto:" + ParametrosGlobales.PORT_BACKUP + " IP ModuloBB:" + ParametrosGlobales.IP_MODULOBB2);
        }

        /// <summary>
        /// Carga desde el XML los datos globales para el Mystery
        /// </summary>
        /// <returns></returns>
        private Boolean cargarConfiguracion()
        {
            int paso = 0;
            ParametrosGlobales.conexiones = new Connections();
            try
            {
                //Parámetros globales
                //seteo el path del appConfig, desde el directorio dde se encuentra el ejecutable, creo.....
                ParametrosGlobales.pathArchivos = System.Reflection.Assembly.GetExecutingAssembly().Location.Substring(0, System.Reflection.Assembly.GetExecutingAssembly().Location.LastIndexOf('\\'));

                //Variable para determinar si hay un premio para pagar
                ParametrosGlobales.pagarPozo0 = false;
                ParametrosGlobales.pagarPozo1 = false;
                ParametrosGlobales.pagarPozo2 = false;
                ParametrosGlobales.MISTERY_IP = Convert.ToString(System.Configuration.ConfigurationManager.AppSettings["MYSTERYIP"].ToString());
                ParametrosGlobales.MISTERY_ID = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["ID"].ToString());
                ParametrosGlobales.MISTERY_NOMBRE = Convert.ToString(System.Configuration.ConfigurationManager.AppSettings["MYSTERYNOMBRE"].ToString());
                ParametrosGlobales.PORT_MYSTERY = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["SERVERPORT"].ToString());
                ParametrosGlobales.PORT_VIEWER = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["VIEWERPORT"].ToString());
                ParametrosGlobales.PORT_BACKUP = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["PORTBACKUP"].ToString());
                ParametrosGlobales.PORT_PREMIO = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["PORTPREMIO"].ToString());
                ParametrosGlobales.PORT_BB2 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["PORTBB2"].ToString());
                ParametrosGlobales.IP_VIEWER = System.Configuration.ConfigurationManager.AppSettings["VIEWERIP"].ToString();
                ParametrosGlobales.IP_BACKUP = System.Configuration.ConfigurationManager.AppSettings["BACKUPIP"].ToString();
                ParametrosGlobales.IP_ONLINE = System.Configuration.ConfigurationManager.AppSettings["ONLINEIP"].ToString();
                ParametrosGlobales.IP_MODULOBB2 = System.Configuration.ConfigurationManager.AppSettings["IPMODULOBB2"].ToString();

                paso++;//1
                //encriptacion activada
                ParametrosGlobales.encriptacionActivada = Convert.ToBoolean(System.Configuration.ConfigurationManager.AppSettings["ENCRIPTACIONACTIVADA"].ToString());
                //variable que indica si se usa el módulo BB2 para almacenar datos de configuración
                ParametrosGlobales.usarModuloBB2 = Convert.ToBoolean(System.Configuration.ConfigurationManager.AppSettings["MODULOBB2ACTIVADO"].ToString());
                //variable que indica si estamos en modo Test
                ParametrosGlobales.test = Convert.ToBoolean(System.Configuration.ConfigurationManager.AppSettings["MODOTEST"].ToString());
                //multiplicador para sortear de nuevo
                ParametrosGlobales.multiplicadorHastaNuevoSorteo = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["MULTIPLICADORHASTANUEVOSORTEO"].ToString());//60000

                paso++;//2
                //intervalos de tiempo para los sorteos
                ParametrosGlobales.valorInicialIntervaloHastaNuevoSorteoPozo0 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["VALORINICIALINTERVALOHASTANUEVOSORTEOP0"].ToString());
                ParametrosGlobales.valorFinalIntervaloHastaNuevoSorteoPozo0 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["VALORFINALINTERVALOHASTANUEVOSORTEOP0"].ToString());
                ParametrosGlobales.valorInicialIntervaloHastaNuevoSorteoPozo1 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["VALORINICIALINTERVALOHASTANUEVOSORTEOP1"].ToString());
                ParametrosGlobales.valorFinalIntervaloHastaNuevoSorteoPozo1 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["VALORFINALINTERVALOHASTANUEVOSORTEOP1"].ToString());
                ParametrosGlobales.valorInicialIntervaloHastaNuevoSorteoPozo2 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["VALORINICIALINTERVALOHASTANUEVOSORTEOP2"].ToString());
                ParametrosGlobales.valorFinalIntervaloHastaNuevoSorteoPozo2 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["VALORFINALINTERVALOHASTANUEVOSORTEOP2"].ToString());

                paso++;//3
                //tiempo para determinar si hay maquinas jugando
                ParametrosGlobales.MAXSEGUNDOS_ENJUEGO = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["MAXSEGENJUEGO"].ToString());
                //tiempo para determinar si hay maquinas en condiciones de sortear
                ParametrosGlobales.MAXSEGUNDOS_SORTEO = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["MAXSEGUNDOS_SORTEO"].ToString());
                //tiempo de espera entre escritura
                ParametrosGlobales.sleepEscrituraArchivo = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["SLEEPESCRITURADEARCHIVOS"].ToString());
                //tiempo de espera para enviar Premios al Servidor
                ParametrosGlobales.sleepEnvioDePremios = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["SLEEPENVIODEPREMIOS"].ToString());
                //Cantidad de paquetes descartados antes de registrar en el log
                ParametrosGlobales.cantidadDePaquetesADescartar = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["PAQUETESADESCARTAR"].ToString());
                //Porcentaje del apostado que se destina al Mistery
                ParametrosGlobales.porcentajeGeneral = Convert.ToDecimal(System.Configuration.ConfigurationManager.AppSettings["PORCENTAJEAPUESTAPARAPREMIO"].ToString());

                paso++;//4
                //Porcentajes que se destina a cada Pozos
                mistery.PorcentajePozo0 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["PORCENTAJEPOZO0"].ToString());
                mistery.PorcentajePozo1 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["PORCENTAJEPOZO1"].ToString());
                mistery.PorcentajePozo2 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["PORCENTAJEPOZO2"].ToString());

                //parámetros del Mystery
                //valores iniciales de los Pozos
                mistery.inicialPozo0 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["VALORINICIALPOZO0"].ToString());
                mistery.inicialPozo1 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["VALORINICIALPOZO1"].ToString());
                mistery.inicialPozo2 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["VALORINICIALPOZO2"].ToString());
                paso++;//5
                //Delta minimo para incremento
                mistery.DELTACOININMINIMOPOZO0 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["DELTACOININMINIMOPOZO0"].ToString());
                mistery.DELTACOININMINIMOPOZO1 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["DELTACOININMINIMOPOZO1"].ToString());
                mistery.DELTACOININMINIMOPOZO2 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["DELTACOININMINIMOPOZO2"].ToString());
                paso++;//6
                //guardo las probabilidades iniciales
                mistery.probabilidadInicialPozo0 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["PROBABILIDADINICIALPOZO0"].ToString());
                mistery.probabilidadInicialPozo1 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["PROBABILIDADINICIALPOZO1"].ToString());
                mistery.probabilidadInicialPozo2 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["PROBABILIDADINICIALPOZO2"].ToString());
                paso++;//7
                //guardo las probabilidades iniciales, dsps cargo desde el archivo la probabilidad actual del pozo
                mistery.probabilidadPozo0 = mistery.probabilidadInicialPozo0;
                mistery.probabilidadPozo1 = mistery.probabilidadInicialPozo1;
                mistery.probabilidadPozo2 = mistery.probabilidadInicialPozo2;
                paso++;//8
                //valor minimo para poder sortear
                mistery.valorMininoSorteoPozo0 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["VALORMINIMOSORTEOPOZO0"].ToString());
                mistery.valorMininoSorteoPozo1 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["VALORMINIMOSORTEOPOZO1"].ToString());
                mistery.valorMininoSorteoPozo2 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["VALORMINIMOSORTEOPOZO2"].ToString());
                paso++;//9
                //umbral para incremento de probabilidades
                mistery.umbralIncrementoProbabilidadPozo0 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["UMBRALMAXIMOINCREMENTOPROBABILIDADPOZO0"].ToString());
                mistery.umbralIncrementoProbabilidadPozo1 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["UMBRALMAXIMOINCREMENTOPROBABILIDADPOZO1"].ToString());
                mistery.umbralIncrementoProbabilidadPozo2 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["UMBRALMAXIMOINCREMENTOPROBABILIDADPOZO2"].ToString());
                paso++;//10
                //tope maximo de incremento de los Pozos
                mistery.umbralMaximoPozo0 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["TOPEMAXIMOINCREMENTOPOZO0"].ToString());
                mistery.umbralMaximoPozo1 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["TOPEMAXIMOINCREMENTOPOZO1"].ToString());
                mistery.umbralMaximoPozo2 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["TOPEMAXIMOINCREMENTOPOZO2"].ToString());
                paso++;//11
                //porcentaje en el cual debemos incrementar la probabilidad del pozo
                mistery.porcentajeAIncrementarProbabilidadPozo0 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["PORCENTAJEAINCREMENTARPROBABILIDADPOZO0"].ToString());
                mistery.porcentajeAIncrementarProbabilidadPozo1 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["PORCENTAJEAINCREMENTARPROBABILIDADPOZO1"].ToString());
                mistery.porcentajeAIncrementarProbabilidadPozo2 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["PORCENTAJEAINCREMENTARPROBABILIDADPOZO2"].ToString());
                paso++;//12
                //cargar Slots
                cargarSlot();
                paso++;//13
                //valores iniciales de las semillas
                mistery.seed0Pozo0 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["SEED0POZO0"].ToString());
                mistery.seed1Pozo0 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["SEED1POZO0"].ToString());
                mistery.seed2Pozo0 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["SEED2POZO0"].ToString());
                mistery.seed3Pozo0 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["SEED3POZO0"].ToString());
                paso++;//14
                mistery.seed0Pozo1 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["SEED0POZO1"].ToString());
                mistery.seed1Pozo1 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["SEED1POZO1"].ToString());
                mistery.seed2Pozo1 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["SEED2POZO1"].ToString());
                mistery.seed3Pozo1 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["SEED3POZO1"].ToString());
                paso++;//15
                mistery.seed0Pozo2 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["SEED0POZO2"].ToString());
                mistery.seed1Pozo2 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["SEED1POZO2"].ToString());
                mistery.seed2Pozo2 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["SEED2POZO2"].ToString());
                mistery.seed3Pozo2 = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["SEED3POZO2"].ToString());
                paso++;//16
                
                return crearConexiones();
            }
            catch (Exception ex) { log.Error("Error al cargar la configuración global, cargarConfiguracion(). Paso: " + paso, ex); return false; }
        }

        private Boolean crearConexiones()
        {
            //cargar Conexión Mystery
            Connection conexion = null;
            int i = 0;
            ConnectionInfo parametrosConexion = new ConnectionInfo();
            parametrosConexion.ConnectionName = Convert.ToString(System.Configuration.ConfigurationManager.AppSettings["ConexionNombre"].ToString());
            parametrosConexion.DataBaseName = Convert.ToString(System.Configuration.ConfigurationManager.AppSettings["DataBaseName"].ToString());
            parametrosConexion.Password = Utiles.DesencriptarTripleDES(Convert.FromBase64String(Convert.ToString(System.Configuration.ConfigurationManager.AppSettings["Password"].ToString())));
            parametrosConexion.UserName = Convert.ToString(System.Configuration.ConfigurationManager.AppSettings["UserName"].ToString());
            parametrosConexion.PersistSecurityInfo = Convert.ToBoolean(System.Configuration.ConfigurationManager.AppSettings["PersistSecurityInfo"].ToString());
            parametrosConexion.ServerName = Convert.ToString(System.Configuration.ConfigurationManager.AppSettings["ServerName"].ToString());
            ParametrosGlobales.SERVERMYSTERYIP = parametrosConexion.ServerName;
            parametrosConexion.Timeout = Convert.ToInt32(System.Configuration.ConfigurationManager.AppSettings["TimeOut"].ToString());
            parametrosConexion.BaseType = Convert.ToString(System.Configuration.ConfigurationManager.AppSettings["BaseType"].ToString());

            if (parametrosConexion.ConnectionName.ToUpper().Equals("MYSTERY"))
            { ParametrosGlobales.stringConexion = "Data Source=" + parametrosConexion.ServerName + ";Initial Catalog=" + parametrosConexion.DataBaseName + ";User ID=" + parametrosConexion.UserName + ";Password=" + parametrosConexion.Password + ""; }
                        
            while (i < 6)
            {
                switch (i)
                {
                    case (0): { parametrosConexion.ConnectionName = "SORTEADORPOZO0"; break; }
                    case (1): { parametrosConexion.ConnectionName = "SORTEADORPOZO1"; break; }
                    case (2): { parametrosConexion.ConnectionName = "SORTEADORPOZO2"; break; }
                    case (3): { parametrosConexion.ConnectionName = "BACKUP"; break; }
                    case (4): { parametrosConexion.ConnectionName = "AUDITORPREMIOSSENDER"; break; }
                    case (5): { parametrosConexion.ConnectionName = "MYSTERY"; break; }
                }
                conexion = new Connection(parametrosConexion);
                if (conexion.Conectar()) { ParametrosGlobales.conexiones.Add(conexion); log.Info("conexión a BD's: " + parametrosConexion.ConnectionName + " exitosa"); }
                else { log.Warn("Falló la conexión a BD's: " + parametrosConexion.ConnectionName + " en crearConexiones()."); return false; }
                i++;
            }

            return true;
        }

        //Recibe la cadena con los Slot y crea la lista de tragamonedas
        private void cargarSlot()
        {
            //formato: <add key="SLOT" value="310763|172.17.5.51|-555|1|10000,310000|172.17.6.666|-555|111|2222,...."/>
            String cadenaSlots = (System.Configuration.ConfigurationManager.AppSettings["SLOTS"].ToString()).Trim();
            if (cadenaSlots.Length > 0)
            {
                String[] listaSlot = cadenaSlots.Split(',');
                Tragamoneda t;
                String[] valores;
                foreach (String slot in listaSlot)
                {
                    valores = slot.Split('|');
                    //eventLog.WriteEntry("Slot IP: " + Convert.ToInt32(valores[0]) + " " + valores[1] + " " + -555 + " " + Convert.ToInt32(valores[2]) + " " + Convert.ToInt32(valores[3]));
                    t = new Tragamoneda(Convert.ToInt32(valores[0].Trim()), valores[1].Trim(), -555, Convert.ToInt32(valores[2].Trim()), Convert.ToInt32(valores[3].Trim()));
                    tragamonedasDelMistery.Add(t.Ip, t);
                }
            }
        }

        ///obtiene desde la BD's los valores del Mystery que se actualizan para mantener el Pozo
        private Boolean obtenerConfiguracionDesdeBD()
        {
            QueryEngine qe = new QueryEngine();
            qe.AddNames("@wIpControlador");
            qe.AddTypes(DbType.String);
            qe.AddValues(ParametrosGlobales.MISTERY_IP);
            try
            {
                qe.Query = "bs_obtenerConfiguracionMystery";
                DataSet ds = ParametrosGlobales.conexiones["MYSTERY"].DataSetExecuteReader(qe);

                if (ds != null && ds.Tables[0] != null && ds.Tables[0].Rows.Count == 1)
                {
                    //inicializar Mystery
                    return inicializarMystery(ds.Tables[0].Rows[0]);
                }
            }
            catch (Exception e) { log.Error("Error al obtenerDesdeBD(). IP: " + ParametrosGlobales.MISTERY_IP, e); }
            log.Error("Falló al obtener la Configuración Desde BD. IP: " + ParametrosGlobales.MISTERY_IP);
            return false;
        }

        /// carga los valores en el Mystery
        private Boolean inicializarMystery(DataRow fila)
        {
            try
            {
                //acumulado de los Pozos
                mistery.Pozo0 = Convert.ToInt32(fila["Pozo0"]);
                mistery.Pozo1 = Convert.ToInt32(fila["Pozo1"]);
                mistery.Pozo2 = Convert.ToInt32(fila["Pozo2"]);

                //cantidad de sorteos realizados
                mistery.sorteosRealizadosPozo0 = Convert.ToInt32(fila["sorteosRealizadosPozo0"]);
                mistery.sorteosRealizadosPozo1 = Convert.ToInt32(fila["sorteosRealizadosPozo1"]);
                mistery.sorteosRealizadosPozo2 = Convert.ToInt32(fila["sorteosRealizadosPozo2"]);

                //acumulado para incremento de probabilidad
                mistery.acumuladoParaIncrementoDeProbabilidadPozo0 = Convert.ToInt32(fila["acumuladoParaIncrementoDeProbabilidadPozo0"]);
                mistery.acumuladoParaIncrementoDeProbabilidadPozo1 = Convert.ToInt32(fila["acumuladoParaIncrementoDeProbabilidadPozo1"]);
                mistery.acumuladoParaIncrementoDeProbabilidadPozo2 = Convert.ToInt32(fila["acumuladoParaIncrementoDeProbabilidadPozo2"]);

                //multiplicador de incrementos de probabilidad
                mistery.multiplicadorIncrementosDeProbabilidadPozo0 = Convert.ToInt32(fila["multiplicadorIncrementosDeProbabilidadPozo0"]);
                mistery.multiplicadorIncrementosDeProbabilidadPozo1 = Convert.ToInt32(fila["multiplicadorIncrementosDeProbabilidadPozo1"]);
                mistery.multiplicadorIncrementosDeProbabilidadPozo2 = Convert.ToInt32(fila["multiplicadorIncrementosDeProbabilidadPozo2"]);

                //actualizo la probabilidad de cada Pozo, según la cantidad de incrementos de probabilidad
                mistery.probabilidadPozo0 = mistery.porcentajeAIncrementarProbabilidadPozo0 * mistery.probabilidadInicialPozo0 * mistery.multiplicadorIncrementosDeProbabilidadPozo0;
                mistery.probabilidadPozo1 = mistery.porcentajeAIncrementarProbabilidadPozo1 * mistery.probabilidadInicialPozo1 * mistery.multiplicadorIncrementosDeProbabilidadPozo1;
                mistery.probabilidadPozo2 = mistery.porcentajeAIncrementarProbabilidadPozo2 * mistery.probabilidadInicialPozo2 * mistery.multiplicadorIncrementosDeProbabilidadPozo2;
            }
            catch (Exception e) { log.Error("Error en inicializarMystery(). IP: " + ParametrosGlobales.MISTERY_IP, e); return false; }
            return true;
        }

        ///carga el Mystery con todos sus Parámetros
        private Boolean setearMystery()
        {
            return cargarConfiguracion() && obtenerConfiguracionDesdeBD();
        }

        #endregion setear Mystery

    }
}