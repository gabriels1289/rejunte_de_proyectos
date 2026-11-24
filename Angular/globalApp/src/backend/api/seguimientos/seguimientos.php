<?php
$ARCHIVO = 'seguimientos/seguimientos.php';
// clase para manejar las Sql relacionadas con seguimientos
// TODO: colocar un esquema de logs y cerrar la conexion en algun lugar fuera de la clase
class Seguimientos
{
  private $db = null; // propiedad para la conexion a la DB
  private $requestMethod = null; // propiedad para el metodo de peticion http (GET, POST, ...)

  private $data = null;  // propiedad para obtener los datos de entrada

  private $idseguimiento = null;
  private $idempleado = null;  
  private $listahijos = null;
  private $idacceso = null;
  private	$tipoobservacion = null;
  private	$calificacion = null;
  private	$idusuario = null;
  private	$sector = null;
  private	$detalle = null;
  private $cuerpo = null;
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
      $this->data = json_decode($_POST['data'], true);
    } else {
      $this->data = $_GET;
    }

    // ChromePhp::log(json_decode(file_get_contents("php://input")));
    $this->idseguimiento = (isset($this->data['idseguimiento'])) ? $this->data['idseguimiento'] : null;
    $this->idempleado = (isset($this->data['idempleado'])) ? $this->data['idempleado'] : null;
    $this->listahijos = (isset($this->data['listahijos'])) ? $this->data['listahijos'] : null;
    $this->idacceso = (isset($this->data['idacceso'])) ? $this->data['idacceso'] : null;
    $this->tipoobservacion = (isset($this->data['tipoobservacion'])) ? $this->data['tipoobservacion'] : null;
    $this->calificacion = (isset($this->data['calificacion'])) ? $this->data['calificacion'] : null;
    $this->idusuario = (isset($this->data['idusuario'])) ? $this->data['idusuario'] : null;
    $this->sector = (isset($this->data['sector'])) ? $this->data['sector'] : null;
    $this->detalle = (isset($this->data['detalle'])) ? $this->data['detalle'] : null;
    $this->cuerpo = (isset($this->data['cuerpo'])) ? $this->data['cuerpo'] : null;
    if(isset($this->cuerpo))
    {
      $this->detalle = $this->cuerpo;
    }
    $this->logger = new Logger();
  }

  /**
   * las funciones que comienzan con GET son propias del método GET de HTTP
   * las funciones que comienzan con SET son propias del método PUT de HTTP
   * las funciones que comienzan con PUSH son propias del método POST de HTTP
   * las funciones que comienzan con DELETE son propias del método DELETE de HTTP
   */

  /**
   * Devuleve los tipos de observacion
   * 
   * @return array con los tipos de observaciom.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getTipoDeObservacion()
  {
    try {
      $q = "SELECT * FROM tipoobservacion";

      $consulta = $this->db->query($q);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);

      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)){
        http_response_code(OK);
        return $consulta;
      }
      else {
        http_response_code(NO_CONTENT);
        return "consulta vacia";
      }
    } catch (Exception $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }
  }

  /**
   * Devuleve el historial de seguimientos de un empleado
   * 
   * @param $idempleado
   * @return array con el historial de seguimientos.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getHistorialDeSeguimientos() // $idempleado
  {
    try {
      //obtengo el historial de seguimientos
      $q = "SELECT c1.id, c1.fecha, c1.tipoobservacion, c1.calificacion, c1.idsector, c1.sector, c1.detalle, c1.observacion, CONCAT(sup.apellido,', ',sup.nombre) AS supervisor
            FROM empleados sup INNER JOIN
            (
                SELECT l.id, l.fecha, l.tipoobservacion, l.calificacion, l.usuarioID, l.sector AS idsector, l.detalle, t.nombre AS observacion, s.area AS sector
                FROM legajo l, tipoobservacion t, sectores s, empleados e
                WHERE l.empleadoID='$this->idempleado' AND t.id=l.tipoobservacion AND s.id=e.sector AND e.id=l.empleadoID AND l.activo='A'
            ) AS c1 ON sup.id=c1.usuarioID ORDER BY c1.fecha DESC";

      $consulta = $this->db->query($q);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);

      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)){
        http_response_code(OK);
        return $consulta;
      }
      else {
        http_response_code(NO_CONTENT);
        return "consulta vacia";
      }
    } catch (Exception $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    } 
  }

  /**
   * Devuleve los últimos seguimientos en la sala
   * 
   * @param $idacceso
   * @param $listahijos
   * @return array con los últimos seguimientos en la sala.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getSeguimientosGlobales() //$idacceso, $listahijos
  {
    try {
      // aca tengo que definir los que levanto segun el idacceso
      if($this->idacceso == 1 || $this->idacceso == 10){
        $q = "SELECT l.id, l.fecha, concat(u.apelido,', ',u.nombre) AS supervisor, concat(e.apellido,', ',e.nombre) AS observado, t.nombre AS observacion, l.calificacion,
                l.usuarioID AS idsupervisor, s.area AS sector , l.detalle, l.empleadoID AS idobservado, IF(e.foto IS NULL OR e.foto = '', 'img/user.jpg', e.foto) AS foto,
                e.nombre, e.apellido, e.sector AS idsector
              FROM legajo l, empleados e, sectores s, tipoobservacion t, usuariosareas u
              WHERE u.id=l.usuarioID AND e.id=l.empleadoID AND t.id=l.tipoobservacion AND s.id=l.sector AND e.acceso NOT IN (1,10) AND e.empresa=1 AND l.activo='A'
              ORDER BY l.fecha DESC LIMIT 500";
      }
      else if($this->idacceso == 3 || $this->idacceso == 9 || $this->idacceso == 21 )
      {
        $q = "SELECT l.id, l.fecha, concat(u.apelido,', ',u.nombre) AS supervisor, concat(e.apellido,', ',e.nombre) AS observado, t.nombre AS observacion, l.calificacion,
                l.usuarioID AS idsupervisor, s.area AS sector, l.detalle, l.empleadoID AS idobservado, IF(e.foto IS NULL OR e.foto = '', 'img/user.jpg', e.foto) AS foto,
                e.nombre, e.apellido, e.sector AS idsector
              FROM legajo l, empleados e, sectores s, tipoobservacion t, usuariosareas u
              WHERE u.id=l.usuarioID AND e.id=l.empleadoID AND t.id=l.tipoobservacion AND s.id=l.sector AND e.acceso NOT IN (1,10,3,9,21,18) AND e.empresa=1 AND l.activo='A'
              ORDER BY l.fecha DESC LIMIT 500";
      }
      else// if($idacceso == 18 || $idacceso == 17 || $idacceso == 22 || $idacceso == 6 || $idacceso == 8 || $idacceso == 13)
      {
        $q = "SELECT l.id, l.fecha, concat(u.apelido,' ',u.nombre) AS supervisor, concat(e.apellido,' ',e.nombre) AS observado, t.nombre AS observacion, l.calificacion,
                l.usuarioID AS idsupervisor, s.area AS sector, l.detalle, l.empleadoID AS idobservado, IF(e.foto IS NULL OR e.foto = '', 'img/user.jpg', e.foto) AS foto,
                e.nombre, e.apellido, e.sector AS idsector
              FROM legajo l, empleados e, sectores s, tipoobservacion t, usuariosareas u
              WHERE u.id=l.usuarioID AND e.id=l.empleadoID AND t.id=l.tipoobservacion AND s.id=l.sector AND e.acceso IN ($this->listahijos) AND e.empresa=1 AND l.activo='A'
              ORDER BY l.fecha DESC LIMIT 500";
      }

      $consulta = $this->db->query($q);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);

      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)){
        http_response_code(OK);
        return $consulta;
      }
      else {
        http_response_code(NO_CONTENT);
        return "consulta vacia";
      }
     
    } catch (Exception $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }
  }

  /**
   * Devuleve los posibles empleados para generarles un seguimiento
   * 
   * @param $idacceso
   * @param $listahijos
   * @return array con los posibles empleados para generarles un seguimiento.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getEmpleadosParaSeguimiento() //$idacceso, $listahijos
  {
    try {
      if($this->idacceso == 1 || $this->idacceso == 10){
				$q = "SELECT e.id, concat(e.apellido,', ',e.nombre) AS nombre, e.sector
							FROM empleados e WHERE e.activo='SI' AND e.empresa=1 AND e.acceso NOT IN (1,10)
							ORDER BY nombre ASC";
			}
			else if($this->idacceso == 3 || $this->idacceso == 9 || $this->idacceso == 21 )
			{
				$q = "SELECT e.id, concat(e.apellido,', ',e.nombre) AS nombre, e.sector
							FROM empleados e WHERE e.activo='SI' AND e.acceso NOT IN (1,10,3,9,21,18) AND e.empresa=1
							ORDER BY nombre ASC";
			}
			else if($this->idacceso == 18 || $this->idacceso == 22 || $this->idacceso == 13)
			{
				$q = "SELECT e.id , concat(e.apellido,', ',e.nombre) AS nombre, e.sector
							FROM empleados e WHERE e.activo='SI' AND e.acceso IN ($this->listahijos) AND e.empresa=1
							ORDER BY nombre ASC";
			}

      $consulta = $this->db->query($q);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);

      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)){
        http_response_code(OK);
        return $consulta;
      }
      else {
        http_response_code(NO_CONTENT);
        return "consulta vacia";
      }
     
    } catch (Exception $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }
  }
  
  /**
   * Borra un seguimiento
   * 
   * @param $idseguimiento
   * @return string ok.
   * @return OK consulta exitosa.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function deleteSeguimiento()  // $idseguimiento
  {
    try {
      $activo = 'B';
      $q = "UPDATE legajo SET activo=:activo WHERE id=:id";
      $stmt = $this->db->prepare($q);
      $stmt->bindParam(':id', $this->idseguimiento);
      $stmt->bindParam(':activo', $activo);
      $stmt->execute();
      if($stmt) {
        http_response_code(OK);
        return "OK";
      } else {
        $this->logger->fatal("fallo al borrar seguimiento: " . $this->idseguimiento, $this->ARCHIVO, __LINE__);
        http_response_code(INTERNAL_SERVER_ERROR);
        return "fallo en la consulta";
      }
    } catch (Exception $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }
  }

  /**
   * Inserta un seguimiento
   * 
   * @param $idempleado
   * @param $tipoobservacion
   * @param $calificacion
   * @param $idusuario
   * @param $sector
   * @param $detalle
   * @return string ok.
   * @return OK consulta exitosa.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  // TODO: mejorar esta función
  public function pushSeguimiento()  // $idempleado, $tipoobservacion, $calificacion, $idusuario, $sector, $detalle
  {
    try {
      $q = "INSERT INTO legajo (empleadoID, tipoobservacion, calificacion, usuarioID, sector, detalle) VALUES (:empleadoID, :tipoobservacion, :calificacion, :usuarioID, :sector, :detalle)";
      // Prepare statement
      $stmt = $this->db->prepare($q);
      // Bind params
      $stmt->bindParam(':empleadoID', $this->idempleado);
      $stmt->bindParam(':tipoobservacion', $this->tipoobservacion);
      $stmt->bindParam(':calificacion', $this->calificacion);
      $stmt->bindParam(':usuarioID', $this->idusuario);
      $stmt->bindParam(':sector', $this->sector);
      $stmt->bindParam(':detalle', $this->detalle);
      $stmt->execute();
      if($stmt) {
        http_response_code(OK);
        return "OK";
      } else {
        $this->logger->fatal("fallo al crear seguimiento", $this->ARCHIVO, __LINE__);
        http_response_code(INTERNAL_SERVER_ERROR);
        return "fallo en la consulta";
      }
    } catch (Exception $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }
  }

}

?>