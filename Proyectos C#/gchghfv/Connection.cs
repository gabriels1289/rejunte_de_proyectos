using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.Common;
using System.Data.SqlClient;
using System.Collections;
using System.Collections.Specialized;

namespace WService

{
    // STRUCT ConnectionInfo
    public struct ConnectionInfo
    {
        /// <summary>
        /// Estructura para mantener todos los datos de
        /// conexión a la Base de Datos.
        /// </summary>

        #region Variables

        public String BaseType; // SQL es el default
        public String ConnectionName;
        public String ServerName;
        public String DataBaseName;
        public Boolean PersistSecurityInfo;
        public Boolean IntegratedSecurity;
        public Int32 Timeout;
        public String UserName;
        public String Password;
        public String ParamKey; // prefijo de los parámetros (Ej: En MySQL deberá ser "?")

        #endregion

        #region Constructores

        public ConnectionInfo(String baseType, String paramKey, String connectionName, String serverName, String dataBaseName, Boolean persistSecurityInfo, Boolean integratedSecurity, Int32 timeout, String userName, String password)
        {
            BaseType = baseType;
            ParamKey = paramKey;
            ConnectionName = connectionName;
            ServerName = serverName;
            DataBaseName = dataBaseName;
            PersistSecurityInfo = persistSecurityInfo;
            IntegratedSecurity = integratedSecurity;
            Timeout = timeout;
            UserName = userName;
            Password = password;
        }

        #endregion

    }
    //--------------------------------------------------------------------------------------


    public class Connection
    {
        /// <summary>
        /// Clase para crear y mantener una conexion a una base de datos.
        /// MSSQL SERVER o MYSQL
        /// </summary>
        #region Variables Locales

        private String _stringConnection = String.Empty;
        private String _connectionName = String.Empty;
        private ConnectionInfo _connectionInfo;
        //  private DbTransaction _transaction = null;

        public DbConnection MiConexion = null;
        DbProviderFactory dataFactory = null;

        #endregion
        #region Constructores

        public Connection() { }

        public Connection(ConnectionInfo connInfo)
        {
            _connectionInfo = connInfo;
            this._connectionName = connInfo.ConnectionName;

            try
            {
                switch (connInfo.BaseType.ToUpper())
                {
                    case "MYSQL":
                        {
                            this.StringConnection = String.Format(@"Database={0};Data Source={1};User ID={2};Password={3}", connInfo.DataBaseName, connInfo.ServerName, connInfo.UserName, connInfo.Password);
                            connInfo.ParamKey = "?";
                            dataFactory = DbProviderFactories.GetFactory("MySql.Data.MySqlClient");
                            break;
                        }
                    default:
                        {
                            // MS SQL SERVER
                            this.StringConnection = String.Format(@"Persist Security Info={0};Integrated Security={1};Initial Catalog={2};Data Source={3};Connect Timeout={4};user id={5}; password={6}; ", connInfo.PersistSecurityInfo.ToString(), connInfo.IntegratedSecurity, connInfo.DataBaseName, connInfo.ServerName, connInfo.Timeout.ToString(), connInfo.UserName, connInfo.Password);
                            dataFactory = DbProviderFactories.GetFactory("System.Data.SqlClient");

                            break;
                        }
                }

                MiConexion = dataFactory.CreateConnection();
                MiConexion.ConnectionString = this.StringConnection;
            }
            catch (Exception ex)
            {
                throw ex;
            }


        }

        public Connection(String strConnection, String connectionName)
        {

            this._connectionName = connectionName;
            this.StringConnection = strConnection;

            try
            {
                // MS SQL SERVER
                dataFactory = DbProviderFactories.GetFactory("System.Data.SqlClient");

                MiConexion = dataFactory.CreateConnection();
                MiConexion.ConnectionString = this.StringConnection;
            }
            catch (Exception ex)
            {
                throw ex;
            }


        }
        #endregion

        #region Getters y Setters

