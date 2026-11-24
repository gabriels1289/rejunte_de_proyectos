using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.ServiceProcess;
using System.Text;
using IRCASHDatabaseEngine;
using System.Net.Sockets;
using System.Net;
using System.Threading;
using System.IO;
using IRCASHDATAMODEL.Tragamonedas;
using System.Xml;
using IRCASHDATAMODEL.Utiles;
using IRCASHDATAMODEL.Clientes;
using IRCASHDATAMODEL.Tarjetas;
using IRCASHDATAMODEL.Cajas;
using IRCASHDATAMODEL.PromocionesPlayerTracking;
using log4net;

namespace IREcoService
{
   
     
    public partial class IREco : ServiceBase
    {
        // Puertos con los que Trabaja el Sistema

        private static Int32 portEco = 1200;
        private static Int32 portUI = 1550;
       
        // Conexiones a BD
        private Connection oMiConexionEco;
        private Connection oMiConexionUI;
        private Connection oMiConexionTragamonedas;

        private List<Tragamoneda> tragamonedas;
        private Hashtable listaTragamonedas = new Hashtable();
        private Hashtable listaTarjetasJugando = new Hashtable();
        private const int DeltaMaximoValido = 1000000;
        private string ConfiguracionNombre = "";
        private Decimal CargaMaximaTarjetaVendedor = 0;
        private Decimal Iva = 0;
        private Decimal UsoDeInstalaciones = 0;
        private Int32 CajaMovilID;
        private Int32 SegundosReingresoTarjeta;
        private Decimal EquivalenteMillasPesos;
        private Boolean CashlessActivo;
        private Boolean encriptacionActivada;
        private Boolean ProcesadorContadoresMultiplesHilos;

        private Int32 SegundosDemoraInicioServicio=15;
        private ConnectionInfo parametrosConexionTragamonedas;
        UserInterface uiAdmin;
        EcoAdmin ecoAdmin;
        Thread uiThread;

        Semaforo semaforo= new Semaforo();
        Semaforo semaforoContadores = new Semaforo();
        
        // Hilos de la Aplicación
       Thread ecoThread;
       

        private static ILog logIREco;
        public IREco()
        {
            InitializeComponent();
            
        }

        protected override void OnStart(string[] args)
        {
            //-- Pausa para darle tiempo a que Inicie SQL SERVER
            Thread.Sleep(SegundosDemoraInicioServicio*1000);

            logIREco = LogManager.GetLogger(typeof(IREco));
            log4net.Config.XmlConfigurator.Configure();


            logIREco.Info("Iniciando Servicio IREco...");
           
            //-- Conexion a BD para manejo de Contadores                       
            cargarConfiguracionXML("C:\\ConfigColector.xml");

            
           
            // Si conecto a BD correctamente inicio los servicios
            // Reintenta hasta tener exito
            Boolean conectado=false;
            while(!conectado) 
            {
                if (oMiConexionEco.Conectar())
                     {conectado=true;}
                else
                {
                    logIREco.Info("ERROR Iniciando Servicio IREco - BD No Conectada.");
                    Thread.Sleep(10000);
                }
            }


                logIREco.Info("BD Conectadas!!");
                // Lee la configuración desde un .xml
                cargarConfiguracion(oMiConexionTragamonedas);      

                // Carga las tragamonedas activas
                tragamonedas = Tragamoneda.obtenerTodasActivas(oMiConexionTragamonedas);
                foreach (Tragamoneda t in tragamonedas)
                {
                    try
                    {
                        if (t.tragamonedaActivo)
                        {
                            listaTragamonedas.Add(t.tragamonedaIP, t);                          
                        }
                    }
                    catch (Exception es)
                    {
                        logIREco.Warn("ERROR - Creando hashtable IP REPETIDO.", es);
                    }
                }            

                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                logIREco.Info("Iniciando Hilo ECO...");
                // Inicia el servicio ECO
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                try
                {
                    ecoAdmin = new EcoAdmin(portEco, listaTragamonedas, semaforo, CargaMaximaTarjetaVendedor, oMiConexionEco, encriptacionActivada, ProcesadorContadoresMultiplesHilos, this);
                    ecoThread = new Thread(ecoAdmin.DoWork);
                    ecoThread.Start();
                }
                catch (Exception e)
                {
                    logIREco.Warn("ERROR - " + e.ToString(), e);
                }

                ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //logIRCollector.Info("Iniciando Hilo User Interface...");
                // Inicia el servicio
                try
                {
                    uiAdmin = new UserInterface(oMiConexionUI, listaTragamonedas, semaforo, portUI);
                    uiThread = new Thread(uiAdmin.DoWork);
                    uiThread.Start();
                    //  logIRCollector.Info("Hilo User Interface iniciado!");
                }
                catch (Exception e)
                {
                    logIREco.Warn("ERROR - " + e.ToString(), e);
                }

                Thread.Sleep(3000);
                logIREco.Info("Servicio IREco Iniciado!");

        }

