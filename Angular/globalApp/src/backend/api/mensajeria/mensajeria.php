<?php

// clase para manejar las Sql relacionadas con mensajeria
// TODO: colocar un esquema de logs y cerrar la conexion en algun lugar fuera de la clase
class Mensajeria
{
  private $db = null; // propiedad para la conexion a la DB
  private $requestMethod = null; // propiedad para el metodo de peticion http (GET, POST, ...)

  private $data = null;  // propiedad para obtener los datos de entrada

  private $seguimientos;

  private $idusuario = null;
  private $idmensaje = null;  
  private $numeroLecturas = null;
  private $tabla = null;
  private $correorestringido = null;
  private $idacceso = null;
  private $asunto = null;
  private $cuerpo = null;
  private $area = null;
  private $destinos = null;
  private $seguimiento = null;
  private $reenviar = null;
  private $cuerpo2 = null;
  private $cantidad = null;
  private $respuestas = null;
  private $fueQuienLoGenero = null;
  private $ARCHIVO = 'mensajeria/mensajeria.php';
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
      $this->cantidad = json_decode($_POST['cantidad'], true);
    } else {
      $this->data = $_GET;
    }

    // ChromePhp::log(json_decode(file_get_contents("php://input")));
    $this->idusuario = (isset($this->data['idusuario'])) ? $this->data['idusuario'] : null;
    $this->idmensaje = (isset($this->data['idmensaje'])) ? $this->data['idmensaje'] : null;
    $this->numeroLecturas = (isset($this->data['numeroLecturas'])) ? $this->data['numeroLecturas'] : null;
    $this->tabla = (isset($this->data['tabla'])) ? $this->data['tabla'] : null;
    $this->correorestringido = (isset($this->data['correorestringido'])) ? filter_var($this->data['correorestringido'], FILTER_VALIDATE_BOOLEAN) : null;
    $this->idacceso = (isset($this->data['idacceso'])) ? $this->data['idacceso'] : null;
    $this->asunto = (isset($this->data['asunto'])) ? $this->data['asunto'] : null;
    $this->cuerpo = (isset($this->data['cuerpo'])) ? $this->data['cuerpo'] : null;
    $this->area = (isset($this->data['area'])) ? $this->data['area'] : null;
    $this->destinos = (isset($this->data['destinos'])) ? $this->data['destinos'] : null;
    $this->seguimiento = (isset($this->data['seguimiento'])) ? filter_var($this->data['seguimiento'], FILTER_VALIDATE_BOOLEAN) : null;
    $this->reenviar = (isset($this->data['reenviar'])) ? filter_var($this->data['reenviar'], FILTER_VALIDATE_BOOLEAN) : null;
    $this->cuerpo2 = (isset($this->data['cuerpo2'])) ? $this->data['cuerpo2'] : null;
    $this->respuestas = (isset($this->data['respuestas'])) ? filter_var($this->data['respuestas'], FILTER_VALIDATE_INT) : null;
    $this->fueQuienLoGenero = (isset($this->data['fueQuienLoGenero'])) ? filter_var($this->data['fueQuienLoGenero'], FILTER_VALIDATE_BOOLEAN) : null;
    if($this->seguimiento == true)
    {
      $this->cuerpo = $this->cuerpo2;
      $this->seguimientos = new Seguimientos($this->db, $this->requestMethod);
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
   * Devuleve los mensajes de la bandeja de entrada
   * 
   * @param $idusuario
   * @return array con los mensajes.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getBandejaDeEntradaTodos() // $idusuario
  {
    $sql = "SELECT s1.id, s1.remitente, s1.asunto, s1.cuerpo, IFNULL(r.fechacreacion, s1.fecha) AS fecha, s1.lecturas, s1.respuestas, 
                s1.estado, s1.leido, s1.destacado, s1.prioridad, s1.area, s1.nombreapellido, s1.foto, s1.fechamsj
            FROM
            (
            -- obtiene los correos de la tabla mensaje dirijidos a él
            SELECT m.id, m.remitente, m.asunto, m.cuerpo, m.fechacreacion AS fecha,
                  m.lecturas, m.respuestas, m.estado, am.leido, am.destacado, ep.prioridad, ua.area, CONCAT(ua.nombre,' ',ua.apelido) AS nombreapellido,
                  IF(ua.foto IS NULL OR ua.foto = '', 'img/user.jpg', ua.foto) AS foto, m.fechacreacion as fechamsj
            FROM mensajes m, usuariosareas ua, estado_prioridad ep,
            (
            SELECT mensajeid, leido, destacado
            FROM area_mensaje
            WHERE areaid='$this->idusuario'
            ORDER BY idarea_mensaje DESC
            -- limit 500
            ) am
            WHERE m.id=am.mensajeid AND ep.estado=am.destacado AND m.estado!='A' AND ua.id=m.remitente 
            ) AS s1 LEFT JOIN(
                          SELECT rta.id, rta.mensajeid, c1.fechacreacion, rta.remitente
                          FROM
                          (
                            SELECT max(fechacreacion) AS fechacreacion
                            FROM respuestas 
                            GROUP BY mensajeid
                          ) AS c1 INNER JOIN respuestas rta ON rta.fechacreacion=c1.fechacreacion
            ) AS r ON s1.id=r.mensajeid
            UNION
            (
            SELECT m.id, m.remitente, m.asunto, m.cuerpo, r.fechacreacion AS fecha, m.lecturas, m.respuestas,
                m.estado, m.leido, m.destacado, ep.prioridad, ua.area,
                CONCAT(ua.nombre,' ',ua.apelido) AS nombreapellido, IF(ua.foto IS NULL OR ua.foto = '', 'img/user.jpg', ua.foto) AS foto, m.fechacreacion as fechamsj
            FROM mensajes m, 
            (
            SELECT rta.id, rta.mensajeid, c1.fechacreacion, rta.remitente
            FROM
            (
              SELECT max(fechacreacion) AS fechacreacion
              FROM respuestas 
              GROUP BY mensajeid
            ) AS c1 INNER JOIN respuestas rta ON rta.fechacreacion=c1.fechacreacion   
            ) AS r, 
            estado_prioridad ep, usuariosareas ua WHERE m.id = r.mensajeid AND m.remitente='$this->idusuario'
            AND ep.estado=m.destacado AND m.estado!='A' AND ua.id=m.remitente
            )
            ORDER BY prioridad ASC, fecha DESC
            LIMIT 500";

    try {
      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
        http_response_code(OK);
        return $consulta;
      }
      else {
        http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
        return "consulta vacia";
      }
    } catch (\PDOException $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }    
  }

  /**
   * Devuleve las respuestas a un mensaje de la bandeja de entrada
   * 
   * @param $idmensaje
   * @return array con los mensajes.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getBandejaDeEntradaRespuestas() // $idmensaje
  {
    // obtengo las areas a las que se envio un mensaje
    $sql = "SELECT r.id, r.cuerpo, r.fechacreacion AS fecha, concat(ua.nombre,' ',ua.apelido) AS nombreapellido, 
            IF(ua.foto IS NULL OR ua.foto = '', 'img/user.jpg', ua.foto) AS foto, ua.area
          FROM respuestas r, usuariosareas ua
          WHERE r.mensajeid='$this->idmensaje' AND ua.id=r.remitente
          ORDER BY r.fechacreacion ASC";

    try {
      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
        http_response_code(OK);
        return $consulta;
      }
      else {
        http_response_code(NO_CONTENT);
        return "consulta vacia";
      }
    } catch (\PDOException $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }  
  }

  /**
   * Devuleve los adjuntos de un mensaje
   * 
   * @param $idmensaje
   * @return array con los adjuntos.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getAdjuntos() // $idmensaje
  {
    // obtengo los adjuntos de un mensaje
    $sql = "SELECT a.id, a.mensajeid, a.nombre, a.url, a.fecha, CONCAT(e.nombre,' ',e.apellido) AS subidopor, a.subidopor AS idsubidopor
    FROM adjuntos a INNER JOIN empleados e ON a.subidopor=e.id
    WHERE a.mensajeid=$this->idmensaje AND a.activo='A' AND a.origen='C'
    ORDER BY a.fecha DESC";

    try {
      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
        http_response_code(OK);
        return $consulta;
      }
      else {
        http_response_code(NO_CONTENT);
        return "consulta vacia";
      }
    } catch (\PDOException $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }  
  }

  /**
   * Devuleve las areas destinos de un mensaje
   * 
   * @param $idmensaje
   * @return array con los destinatarios.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getAreasDestinosMensaje() //$idmensaje
  {
    // obtengo las areas a las que se envio un mensaje
    $sql = "SELECT am.areaid, a.area FROM area_mensaje am, sectores a WHERE am.mensajeid='$this->idmensaje' AND a.id=am.areaid";

    try {
      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["areaid"]!=null)) {
        http_response_code(OK);
        return $consulta;
      }
      else {
        http_response_code(NO_CONTENT);
        return "consulta vacia";
      }
    } catch (\PDOException $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }  
  }

  /**
   * Devuleve los destinatarios de un mensaje
   * 
   * @param $idmensaje
   * @return array con los destinatarios.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getDestinatariosMensaje() //$idmensaje
  {
    // obtengo las areas a las que se envio un mensaje
    $sql = "SELECT am.areaid, concat(u.nombre,' ',u.apellido) AS area
            FROM area_mensaje am, empleados u
            WHERE am.mensajeid='$this->idmensaje' AND u.id=am.areaid AND u.activo='SI'";

    try {
      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["areaid"]!=null)) {
        http_response_code(OK);
        return $consulta;
      }
      else {
        http_response_code(NO_CONTENT);
        return "consulta vacia";
      }
    } catch (\PDOException $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }  
  }

  /**
   * Devuleve los mensajes de la bandeja de salida
   * 
   * @param $idmensaje
   * @return array con los mensajes.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getBandejaDeSalida()  // $idusuario
  {
    $sql = "SELECT sc.*, ep.prioridad
          FROM
          (
          	(SELECT m.id, m.cuerpo, m.asunto, m.remitente, concat(ua.nombre,' ',ua.apelido) AS nombreapellido,
                    IF(ua.foto IS NULL OR ua.foto = '', 'img/user.jpg', ua.foto) AS foto, a.area, m.fechacreacion AS fecha, m.lecturas, 
                    m.respuestas, m.estado, 'M' AS esRespuesta, m.destacado
          				FROM sectores a, mensajes m, usuariosareas ua WHERE m.remitente='$this->idusuario' AND a.id=m.area AND ua.id=m.remitente AND m.estado!='A' ORDER BY fecha DESC LIMIT 250)
          				UNION
          				(SELECT m1.id, r.cuerpo, concat('(Rpta: )',' ',m1.asunto) AS asunto, r.remitente, concat(ua.nombre,' ',ua.apelido) AS nombreapellido, 
                    IF(ua.foto IS NULL OR ua.foto = '', 'img/user.jpg', ua.foto) AS foto, a.area,
          					m1.fechacreacion AS fecha, m1.lecturas, m1.respuestas, m1.estado, 'R' AS esRespuesta, 'N' AS destacado
          				FROM respuestas r,  mensajes m1, sectores a, usuariosareas ua
          				WHERE r.remitente='$this->idusuario' AND m1.id=r.mensajeid AND a.id=r.area AND ua.id=r.remitente AND m1.estado!='A' ORDER BY fecha DESC LIMIT 250)
          				ORDER BY fecha DESC
          )  sc  , estado_prioridad ep
          WHERE ep.estado=sc.destacado
          ORDER BY ep.prioridad ASC, sc.fecha DESC";

      try {
        $consulta = $this->db->query($sql);
        $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
        if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
          http_response_code(OK);
          return $consulta;
        }
        else {
          http_response_code(NO_CONTENT);
          return "consulta vacia";
        }
      } catch (\PDOException $e) {
        $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
        http_response_code(INTERNAL_SERVER_ERROR);
        return "error: ".$e->getMessage();
      }  
  }

  /**
   * Devuleve el detalle de un mensaje
   * 
   * @param $idmensaje
   * @return object con el mensaje.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getDetalleMensaje() // $idmensaje
  {
    // obtiene los datos del mensaje
    $sql = "SELECT m.id, m.remitente, m.asunto, m.cuerpo, m.fechacreacion AS fecha, m.lecturas, m.respuestas,
        m.estado, m.leido, m.destacado, ua.area,
        CONCAT(ua.nombre,' ',ua.apelido) AS nombreapellido, IF(ua.foto IS NULL OR ua.foto = '', 'img/user.jpg', ua.foto) AS foto
    FROM mensajes m, usuariosareas ua WHERE m.id = '$this->idmensaje' AND m.estado!='A' AND ua.id=m.remitente";

    try {
      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
        http_response_code(OK);
        return $consulta[0];
      }
      else {
        http_response_code(NO_CONTENT);
        return "consulta vacia";
      }
    } catch (\PDOException $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }  
  }
  
  /**
   * Devuleve los posibles usuarios y areas a los que enviar un correo
   * 
   * @param $correorestringido
   * @param $idacceso
   * @return array de objetos con los usuarios array[0] y las areas array[1]
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getUsuariosCorreo() // $correorestringido, $idacceso
  {
    try {
        
        if($this->correorestringido == 1)
        {
          $ofrrhh = '25';
          if($this->idacceso == 23)	//Rrpp --> sector permitido: 35 Sup. Rrpp
          {
            $habilitados = '35';
          }
          else if ($this->idacceso == 5 || $this->idacceso == 7) {
            $habilitados = '19,33';
          }
          else if ($this->idacceso == 11) {
            $habilitados = '34';
          }
          else { // por descarte admision
            $habilitados = '18';
          }
  
          $habilitados = $habilitados.','.$ofrrhh;
          
          $datos = array();
  
          $q = "SELECT u.id, concat(apellido,', ',nombre) AS usuario, a.area AS area
                FROM empleados u INNER JOIN sectores a ON u.sector=a.id
                WHERE u.activo='SI' AND u.sector IN ($habilitados)
                ORDER BY usuario ASC";
          $lista = $this->db->query($q);
          $lista = $lista->fetchAll(PDO::FETCH_ASSOC);
  
          $q = "SELECT s.id, s.area
                FROM sectores s INNER JOIN empleados e ON e.sector=s.id
                WHERE e.activo='SI' AND s.id IN ($habilitados)
                GROUP BY e.sector
                ORDER BY s.area ASC";
  
          $lista2 = $this->db->query($q);
          $lista2 = $lista2->fetchAll(PDO::FETCH_ASSOC);
  
          if (count($lista) >= 1 && ($lista[0]["id"]!=null)){
            array_push($datos, $lista);
            if(count($lista2) >= 1 && ($lista2[0]["id"]!=null)){
              array_push($datos, $lista2);
              // echo json_encode($datos);
              http_response_code(OK);
              return $datos;
            }
            else {
              http_response_code(NO_CONTENT);
              return "consulta vacia";
            }
          }
          else {
            http_response_code(NO_CONTENT);
            return "consulta vacia";
          }
        }
        else {
          $datos = array();
          $q = "SELECT u.id, concat(apellido,', ',nombre) AS usuario, a.area AS area
                FROM empleados u INNER JOIN sectores a ON u.sector=a.id
                WHERE u.activo='SI'
                ORDER BY usuario ASC";
  
          $lista = $this->db->query($q);
          $lista = $lista->fetchAll(PDO::FETCH_ASSOC);
  
          $q = "SELECT id, area FROM sectores WHERE activo='A' ORDER BY area ASC";
  
          $lista2 = $this->db->query($q);
          $lista2 = $lista2->fetchAll(PDO::FETCH_ASSOC);
  
          if (count($lista) >= 1 && ($lista[0]["id"]!=null)){
            array_push($datos, $lista);
            if(count($lista2) >= 1 && ($lista2[0]["id"]!=null)){
              array_push($datos, $lista2);
              // echo json_encode($datos);
              http_response_code(OK);
              return $datos;
            }
            else {
              http_response_code(NO_CONTENT);
              return "consulta vacia";
            }
          }
          else {
            http_response_code(NO_CONTENT);
            return "consulta vacia";
          }
        }
     
    } catch (Exception $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }
  }

  /**
   * Devuleve el detalle del mensaje a reenviar
   * 
   * @param $idmensaje
   * @param $tabla
   * @return object con un arreglo de mensajes.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getDetalleReenvio() // $idmensaje, $tabla
  {
    // obtiene los datos del mensaje
    if ($this->tabla == 'mensajes') {
      $sql = "SELECT m.asunto, m.cuerpo, a.*, CONCAT(e.nombre, ' ', e.apellido) AS adjuntadopor
              FROM mensajes m LEFT JOIN adjuntos a ON m.id=a.mensajeid
              INNER JOIN empleados e ON e.id=a.subidopor
              WHERE m.id=$this->idmensaje AND a.origen='C'";
    } else {
      $sql = "SELECT m.asunto, r.cuerpo, a.*, CONCAT(e.nombre, ' ', e.apellido) AS adjuntadopor
              FROM mensajes m 
              INNER JOIN respuestas r ON m.id=r.mensajeid 
              LEFT JOIN adjuntos a ON r.mensajeid=a.mensajeid
              INNER JOIN empleados e ON e.id=a.subidopor
              WHERE r.id=$this->idmensaje AND a.origen='C'";
    }
    
    try {
      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
        http_response_code(OK);
        return $consulta;
      }
      else {
        http_response_code(NO_CONTENT);
        return "consulta vacia";
      }
    } catch (\PDOException $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }  
  }

  /**
   * Setea la prioridad de un mensaje
   * 
   * @param $idmensaje
   * @return string ok.
   * @return OK consulta exitosa.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function setPrioridad()  // $idmensaje
  {
    $destacado = "I";
    try {
      $this->db->beginTransaction();

      $sql = "UPDATE mensajes SET destacado=:destacado WHERE id=:id";
      $stmt = $this->db->prepare($sql);
      $stmt->bindParam(':id', $this->idmensaje);
      $stmt->bindParam(':destacado', $destacado);
      $stmt->execute();    
      
      $sql = "UPDATE area_mensaje SET destacado=:destacado WHERE mensajeid=:id";
      $stmt = $this->db->prepare($sql);
      $stmt->bindParam(':id', $this->idmensaje);
      $stmt->bindParam(':destacado', $destacado);
      $stmt->execute();
     
      // commit the transaction
      $this->db->commit();
      $this->logger->info("Cambio de prioridad a mensaje: ". $this->idmensaje, $this->ARCHIVO, __LINE__);
      http_response_code(OK);
      return "ok";
    }
    catch(PDOException $e)
    {
      // roll back the transaction if something failed
      $this->db->rollback();
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }
  }

  /**
   * Setea la cantidad de veces que se leyo un mensaje de la bandeja de entrada
   * 
   * Setea el número de lecturas de un mensaje y lo marca como leido para ese usuario
   * 
   * @param $idmensaje
   * @param $idusuario
   * @param $numeroLecturas
   * @param $tabla
   * @return string ok.
   * @return OK consulta exitosa.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function setLecturaBandejaDeEntrada()  // $idmensaje, $idusuario, $numeroLecturas, $tabla
  {
    try {
      // begin the transaction
      $this->db->beginTransaction();
      // actualiza el número de lecturas
      $stmt = $this->db->prepare("UPDATE mensajes SET lecturas=:lecturas WHERE id=:id");

      $stmt->bindParam(':lecturas', $this->numeroLecturas);
      $stmt->bindParam(':id', $this->idmensaje);
      $stmt->execute();
      // si es quien creo el mensaje se coloca el estado de destacado en normal y se marca como leido en la table mensajes
      if($this->tabla=="mensajes")
      {
        $stmt = $this->db->prepare("UPDATE mensajes SET leido='S', destacado='N' WHERE id=:id");
        $stmt->bindParam(':id', $this->idmensaje);
        $stmt->execute();
      }
      // en todos los casos actualiza el estado de destacado y lo marca como leido en la tabla area_mensaje
      $stmt = $this->db->prepare("UPDATE area_mensaje SET leido='S', destacado='N' WHERE mensajeid=:id AND areaid=:areaid");
      $stmt->bindParam(':areaid', $this->idusuario);
      $stmt->bindParam(':id', $this->idmensaje);
      $stmt->execute();

      $this->db->commit();
      $this->logger->info("lectura del mensaje: ". $this->idmensaje, $this->ARCHIVO, __LINE__);
      http_response_code(OK);
      return "ok";
    }
    catch(PDOException $e)
    {
      // roll back the transaction if something failed
      $this->db->rollback();
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }
  }

  /**
   * Setea la cantidad de veces que se leyo un mensaje de la bandeja de salida
   * 
   * Setea el número de lecturas de un mensaje y lo marca como leido para ese usuario.
   * 
   * @param $idmensaje
   * @param $idusuario
   * @param $tabla
   * @return string ok.
   * @return OK consulta exitosa.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function setLecturaBandejaDeSalida() // $idmensaje, $idusuario, $tabla
  {
    try {
      // begin the transaction
      $this->db->beginTransaction();
      // actualiza el número de lecturas
  		// si es quien creo el mensaje se coloca el estado de destacado en normal y se marca como leido en la table mensajes
  		if($this->tabla=="mensajes")
  		{
  			$stmt = $this->db->prepare("UPDATE mensajes SET destacado='N' WHERE id=:id");
  			$stmt->bindParam(':id', $this->idmensaje);
  			$stmt->execute();
  		}
  		// en todos los casos actualiza el estado de destacado y lo marca como leido en la tabla area_mensaje
  		$stmt = $this->db->prepare("UPDATE area_mensaje SET destacado='N' WHERE mensajeid=:id AND areaid=:areaid");
  		$stmt->bindParam(':areaid', $this->idusuario);
  		$stmt->bindParam(':id', $this->idmensaje);
  		$stmt->execute();

      $this->db->commit();
      $this->logger->info("lectura del mensaje: ". $this->idmensaje, $this->ARCHIVO, __LINE__);
      http_response_code(OK);
      return "ok";
    }
    catch(PDOException $e)
    {
      // roll back the transaction if something failed
      $this->db->rollback();
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }
  }

  /**
   * Inserta un nuevo mensaje
   * 
   * hace uso de la clase seguimientos para insertar el segumiento de ser necesario
   * @return string ok.
   * @return OK consulta exitosa.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion. 
  */
  public function pushNuevoCorreo()
  {
    try {
      $this->db->beginTransaction();

      // inserto en la tabla mensajes
      $stmt = $this->db->prepare("INSERT INTO mensajes (remitente, asunto, cuerpo, area) VALUES (:remitente, :asunto, :cuerpo, :area)");

      $stmt->bindParam(':remitente', $this->idusuario);
      $stmt->bindParam(':asunto', $this->asunto);
      $stmt->bindParam(':cuerpo', $this->cuerpo);
      $stmt->bindParam(':area', $this->area);
      $stmt->execute();

      $id = $this->db->lastInsertId();	// obtengo el mensajeId
      $cant = count($this->destinos);	// obtengo la cantidad de areas destino
      $aux = null;
      $stmt = $this->db->prepare("INSERT INTO area_mensaje (areaid, mensajeid) VALUES (:areaid, :id)");
  		$stmt->bindParam(':areaid', $aux);
      $stmt->bindParam(':id', $id);
      
      for($i = 0; $i < $cant; $i++)
  		{
        $aux = $this->destinos[$i]['id'];
        $stmt->execute();
      }
      
      if($this->seguimiento == true)
  		{
        // TODO: verificar si falla y forzar un roolback junto con un exit
        $this->seguimientos->pushSeguimiento();
      }
      
      if($this->cantidad > 0)
      {
        $directorio = "../assets/adjuntos/";
        chdir("../");   // subo un nivel de directorio
        if(!is_dir($directorio))
          mkdir($directorio, 0777, true);

        //Loop through each file
        for($i=0; $i<$this->cantidad; $i++)
        {
          //Make sure we have a filepath
          $nombre = $_FILES["adjunto".$i]["name"];

        	$temp = explode(".", $_FILES["adjunto".$i]["name"]);
        	$url = $directorio.uniqid() . uniqid() . uniqid() . '.' . end($temp);
        	move_uploaded_file($_FILES["adjunto".$i]["tmp_name"], $url);

      	   // prepare sql and bind parameters
          $sql = "INSERT INTO adjuntos (mensajeid, nombre, url, subidopor) VALUES (:mensajeid, :nombre, :url, :subidopor)";
      	  $stmt = $this->db->prepare($sql);
          $stmt->bindParam(':mensajeid', $id);
          $stmt->bindParam(':nombre', $nombre);
      	  $stmt->bindParam(':url', $url);
          $stmt->bindParam(':subidopor', $this->idusuario);
          $stmt->execute();
        }
      }

      if($this->reenviar == true)
      {
        $cantidadAdjuntos = count($this->data['adjuntosReenvio']);
        //Loop through each file
        for($i=0; $i<$cantidadAdjuntos; $i++)
        {
          $varauxadjunto = $this->data['adjuntosReenvio'][$i];
          // prepare sql and bind parameters
          $sql = "INSERT INTO adjuntos (mensajeid, nombre, url, subidopor, fecha) VALUES (:mensajeid, :nombre, :url, :subidopor, :fecha)";
      	  $stmt = $this->db->prepare($sql);
          $stmt->bindParam(':mensajeid', $id);
          $stmt->bindParam(':nombre', $varauxadjunto['nombre']);
      	  $stmt->bindParam(':url', $varauxadjunto['url']);
          $stmt->bindParam(':subidopor', $varauxadjunto['subidopor']);
          $stmt->bindParam(':fecha', $varauxadjunto['fecha']);
          // execute the query
          $stmt->execute();
        }
      }
     
      // commit the transaction
      $this->db->commit();
      http_response_code(OK);
      return "ok";
    }
    catch(PDOException $e)
    {
      // roll back the transaction if something failed
      $this->db->rollback();
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }
  }

  /**
   * Inserta una respuesta
   * 
   * @return string ok.
   * @return OK consulta exitosa.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion. 
  */
  public function pushRespuesta()
  {
    try {
      $this->db->beginTransaction();

      // inserto en la tabla mensajes
      $stmt = $this->db->prepare("INSERT INTO respuestas (mensajeid, remitente, cuerpo, area) VALUES (:mensajeid, :remitente, :cuerpo, :area)");

      $stmt->bindParam(':mensajeid', $this->idmensaje);
      $stmt->bindParam(':remitente', $this->idusuario);      
      $stmt->bindParam(':cuerpo', $this->cuerpo);
      $stmt->bindParam(':area', $this->area);
      $stmt->execute();

      //update cantidad de respuestas
      $this->respuestas+=1;
      $estado = 'N';
    	$q = "UPDATE mensajes SET respuestas='$this->respuestas', estado='$estado' WHERE id='$this->idmensaje'";

    	$stmt = $this->db->prepare($q);
      $stmt->execute();
      
      // archivos adjuntos
      if($this->cantidad > 0)
      {
        $directorio = "../assets/adjuntos/";
        chdir("../");   // subo un nivel de directorio
        if(!is_dir($directorio))
          mkdir($directorio, 0777, true);

        //Loop through each file
        for($i=0; $i<$this->cantidad; $i++)
        {
          //Make sure we have a filepath
          $nombre = $_FILES["adjunto".$i]["name"];

        	$temp = explode(".", $_FILES["adjunto".$i]["name"]);
        	$url = $directorio.uniqid() . uniqid() . uniqid() . '.' . end($temp);
        	move_uploaded_file($_FILES["adjunto".$i]["tmp_name"], $url);

      	   // prepare sql and bind parameters
          $sql = "INSERT INTO adjuntos (mensajeid, nombre, url, subidopor) VALUES (:mensajeid, :nombre, :url, :subidopor)";
      	  $stmt = $this->db->prepare($sql);
          $stmt->bindParam(':mensajeid', $this->idmensaje);
          $stmt->bindParam(':nombre', $nombre);
      	  $stmt->bindParam(':url', $url);
          $stmt->bindParam(':subidopor', $this->idusuario);
          $stmt->execute();
        }
      }

      // actualizar leidos
      if($this->fueQuienLoGenero)
  		{
        $stmt = $this->db->prepare("UPDATE area_mensaje SET leido='N' WHERE mensajeid=:id");
  			$stmt->bindParam(':id', $this->idmensaje);
  			$stmt->execute();

        $stmt = $this->db->prepare("UPDATE mensajes SET leido='S' WHERE id=:id");
  			$stmt->bindParam(':id', $this->idmensaje);
  			$stmt->execute();
  		}
  		else
  		{
        $stmt = $this->db->prepare("UPDATE mensajes SET leido='N' WHERE id=:id");
  			$stmt->bindParam(':id', $this->idmensaje);
  			$stmt->execute();

  			$stmt = $this->db->prepare("UPDATE area_mensaje SET leido='N' WHERE mensajeid=:id AND areaid!=:areaid");
  			$stmt->bindParam(':areaid', $this->idusuario);
  			$stmt->bindParam(':id', $this->idmensaje);
  			$stmt->execute();

        $stmt = $this->db->prepare("UPDATE area_mensaje SET leido='S' WHERE mensajeid=:id AND areaid=:areaid");
  			$stmt->bindParam(':areaid', $this->idusuario);
  			$stmt->bindParam(':id', $this->idmensaje);
  			$stmt->execute();
  		}
     
      // commit the transaction
      $this->db->commit();
      http_response_code(OK);
      return "ok";
    }
    catch(PDOException $e)
    {
      // roll back the transaction if something failed
      $this->db->rollback();
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }
  }

}

?>