        public String StringConnection
        {
            get
            {
                return _stringConnection;
            }
            set
            {
                _stringConnection = value;
            }
        }

        public String ConnectionName
        {
            get
            {
                return _connectionName;
            }
            set
            {
                _connectionName = value;
            }
        }

        public ConnectionInfo ConnectionInformation
        {
            get { return _connectionInfo; }
            set { _connectionInfo = value; }
        }

        public ConnectionState ConnectionState
        {
            get
            {
                if (this.MiConexion != null)
                    return this.MiConexion.State;
                else
                    return ConnectionState.Closed;
            }
        }

        #endregion

        #region Metodos de Instancia

        #region Apertura y cierre de la base de datos

        /// <summary>
        /// Intenta conectarse a la base de datos.
        /// </summary>
        /// <returns>Boolean</returns>
        public Boolean Conectar()
        {
            Boolean conectada = false;
            String ex1;
            try
            {
                if (MiConexion != null)
                {
                    if (MiConexion.State == ConnectionState.Open)
                    {
                        conectada = true;
                    }
                    else if (MiConexion.State == ConnectionState.Closed || MiConexion.State == ConnectionState.Broken)
                    {
                        MiConexion.Open();
                        conectada = true;
                    }
                }
            }
            catch (Exception ex)
            {
                ex1 = ex.ToString();
                //  throw ex;
            }

            return conectada;
        }