        /// <summary>
        /// Carga la configuracion de una tabla en la BD
        /// </summary>
        /// <param name="oMiConexion"></param>
        private void cargarConfiguracion(Connection oMiConexion)
        {
            try
            {
                logIREco.Info("Cargando Parametros de la aplicación desde BD...");
                QueryEngine qe = new QueryEngine();

                qe.AddNames("@wConfiguracionNombre");
                qe.AddTypes(DbType.String);
                qe.AddValues(ConfiguracionNombre);

                qe.Query = "sp_ObtenerConfiguracionByName";
                DataSet ds = null;
                ds = oMiConexion.DataSetExecuteReader(qe);
                if (ds.Tables[0].Rows.Count > 0)
                {
                    // Configuracion Encontrada
                    asignarDatosConfiguracion(ds.Tables[0].Rows[0]);
                }

            }
            catch (Exception ex)
            {
                logIREco.Warn("Error. cargarConfiguracion" + ex.Message, ex);
            }
        }

        private void asignarDatosConfiguracion(DataRow fila)
        {
            try
            {
                Iva = Convert.ToDecimal(fila["IVA"]);
                UsoDeInstalaciones = Convert.ToDecimal(fila["UsoDeInstalaciones"]);
                CargaMaximaTarjetaVendedor = Convert.ToDecimal(fila["CargaMaximaTarjetaVendedor"]);
                CajaMovilID = Convert.ToInt32(fila["CajaMovilID"]);
                SegundosReingresoTarjeta = Convert.ToInt32(fila["SegundosReingresoTarjeta"]);
                EquivalenteMillasPesos = Convert.ToDecimal(fila["EquivalenteMillasPesos"]);
                CashlessActivo = Convert.ToBoolean(fila["CashlessActivo"]);
                
                try
                {
                    ProcesadorContadoresMultiplesHilos = Convert.ToBoolean(fila["ProcesadorContadoresMultiplesHilos"]);
                }
                catch (Exception ex)
                {
                    ProcesadorContadoresMultiplesHilos = false;
                }               
            }
            catch (Exception ex)
            {
                logIREco.Warn("Error. Al asignarDatosConfiguracion" + ex.Message, ex);
            }
        }

        /// <summary>
        /// Metodo ejecutado cuando se finaliza el hilo.
        /// Al finalizar desconecta las conexiones a BD.
        /// </summary>
        protected override void OnStop()
        {
            logIREco.Warn("Servicio IREco Detenido!!");          
            oMiConexionTragamonedas.Desconectar();          
            
        }

