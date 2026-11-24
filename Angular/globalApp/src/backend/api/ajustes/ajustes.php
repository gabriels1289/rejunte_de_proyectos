<?php
$ARCHIVO = 'ajustes/ajustes.php';
// clase para manejar las Sql relacionadas con evaluaciones
// TODO: colocar un esquema de logs y cerrar la conexion en algun lugar fuera de la clase
class Ajustes
{
  private $db = null; // propiedad para la conexion a la DB
  private $requestMethod = null; // propiedad para el metodo de peticion http (GET, POST, ...)

  private $data = null;  // propiedad para obtener los datos de entrada

  private $idusuario = null;
  private $nuevopassword = null;
  private $viejopassword = null;
  private $newpassword = null;
  private $oldpassword = null;
  private $cantidad = null; // cantidad de registros logs
  private $logger;
  
  /**
   * El constructor espera un objeto de tipo DatabaseConnector
   * y trata de inicializar todos los parametros que se puedan haber enviado
   */
  public function __construct($db, $requestMethod)
  {
    $this->db = $db;
    // en funcion del metodo solicitado obtengo e inicializo las propiedades para las sql
    $this->requestMethod = $requestMethod;
    // creo que salvo el metodo post, toda la info va a $_GET
    if ($this->requestMethod == 'POST') {
      //$this->data = $_POST['data'];
      $this->data = json_decode($_POST['data'], true);
    } else if ($this->requestMethod == 'PUT') {
      $this->data = json_decode(file_get_contents("php://input"), true);
    } else {
      $this->data = $_GET;
    }

    // ChromePhp::log(json_decode(file_get_contents("php://input")));
    $this->idusuario = (isset($this->data['idusuario'])) ? filter_var($this->data['idusuario'], FILTER_VALIDATE_INT) : null;
    $this->nuevopassword = (isset($this->data['newpassword'])) ? $this->data['newpassword'] : null;
    $this->viejopassword = (isset($this->data['oldpassword'])) ? $this->data['oldpassword'] : null;
    $this->newpassword = (isset($this->data['newpassword'])) ? sha1($this->data['newpassword']) : null;
    $this->oldpassword = (isset($this->data['oldpassword'])) ? sha1($this->data['oldpassword']) : null;
    $this->cantidad = (isset($this->data['cantidad'])) ? filter_var($this->data['cantidad'], FILTER_VALIDATE_INT) : null;

    $this->logger = new Logger();
  }

  /**
   * Cambia el password de un usuario
   * 
   * @param $idusuario
   * @return array con los mensajes.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function setPassword() //idusuario, newpassword, oldpassword, nuevopassword, viejopassword
  {
    try {
      $sql = "SELECT password FROM empleados WHERE id='$this->idusuario'";

      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) == 1 && ($consulta[0]["password"]==$this->oldpassword)) {
        $sql = "UPDATE empleados SET password=:password WHERE id=:id";
        $stmt = $this->db->prepare($sql);
        $stmt->bindParam(':id', $this->idusuario);
        $stmt->bindParam(':password', $this->newpassword);

        if ($stmt->execute())
        {
          $this->logger->info("el empleado: " . $this->idusuario . " cambio su contraseña", $this->ARCHIVO, __LINE__);
          http_response_code(OK);
          return "ok";
        }else {
          $this->logger->fatal("hubo un fallo en la consulta", $this->ARCHIVO, __LINE__);
          http_response_code(INTERNAL_SERVER_ERROR);
          return "fallo consulta";
        }
      }
      else {
        $this->logger->warn("la contraseña actual ingresada no es correcta", $this->ARCHIVO, __LINE__);
        http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
        return "La contraseña actual ingresada no es correcta";
      }
    } catch (\PDOException $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }    
  }

  /**
   * Cambia el password de un usuario
   * 
   * @param $idusuario
   * @return array con los mensajes.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getLogs()
  {
    try {
      $datos = array();
      $sql = "SELECT l.id,
                CASE l.nivel
                  WHEN 0 THEN 'FATAL'
                WHEN 1 THEN 'ERROR'
                  WHEN 2 THEN 'WARN'
                  WHEN 3 THEN 'INFO'
                  WHEN 4 THEN 'DEBUG'
                  WHEN 5 THEN 'TRACE'
                END AS nivel,
                l.usuario, IFNULL(concat(e.apellido,', ',e.nombre), 'USUARIO NO REGISTRADO') AS nombre, l.mensaje, l.fecha, l.archivo, l.linea
              FROM (SELECT * FROM logs ORDER BY fecha DESC LIMIT $this->cantidad) AS l LEFT JOIN empleados e
              ON l.usuario=e.username
              ORDER BY id DESC";

      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
        array_push($datos, $consulta);

        //devuelve la cantidad de empleados no deslogueados
				$sql = "SELECT count(*) AS noDeslogueados 
                FROM empleados 
                WHERE token!='LOGGED OUT' AND activo='SI'";
        
        $consulta = $this->db->query($sql);
        $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);

        if (count($consulta) >= 1 && ($consulta[0]["noDeslogueados"]!=null)) {
          array_push($datos, $consulta);
				
					//devuelve la cantidad de empleados activos
					$sql = "SELECT count(*) AS totalActivos 
								FROM empleados 
								WHERE activo='SI'";

          $consulta = $this->db->query($sql);
          $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
          if (count($consulta) >= 1 && ($consulta[0]["totalActivos"]!=null)) {
            array_push($datos, $consulta);
            http_response_code(OK);
            return $datos;
          } else {
            $this->logger->fatal("hubo un fallo en la consulta", $this->ARCHIVO, __LINE__);
            http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
            return "fallo consulta";
          }
        } else {
          $this->logger->fatal("hubo un fallo en la consulta", $this->ARCHIVO, __LINE__);
          http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
          return "fallo consulta";
        }
      }
      else {
        $this->logger->fatal("hubo un fallo en la consulta", $this->ARCHIVO, __LINE__);
        http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
        return "fallo consulta";
      }
    } catch (\PDOException $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }    
  }
}
?>