        /// <summary>
        /// Intenta desconectarse de la base de datos.
        /// </summary>
        /// <returns>Boolean</returns>
        public Boolean Desconectar()
        {
            Boolean desconectada = false;
            try
            {
                if (MiConexion != null && MiConexion.State == ConnectionState.Open)
                {
                    MiConexion.Close();
                    desconectada = true;
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }

            return desconectada;
        }

        #endregion


        #endregion

        #region SqlDataReader

        /// <summary>
        /// Ejecuta un consulta desde una cadena de texto. No implementa Transacciones.
        /// </summary>
        /// <param name="queryName">Cadena de texto que contiene el nombre del procedimiento almacenado.</param>
        /// <returns>DbDataReader</returns>
        public DbDataReader ExecuteReader(String queryName)
        {
            return ExecuteReader(new QueryEngine(queryName));
        }

        /// <summary>
        /// Ejecuta un consulta desde un objeto Query. Implementa Transacciones.
        /// </summary>
        /// <param name="query">Objeto Query que contiene a la consulta.</param>
        /// <returns>DbDataReader</returns>
        public DbDataReader ExecuteReader(QueryEngine query)
        {
            DbDataReader dbDataReader = null;
            DbCommand dbCommand = null;

            if (this.StringConnection == String.Empty)
                throw new Exception("Cadena de conexión vacía.");

            if (Conectar() == true)
            {
                try
                {
                    // seteo del command
                    dbCommand = dataFactory.CreateCommand();
                    dbCommand.Connection = MiConexion;
                    dbCommand.CommandType = CommandType.StoredProcedure;
                    dbCommand.CommandText = query.Query;
                    //dbCommand.Transaction = this._transaction;

                    if (query.parameterName.Count != 0 && query.parameterType.Count != 0 && query.parameterValue.Count != 0)
                    {
                        if ((query.parameterName.Count != query.parameterValue.Count) || (query.parameterName.Count != query.parameterType.Count) || (query.parameterValue.Count != query.parameterType.Count))
                        {
                            throw new Exception("Parameters (name, type and value) must be the same length!");
                        }
                        else
                        {
                            DbParameter dbParam = null;

                            // Carga de los parámetros
                            for (Int32 i = 0; i < query.parameterName.Count; i++)
                            {
                                dbParam = dataFactory.CreateParameter();

                                dbParam.ParameterName = ConnectionInformation.ParamKey + (String)query.parameterName[i];
                                if (query.parameterType.Count != 0)
                                    dbParam.DbType = (DbType)query.parameterType[i];
                                if (query.parameterValue.Count != 0)
                                    dbParam.Value = query.parameterValue[i];
                                if (query.parameterSize.Count != 0)
                                    dbParam.Size = (Int32)query.parameterSize[i];
                                //if (query.parameterDirection.Count != 0)
                                //    sqlParam.Direction = (System.Data.ParameterDirection)query.parameterDirection[i];

                                dbCommand.Parameters.Add(dbParam);
                            }
                        }
                    }
                    else
                    {
                        if (!(query.parameterName.Count == 0 && query.parameterType.Count == 0 && query.parameterValue.Count == 0))
                            throw new Exception("Must give Name, Type and Value to set parameters");
                    }

                    try
                    {
                        dbDataReader = dbCommand.ExecuteReader(CommandBehavior.CloseConnection);

                        // if (_transaction == null)
                        //     Desconectar();
                    }
                    catch (DbException)
                    {
                        // this.RollBackTransaction();
                        throw;
                    }
                }
                catch (Exception e)
                {
                    throw new Exception(e.Message);
                }
                finally
                {
                    dbCommand.Dispose();
                    dbCommand = null;
                    dbDataReader.Dispose();
                    dbDataReader = null;
                }

            }

            return dbDataReader;
        }

        /// <summary>
        /// Ejecuta una consulta tipo Scalar. No implementa transacciones.
        /// </summary>
        /// <param name="queryName">Cadena de texto que contiene el nombre del procedimiento almacenado.</param>
        /// <returns>Object</returns>
        public Object ExecuteScalar(String queryName)
        {
            return ExecuteScalar(new QueryEngine(queryName));
        }

        /// <summary>
        /// Ejecuta una consulta tipo Scalar. Implementa transacciones.
        /// </summary>
        /// <param name="query">Objeto tipo QueryEngine que contiene a la consulta.</param>
        /// <returns>Object</returns>
        public Object ExecuteScalar(QueryEngine query)
        {
            Object obj = null;
            DbCommand dbCommand = null;

            if (Conectar() == true)
            {
                try
                {
                    // seteo del command
                    dbCommand = dataFactory.CreateCommand();
                    dbCommand.Connection = MiConexion;
                    dbCommand.CommandType = CommandType.StoredProcedure;
                    dbCommand.CommandText = query.Query;
                    //                    dbCommand.Transaction = this._transaction;

                    if (query.parameterName.Count != 0 && query.parameterType.Count != 0 && query.parameterValue.Count != 0)
                    {
                        if ((query.parameterName.Count != query.parameterValue.Count) || (query.parameterName.Count != query.parameterType.Count) || (query.parameterValue.Count != query.parameterType.Count))
                        {
                            throw new Exception("Parameters (name, type and value) must be the same length!");
                        }
                        else
                        {
                            DbParameter dbParam = null;

                            // Carga de los parámetros
                            for (Int32 i = 0; i < query.parameterName.Count; i++)
                            {
                                dbParam = dataFactory.CreateParameter();

                                dbParam.ParameterName = ConnectionInformation.ParamKey + (String)query.parameterName[i];
                                if (query.parameterSize.Count != 0)
                                {
                                    dbParam.Size = (Int32)query.parameterSize[i];
                                }
                                dbParam.DbType = (DbType)query.parameterType[i];
                                dbParam.Value = query.parameterValue[i];
                                //sqlParam.Value=this.parameterDirection[i];

                                dbCommand.Parameters.Add(dbParam);
                            }
                        }
                    }
                    else
                    {
                        if (!(query.parameterName.Count == 0 && query.parameterType.Count == 0 && query.parameterValue.Count == 0))
                            throw new Exception("Must give Name, Type and Value to set parameters");
                    }

                    try
                    {
                        // Ejecución del SP
                        obj = dbCommand.ExecuteScalar();

                        //     if (_transaction == null)
                        //        Desconectar();
                    }
                    catch (DbException)
                    {
                        //    this.RollBackTransaction();
                        throw;
                    }
                }
                catch (Exception e)
                {
                    throw new Exception(e.Message);
                }
                finally
                {
                    dbCommand.Dispose();
                    dbCommand = null;
                }
            }

            return obj;
        }

        /// <summary>
        /// Ejecuta una consulta destinada por lo usual a actualización de datos (Insert/Update/Delete)
        /// Este método puede asignarse a una transacción.
        /// </summary>
        /// <param name="queryName">Cadena de texto que contiene el nombre del procedimiento almacenado.</param>
        /// <returns>Int32</returns>
        public Int32 ExecuteNonQuery(String queryName)
        {
            return ExecuteNonQuery(new QueryEngine(queryName));
        }

        /// <summary>
        /// Ejecuta una consulta destinada por lo usual a actualización de datos (Insert/Update/Delete)
        /// Este método puede asignarse a una transacción.
        /// </summary>
        /// <param name="query">Objeto QueryEngine que contiene a la consulta.</param>
        /// <returns>Int32</returns>
        public Int32 ExecuteNonQuery(QueryEngine query)
        {
            if (GetStringConnection() == String.Empty)
                throw new Exception("Error. Sin Conexión!!");
            
            DbCommand dbCommand = null;
            Int32 result = -1;

            if (Conectar() == true)
            {
                try
                {
                    // seteo del command
                    dbCommand = dataFactory.CreateCommand();
                    dbCommand.Connection = MiConexion;
                    dbCommand.CommandType = CommandType.StoredProcedure;
                    dbCommand.CommandText = query.Query;
                    //  dbCommand.Transaction = this._transaction;

                    if (query.parameterName.Count != 0 && query.parameterType.Count != 0 && query.parameterValue.Count != 0)
                    {
                        if ((query.parameterName.Count != query.parameterValue.Count))
                        {
                            throw new Exception("No coincide la cantidad de Parámetros de la consulta(nombres y valores)!");
                        }
                        else
                        {
                            DbParameter dbParam = null;

                            // Carga de los parámetros
                            for (Int32 i = 0; i < query.parameterName.Count; i++)
                            {
                                dbParam = dataFactory.CreateParameter();

                                dbParam.ParameterName = ConnectionInformation.ParamKey + (String)query.parameterName[i];
                                dbParam.Value = query.parameterValue[i];
                                //sqlParam.Value=this.parameterDirection[i];
                                if ((query.parameterSize.Count != 0) && ((query.parameterSize.Count - 1) <= i)) // la especificación de tamaño es opcional
                                {
                                    dbParam.Size = (Int32)query.parameterSize[i];
                                }
                                if ((query.parameterType.Count != 0) && ((query.parameterType.Count - 1) <= i)) // la especificación de tipo es opcional
                                {
                                    dbParam.DbType = (DbType)query.parameterType[i];
                                }

                                dbCommand.Parameters.Add(dbParam);
                            }
                        }
                    }
                    else
                    {
                        if (!(query.parameterName.Count == 0 && query.parameterType.Count == 0 && query.parameterValue.Count == 0))
                            throw new Exception("Must give Name, Type and Value to set parameters");
                    }

                    try
                    {
                        result = dbCommand.ExecuteNonQuery();

                        // if (_transaction == null)
                        //    Desconectar();
                    }
                    catch (DbException)
                    {
                        //  this.RollBackTransaction();
                        throw;
                    }

                }
                catch (Exception e)
                {
                    throw new Exception(e.Message);
                }
                finally
                {
                    dbCommand.Dispose();
                    dbCommand = null;
                }
            }

            return result;

        }

        #region Métodos para ejecutar un query sin SP (usar únicamente para pruebas)

        /// <summary>
        /// * Método no recomendado, usar sólo para pruebas *
        /// Ejecuta una consulta de lectura.
        /// </summary>
        /// <param name="queryString">Cadena de texto que contiene a la consulta.</param>
        /// <returns>DbDataReader</returns>
        public DbDataReader ExecuteReaderString(String queryString)
        {
            if (GetStringConnection() == String.Empty)
                throw new Exception("No connection!");

            DbCommand dbCommand = null;
            DbDataReader dbDataReader = null;

            if (Conectar() == true)
            {
                try
                {
                    // seteo del command
                    dbCommand = dataFactory.CreateCommand();
                    dbCommand.Connection = MiConexion;
                    dbCommand.CommandType = CommandType.Text;
                    dbCommand.CommandText = queryString;

                    dbDataReader = dbCommand.ExecuteReader(CommandBehavior.CloseConnection);
                }
                catch (Exception e)
                {
                    throw new Exception(e.Message);
                }
                finally
                {
                    dbCommand.Dispose();
                    dbCommand = null;
                    dbDataReader.Dispose();
                    dbDataReader = null;
                }
            }

            return dbDataReader;
        }

        /// <summary>
        /// * Método no recomendado, usar sólo para pruebas *
        /// Ejecuta una consulta de tipo Scalar.
        /// </summary>
        /// <param name="queryString">Cadena de texto que contiene a la consulta.</param>
        /// <returns>Object</returns>
        public Object ExecuteScalarString(String queryString)
        {
            if (GetStringConnection() == String.Empty)
                throw new Exception("No connection!");

            DbCommand dbCommand = null;
            Object obj = null;

            if (Conectar() == true)
            {
                try
                {
                    // seteo del command
                    dbCommand = dataFactory.CreateCommand();
                    dbCommand.Connection = MiConexion;
                    dbCommand.CommandType = CommandType.Text;
                    dbCommand.CommandText = queryString;

                    // Ejecución del SP
                    obj = dbCommand.ExecuteScalar();
                }
                catch (Exception e)
                {
                    throw new Exception(e.Message);
                }
                finally
                {
                    dbCommand = null;
                }
            }

            return obj;

        }

        /// <summary>
        /// * Método no recomendado, usar sólo para pruebas *
        /// Ejecuta una consulta destinada por lo usual a actualización de datos (Insert/Update/Delete)
        /// Este método puede asignarse a una transacción.
        /// </summary>
        /// <param name="queryString">Cadena de texto que contiene a la consulta.</param>
        /// <returns>Int32</returns>
        public Int32 ExecuteNonQueryString(String queryString)
        {
            if (GetStringConnection() == String.Empty)
                throw new Exception("No connection!");

            DbCommand dbCommand = null;
            Int32 result = -1;

            if (Conectar() == true)
            {
                try
                {
                    // seteo del command
                    dbCommand = dataFactory.CreateCommand();
                    dbCommand.Connection = MiConexion;
                    dbCommand.CommandType = CommandType.Text;
                    dbCommand.CommandText = queryString;
                    //  dbCommand.Transaction = this._transaction;

                    try
                    {
                        result = dbCommand.ExecuteNonQuery();
                        //      if (_transaction == null)
                        //           Desconectar();
                    }
                    catch (DBConcurrencyException)
                    {
                        //      this.RollBackTransaction();
                        throw;
                    }
                }
                catch (Exception e)
                {
                    throw new Exception(e.Message);
                }
                finally
                {
                    dbCommand.Dispose();
                    dbCommand = null;
                }
            }

            return result;

        }

        #endregion

        /// <summary>
        /// Devuelve el string de conexión; si no existe, lo arma con las propiedades de conexión
        /// </summary>
        /// <returns>String</returns>
        private String GetStringConnection()
        {
            String stringBuild = String.Empty;

            try
            {
                if (this.StringConnection == String.Empty)
                {
                    // Para generarlo, debe contener, al menos, nombre del servidor y BD
                    if (this.ConnectionInformation.ServerName != String.Empty && this.ConnectionInformation.DataBaseName != String.Empty)
                    {
                        switch (this.ConnectionInformation.BaseType.ToUpper())
                        {
                            case "MYSQL":
                                {
                                    stringBuild = String.Format(@"Database={0};Data Source={1};User ID={2};Password={3}", this.ConnectionInformation.DataBaseName, this.ConnectionInformation.ServerName, this.ConnectionInformation.UserName, this.ConnectionInformation.Password);
                                    break;
                                }
                            default:
                                {
                                    // SQL
                                    stringBuild = String.Format(@"Persist Security Info={0};Integrated Security={1};Data Source={2};Initial Catalog={3};User Id={4};Password={5};Connect Timeout={6}", this.ConnectionInformation.PersistSecurityInfo, this.ConnectionInformation.IntegratedSecurity, this.ConnectionInformation.ServerName, this.ConnectionInformation.DataBaseName, this.ConnectionInformation.UserName, this.ConnectionInformation.Password, this.ConnectionInformation.Timeout);
                                    break;
                                }
                        }
                    }
                }
                else
                {
                    stringBuild = this.StringConnection;
                }
            }
            catch
            {
                stringBuild = String.Empty;
            }

            return stringBuild;
        }

        #endregion

        #region DataSet

        /// <summary>
        /// Ejecuta una consulta de lectura a traves de una cadena de texto.
        /// </summary>
        /// <param name="query">Cadena de texto que contiene la consulta.</param>
        /// <returns>DataSet</returns>
        public DataSet DataSetExecuteReaderString(QueryEngine query)
        {
            return _dataSetExecuteReader(query, false);
        }

        /// <summary>
        /// Ejecuta una consulta de lectura, a través del nombre de un procedimiento almacenado.
        /// </summary>
        /// <param name="queryName">Cadena de texto que contiene el nombre del procedimiento almacenado.</param>
        /// <returns>DataSet</returns>
        public DataSet DataSetExecuteReader(String queryName)
        {
            return _dataSetExecuteReader(new QueryEngine(queryName), true);
        }

        /// <summary>
        /// Ejecuta una consulta de lectura, a través de un objeto QueryEngine, a un procedimiento almacenado.
        /// </summary>
        /// <param name="query">Objeto Query que contiene la consulta.</param>
        /// <returns>DataSet</returns>
        public DataSet DataSetExecuteReader(QueryEngine query)
        {
            return _dataSetExecuteReader(query, true);
        }

        /// <summary>
        /// Ejecuta la consulta.
        /// </summary>
        /// <param name="query">Objeto QueryEngine que contiene la consulta.</param>
        /// <param name="esSP">Determina si es un procedimiento almacenado.</param>
        /// <returns>DataSet</returns>
        private DataSet _dataSetExecuteReader(QueryEngine query, Boolean esSP)
        {
            if (GetStringConnection() == String.Empty)
                throw new Exception("No connection!");

            DbDataAdapter dbDataAdapter = null;
            DbCommand dbCommand = null;
            DataSet ds = null;

            if (Conectar() == true)
            {
                try
                {
                    dbDataAdapter = dataFactory.CreateDataAdapter();

                    // seteo del command
                    dbCommand = dataFactory.CreateCommand();
                    dbCommand.Connection = MiConexion;
                    if (esSP == true)
                        dbCommand.CommandType = CommandType.StoredProcedure;
                    else
                        dbCommand.CommandType = CommandType.Text;
                    dbCommand.CommandText = query.Query;

                    if (query.parameterName.Count != 0 && query.parameterType.Count != 0 && query.parameterValue.Count != 0)
                    {
                        if ((query.parameterName.Count != query.parameterValue.Count) || (query.parameterName.Count != query.parameterType.Count) || (query.parameterValue.Count != query.parameterType.Count))
                        {
                            throw new Exception("Parameters (name, type and value) must be the same length!");
                        }
                        else
                        {
                            DbParameter dbParam = null;

                            // Carga de los parámetros
                            for (Int32 i = 0; i < query.parameterName.Count; i++)
                            {
                                dbParam = dataFactory.CreateParameter();

                                dbParam.ParameterName = ConnectionInformation.ParamKey + (String)query.parameterName[i];
                                if (query.parameterType.Count != 0)
                                    dbParam.DbType = (DbType)query.parameterType[i];
                                if (query.parameterValue.Count != 0)
                                    dbParam.Value = query.parameterValue[i];
                                if (query.parameterSize.Count != 0)
                                    dbParam.Size = (Int32)query.parameterSize[i];
                                //if (query.parameterDirection.Count != 0)
                                //    sqlParam.Direction = (System.Data.ParameterDirection)query.parameterDirection[i];

                                dbCommand.Parameters.Add(dbParam);
                            }
                        }
                    }
                    else
                    {
                        if (!(query.parameterName.Count == 0 && query.parameterType.Count == 0 && query.parameterValue.Count == 0))
                            throw new Exception("Must give Name, Type and Value to set parameters");
                    }

                    dbDataAdapter.SelectCommand = dbCommand;
                    ds = new DataSet();
                    dbDataAdapter.Fill(ds, "tabla");
                    Desconectar();
                }
                catch (Exception e)
                {
                    throw new Exception(e.Message);
                }
                finally
                {
                    dbCommand.Dispose();
                    dbCommand = null;
                    dbDataAdapter.Dispose();
                    dbDataAdapter = null;
                }
            }

            return ds;

        }

        #endregion

        #region Testing

        /// <summary>
        /// Intenta establecer la conexión a la base de datos. Luego, la cierra.
        /// </summary>
        /// <returns>Boolean</returns>
        public Boolean ProbarConexion()
        {
            try
            {
                if (GetStringConnection() == String.Empty)
                    throw new Exception("No connection!");

                this.MiConexion = dataFactory.CreateConnection();
                MiConexion.ConnectionString = this.StringConnection;
                //System.Data.SqlClient.SqlConnection conn = new SqlConnection(this.StringConnection);
                //conn.Open();
                MiConexion.Open();
                MiConexion.Close();
                return true;
            }
            catch (Exception ex)
            {
                throw new Exception(ex.Message);
            }

        }

        #endregion



    }
    //--------------------------------------------------------------------------------------
    /// <summary>
    /// Clase destinada a crear un objeto Query para realizar la consulta a la base de datos.
    /// </summary>
    public class QueryEngine
    {