        /// <summary>
        /// Carga la configuracion desde un archivo XML.
        /// Devuelve true si pudo cargarla con exito
        /// </summary>
        /// <param name="path"></param>
        /// <returns></returns>
        public Boolean cargarConfiguracionXML(String path)
        {
            logIREco.Info("Cargando archivo de configuracion xml...");
            Boolean resultado = true;
            try
            {
                XmlDocument xDoc = new XmlDocument();

                //La ruta del documento XML permite rutas relativas 
                //respecto del ejecutable!

                xDoc.Load(path);

                XmlNodeList xmlConexiones = xDoc.GetElementsByTagName("BaseDeDatos");

                int i = 0;
                XmlNodeList listaDeConexiones = ((XmlElement)xmlConexiones[0]).GetElementsByTagName("Conexion");
                foreach (XmlElement nodoConexion in listaDeConexiones)
                {

                    XmlNodeList nConexionNombre = nodoConexion.GetElementsByTagName("ConexionNombre");
                    XmlNodeList nDataBaseName = nodoConexion.GetElementsByTagName("DataBaseName");
                    XmlNodeList nPassword = nodoConexion.GetElementsByTagName("Password");
                    XmlNodeList nUserName = nodoConexion.GetElementsByTagName("UserName");
                    XmlNodeList nPersistSecurityInfo = nodoConexion.GetElementsByTagName("PersistSecurityInfo");
                    XmlNodeList nServerName = nodoConexion.GetElementsByTagName("ServerName");
                    XmlNodeList nTimeOut = nodoConexion.GetElementsByTagName("Timeout");
                    XmlNodeList nBaseType = nodoConexion.GetElementsByTagName("BaseType");

                    ConnectionInfo parametrosConexion = new ConnectionInfo();

                    parametrosConexion.ConnectionName = nConexionNombre[0].InnerText;
                    parametrosConexion.DataBaseName = nDataBaseName[0].InnerText;
                    parametrosConexion.Password = Encriptacion.DesencriptarTripleDES(Convert.FromBase64String(nPassword[0].InnerText));
                    parametrosConexion.UserName = nUserName[0].InnerText;
                    parametrosConexion.PersistSecurityInfo = Convert.ToBoolean(nPersistSecurityInfo[0].InnerText);
                    parametrosConexion.ServerName = nServerName[0].InnerText; ;
                    parametrosConexion.Timeout = Convert.ToInt32(nTimeOut[0].InnerText);
                    parametrosConexion.BaseType = nBaseType[0].InnerText;

                    Connection conexion = new Connection(parametrosConexion);

                    if (parametrosConexion.DataBaseName.Equals("IRCASHONLINE"))
                    {
                        oMiConexionTragamonedas = conexion;
                        parametrosConexionTragamonedas = new ConnectionInfo();
                        parametrosConexionTragamonedas.DataBaseName = parametrosConexion.DataBaseName;
                        parametrosConexionTragamonedas.Password = parametrosConexion.Password;
                        parametrosConexionTragamonedas.UserName = parametrosConexion.UserName;
                        parametrosConexionTragamonedas.PersistSecurityInfo = parametrosConexion.PersistSecurityInfo;
                        parametrosConexionTragamonedas.ServerName = parametrosConexion.ServerName;
                        parametrosConexionTragamonedas.Timeout = 30;
                        parametrosConexionTragamonedas.BaseType = parametrosConexion.BaseType;
                        parametrosConexionTragamonedas.DataBaseName = parametrosConexion.DataBaseName;                       
                        oMiConexionEco = new Connection(parametrosConexion);


                    }                  
                    i++;
                }


                XmlNodeList xmlParametrosConfiguracion = xDoc.GetElementsByTagName("Configuracion");
                XmlNodeList listaDeParametrosConfiguracion = ((XmlElement)xmlParametrosConfiguracion[0]).GetElementsByTagName("Parametros");

                foreach (XmlElement nodoParametro in listaDeParametrosConfiguracion)
                {
                    XmlNodeList nConfiguracionNombre = nodoParametro.GetElementsByTagName("nombre");
                    ConfiguracionNombre = Convert.ToString(nConfiguracionNombre[0].InnerText);

                    XmlNodeList nConfiguracionSegundosDemoraInicio = nodoParametro.GetElementsByTagName("segundosdemorainicio");
                    SegundosDemoraInicioServicio = Convert.ToInt32(nConfiguracionSegundosDemoraInicio[0].InnerText);

                    XmlNodeList nConfiguracionEncriptacion = nodoParametro.GetElementsByTagName("encriptacion");
                    encriptacionActivada = Convert.ToBoolean(nConfiguracionEncriptacion[0].InnerText);
                    logIREco.Info("ENCRIPTACION ACTIVADA:" + encriptacionActivada);

                }

               
            }
            catch (Exception ex)
            {
                logIREco.Warn("Error cargarConfiguracionXML" + ex.Message,ex);
                resultado = false;
            }
            return resultado;
        }
       
    }



   
}