        #region Variables

        private System.Collections.ArrayList _parameterName = new ArrayList(); //String
        private System.Collections.ArrayList _parameterValue = new ArrayList(); // Object
        private System.Collections.ArrayList _parameterType = new ArrayList(); // System.Data.SqlDbType

        private System.Collections.ArrayList _parameterSize = new ArrayList(); //Int32
        private System.Collections.ArrayList _parameterDirection = new ArrayList(); //System.Data.ParameterDirection
        private System.Collections.ArrayList _parameterScale = new ArrayList(); //Byte
        private System.Collections.ArrayList _parameterPrecision = new ArrayList(); //Byte
        private String _query;

        #endregion

        #region Constructores

        public QueryEngine()
        {
            // Default Constructor
        }

        public QueryEngine(String QueryName)
        {
            this._query = QueryName;
        }

        #endregion

        #region Propiedades

        public System.Collections.ArrayList parameterName //String
        {
            get
            {
                return _parameterName;
            }
            set
            {
                _parameterName = value;
            }
        }

        public System.Collections.ArrayList parameterType //System.Data.SqlDbType
        {
            get
            {
                return _parameterType;
            }
            set
            {
                _parameterType = value;
            }
        }

        public System.Collections.ArrayList parameterValue //Object
        {
            get
            {
                return _parameterValue;
            }
            set
            {
                _parameterValue = value;
            }
        }

        public System.Collections.ArrayList parameterSize //Int32
        {
            get
            {
                return _parameterSize;
            }
            set
            {
                _parameterSize = value;
            }
        }

        public System.Collections.ArrayList parameterDirection //System.Data.ParameterDirection[]
        {
            get
            {
                return _parameterDirection;
            }
            set
            {
                _parameterDirection = value;
            }
        }

        public System.Collections.ArrayList parameterScale // Byte
        {
            get
            {
                return _parameterScale;
            }
            set
            {
                _parameterScale = value;
            }
        }

        public System.Collections.ArrayList parameterPrecision //Byte
        {
            get
            {
                return _parameterPrecision;
            }
            set
            {
                _parameterPrecision = value;
            }
        }

        public String Query
        {
            get
            {
                return _query;
            }
            set
            {
                _query = value;
            }
        }

        #endregion

        #region Métodos de instancia

        /// <summary>
        /// Agrega los nombres de los parámetros, separados por coma.
        /// </summary>
        /// <param name="parameterNameList">Nombre de parámetro.</param>
        public void AddNames(params String[] parameterNameList)
        {
            foreach (String pnl in parameterNameList)
                this._parameterName.Add(pnl);
        }

        /// <summary>
        /// Agrega los tipos de parámtros, separados por coma.
        /// </summary>
        /// <param name="parameterTypeList">Tipo de parámetro.</param>
        public void AddTypes(params DbType[] parameterTypeList)
        {
            foreach (DbType ptl in parameterTypeList)
                this._parameterType.Add(ptl);
        }

        /// <summary>
        /// Agrega los valores de los parámetros, separados por coma.
        /// </summary>
        /// <param name="parameterValueList">Valor del parámetro.</param>
        public void AddValues(params Object[] parameterValueList)
        {
            foreach (Object pvl in parameterValueList)
                this._parameterValue.Add(pvl);
        }

        /// <summary>
        /// Elimina los valores conservando la estructura, para su reutilización.
        /// </summary>
        public void RemoveValues()
        {
            this._parameterValue = new System.Collections.ArrayList();
        }

        /// <summary>
        /// Agrega los tamaños de los parámetros, separados por coma. Opcional.
        /// </summary>
        /// <param name="parameterSizeList">Tamaño del parámetro.</param>
        public void AddSizes(params Int32[] parameterSizeList)
        {
            foreach (Object psl in parameterSizeList)
                this._parameterSize.Add(psl);
        }

        #endregion



    }

    //-----------------------------------------------------------------------------------------

    public class Connections : CollectionBase
    {

        #region Métodos heredados

        public Connection this[int index]
        {
            get
            {
                return ((Connection)List[index]);
            }
            set
            {
                List[index] = value;
            }
        }

        public Connection this[String connectionName]
        {
            get
            {
                foreach (Connection aConnection in this)
                {
                    if (aConnection.ConnectionName == connectionName)
                        return aConnection;
                }
                return new Connection();
            }
            set
            {
                this[connectionName] = value;
            }
        }

        public int Add(Connection value)
        {
            Boolean prevent = false;
            foreach (Connection con in this)
            {
                if (con.ConnectionName == value.ConnectionName)
                    prevent = true;
            }
            if (prevent == true)
                throw new Exception("connectionName must be unique!");
            else
                return (List.Add(value));
        }

        public int IndexOf(Connection value)
        {
            return (List.IndexOf(value));
        }

        public int IndexOf(String connectionName)
        {
            return (List.IndexOf(this[connectionName]));
        }

        public void Insert(int index, Connection value)
        {
            List.Insert(index, value);
        }

        public void Remove(Connection value)
        {
            List.Remove(value);
        }

        public void Remove(String connectionName)
        {
            if (this.Contains(connectionName))
                List.Remove(this[connectionName]);
        }

        public bool Contains(Connection value)
        {
            return (List.Contains(value));
        }

        public bool Contains(String connectionName)
        {
            return (this[connectionName].ConnectionName == connectionName);
        }

        #endregion

    }

}


