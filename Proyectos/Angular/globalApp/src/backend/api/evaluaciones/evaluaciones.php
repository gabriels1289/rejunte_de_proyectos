<?php
$ARCHIVO = 'evaluaciones/evaluaciones.php';
// clase para manejar las Sql relacionadas con evaluaciones
// TODO: colocar un esquema de logs y cerrar la conexion en algun lugar fuera de la clase
class Evaluaciones
{
  private $db = null; // propiedad para la conexion a la DB
  private $requestMethod = null; // propiedad para el metodo de peticion http (GET, POST, ...)

  private $data = null;  // propiedad para obtener los datos de entrada

  private $idusuario = null;
  private $texto = null;
  private $tabla = null;
  private $listaPermitidos = null;
  private $positivo = null;
  private $negativo = null;
  private $modelo = null;
  private $iddevolucion = null;
  private $editar = null;
  private $idevaluado = null;
  private $idsupervisor = null;
  private $idevaluacion = null;
  private $idmodelo = null;
  private $origen = null;
  private $activa = null;
  private $idsector = null;
  private $tituloReporte = null;
  private $nombreExcel = null;
  private $entregada = null;
  private $rtaEmpleado = null;
  private $destinos = null;
  private $nombreevaluacion = null;
  private $evaluadores = null;
  private $usarplantilla = null;
  private $numeroplantilla = null;
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
    $this->texto = (isset($this->data['texto'])) ? $this->data['texto'] : null;
    $this->tabla = (isset($this->data['tabla'])) ? $this->data['tabla'] : null;
    $this->listaPermitidos = (isset($this->data['listaPermitidos'])) ? $this->data['listaPermitidos'] : null;
    $this->positivo = (isset($this->data['positivo'])) ? $this->data['positivo'] : null;
    $this->negativo = (isset($this->data['negativo'])) ? $this->data['negativo'] : null;
    $this->modelo = (isset($this->data['modelo'])) ? $this->data['modelo'] : null;
    $this->iddevolucion = (isset($this->data['iddevolucion'])) ? filter_var($this->data['iddevolucion'], FILTER_VALIDATE_INT) : null;
    $this->editar = (isset($this->data['editar'])) ? filter_var($this->data['editar'], FILTER_VALIDATE_BOOLEAN) : null;
    $this->idevaluado = (isset($this->data['idevaluado'])) ? filter_var($this->data['idevaluado'], FILTER_VALIDATE_INT) : null;
    $this->idsupervisor = (isset($this->data['idsupervisor'])) ? filter_var($this->data['idsupervisor'], FILTER_VALIDATE_INT) : null;
    $this->idevaluacion = (isset($this->data['idevaluacion'])) ? filter_var($this->data['idevaluacion'], FILTER_VALIDATE_INT) : null;
    $this->idmodelo = (isset($this->data['idmodelo'])) ? filter_var($this->data['idmodelo'], FILTER_VALIDATE_INT) : null;
    $this->origen = (isset($this->data['origen'])) ? $this->data['origen'] : null;
    $this->activa = (isset($this->data['activa'])) ? $this->data['activa'] : null;
    $this->idsector = (isset($this->data['idsector'])) ? filter_var($this->data['idsector'], FILTER_VALIDATE_INT) : null;
    $this->tituloReporte = (isset($this->data['tituloReporte'])) ? $this->data['tituloReporte'] : null;
    $this->nombreExcel = (isset($this->data['nombreExcel'])) ? $this->data['nombreExcel'] : null;
    $this->entregada = (isset($this->data['entregada'])) ? $this->data['entregada'] : null;
    $this->rtaEmpleado = (isset($this->data['rtaEmpleado'])) ? $this->data['rtaEmpleado'] : null;
    $this->destinos = (isset($this->data['destinos'])) ? $this->data['destinos'] : null;
    $this->nombreevaluacion = (isset($this->data['nombreevaluacion'])) ? $this->data['nombreevaluacion'] : null;
    $this->evaluadores = (isset($this->data['evaluadores'])) ? $this->data['evaluadores'] : null;
    $this->usarplantilla = (isset($this->data['usarplantilla'])) ? filter_var($this->data['usarplantilla'], FILTER_VALIDATE_BOOLEAN) : null;
    $this->numeroplantilla = (isset($this->data['numeroplantilla'])) ? filter_var($this->data['numeroplantilla'], FILTER_VALIDATE_INT) : null;

    $this->logger = new Logger();
  }

  /**
   * las funciones que comienzan con GET son propias del método GET de HTTP
   * las funciones que comienzan con SET son propias del método PUT de HTTP
   * las funciones que comienzan con PUSH son propias del método POST de HTTP
   * las funciones que comienzan con DELETE son propias del método DELETE de HTTP
   */

  /**
   * Devuleve el listado de evaluaciones de un empleado
   * 
   * @param $idevaluado
   * @param $origen
   * @return array con los empleados.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getEvaluacionesDeEmpleado() // $idevaluado, $origen
  {
    if($this->origen == "misevaluaciones")
			{
				$sql = "SELECT eval.id AS evaluacionid, eval.titulo, eval.modelo, DATE_FORMAT(eval.fecha, '%Y-%m-%d') AS fecha, eval.activa,
          CONCAT(eval.titulo,' - ',DATE_FORMAT(eval.fecha, '%Y-%m-%d')) AS texto
				FROM puntajes_evaluacion pe, evaluacion eval
				WHERE pe.evaluado='$this->idevaluado' AND eval.id=pe.evaluacion AND eval.activa='NO'
				GROUP BY eval.titulo
				ORDER BY fecha DESC";
			}
			else{
				$sql = "SELECT eval.id AS evaluacionid, eval.titulo, eval.modelo, DATE_FORMAT(eval.fecha, '%Y-%m-%d') AS fecha, eval.activa,
          CONCAT(eval.titulo,' - ',DATE_FORMAT(eval.fecha, '%Y-%m-%d')) AS texto
				FROM puntajes_evaluacion pe, evaluacion eval
				WHERE pe.evaluado='$this->idevaluado' AND eval.id=pe.evaluacion AND (eval.activa='CE' OR eval.activa='NO')
				GROUP BY eval.titulo
				ORDER BY fecha DESC";
			}

    try {
      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["evaluacionid"]!=null)) {
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
   * Devuleve el detalle de la evaluacion de un empleado
   * 
   * @param $idevaluado
   * @param $idevaluacion
   * @return un objeto con toda la informacion del empleado.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getDetalleEvaluacionEmpleado() // $idevaluado, $idevaluacion
  {
    try {
      $datos = array();

      // modelo
      $sql = "SELECT s2.id, s2.idcompetencia, s2.competencia, s2.idcomportamiento, s2.comportamiento, s1.promedio
			 			FROM
						(
							SELECT p.modeloid, AVG(p.puntaje) AS promedio
							FROM comportamientos_evaluacion ce, modelo_evaluacion m, competencias_evaluacion c, evaluacion e, puntajes_evaluacion p
							WHERE e.id='$this->idevaluacion' AND ce.id=m.comportamiento AND m.competencia=c.id AND e.modelo=m.evaluacionid AND p.evaluacion=e.id AND p.evaluado='$this->idevaluado' AND p.ignorar='F'
							GROUP BY p.modeloid
						) AS s1
						LEFT JOIN
						(
							SELECT c.id AS idcompetencia, c.competencia, m.id, ce.id AS idcomportamiento, ce.comportamiento
							FROM comportamientos_evaluacion ce, modelo_evaluacion m, competencias_evaluacion c, evaluacion e, puntajes_evaluacion p
							WHERE e.id='$this->idevaluacion' AND ce.id=m.comportamiento AND m.competencia=c.id AND e.modelo=m.evaluacionid AND p.evaluacion=e.id AND p.evaluado='$this->idevaluado'
							GROUP BY m.id
						) AS s2
						ON s1.modeloid=s2.id";

      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
        array_push($datos, $consulta);
        
        // competencias
        $sql = "SELECT m.id, c.competencia
	            FROM modelo_evaluacion m, competencias_evaluacion c, evaluacion e
	            WHERE m.competencia=c.id AND m.evaluacionid=e.modelo AND e.id='$this->idevaluacion'
              GROUP BY m.competencia";
        $consulta = $this->db->query($sql);
        $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
        if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
          array_push($datos, $consulta);

          // supervisores
          $sql = "SELECT p.supervisor AS idsup, CONCAT(sup.nombre,' ',sup.apellido) AS nombresupervisor,
                    CONCAT(SUBSTRING(sup.nombre,1,1),'.',SUBSTRING(sup.apellido,1,1),'.') AS supacortado, AVG(p.puntaje) AS promedio
                  FROM puntajes_evaluacion p, empleados sup, modelo_evaluacion m
                  WHERE p.evaluacion='$this->idevaluacion' AND p.evaluado='$this->idevaluado' AND sup.id=p.supervisor AND p.ignorar='F'
                  GROUP BY p.supervisor";

          $consulta = $this->db->query($sql);
          $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
          if (count($consulta) >= 1 && ($consulta[0]["idsup"]!=null)) {
            array_push($datos, $consulta);
            // puntajes
            $sql = "SELECT p.id AS idpe, p.supervisor AS idsup, c.id AS idcompetencia, p.modeloid, ce.id AS idcomportamiento, p.puntaje, p.ignorar
                    FROM puntajes_evaluacion p, empleados sup, comportamientos_evaluacion ce , modelo_evaluacion m, competencias_evaluacion c
                    WHERE p.evaluacion='$this->idevaluacion' AND p.evaluado='$this->idevaluado' AND sup.id=p.supervisor AND ce.id=m.comportamiento  AND m.competencia=c.id AND p.modeloid=m.id";
            $consulta = $this->db->query($sql);
            $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
            if (count($consulta) >= 1 && ($consulta[0]["idpe"]!=null)) {
              array_push($datos, $consulta);
              http_response_code(OK);
              return $datos;
            }
            else {
              http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
              return "consulta vacia";
            }
          }
          else {
            http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
            return "consulta vacia";
          }
        }
        else {
          http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
          return "consulta vacia";
        }
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
   * Devuleve el detalle de la evaluacion de un empleado
   * 
   * @param $idevaluado
   * @param $idevaluacion
   * @return un objeto con toda la informacion del empleado.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getDetalleEvaluacionEdicion() // $idevaluado, $idevaluacion, $idsupervisor
  {
    try {
      $datos = array();

      // modelo
      $sql = "SELECT m.id, c.competencia, m.comportamiento AS idcomportamiento, ce.comportamiento, pe.puntaje, pe.id AS puntajeid, pe.ignorar
              FROM modelo_evaluacion m, competencias_evaluacion c, puntajes_evaluacion pe, comportamientos_evaluacion ce, evaluacion e
              WHERE m.competencia=c.id AND m.evaluacionid=e.modelo AND e.id='$this->idevaluacion' AND pe.supervisor='$this->idsupervisor'
              AND pe.evaluado='$this->idevaluado' AND pe.evaluacion='$this->idevaluacion' AND pe.modeloid=m.id AND m.comportamiento=ce.id";

      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
        array_push($datos, $consulta);
        
        // competencias
        $sql = "SELECT m.id, c.competencia
                FROM modelo_evaluacion m, competencias_evaluacion c, evaluacion e
                WHERE m.competencia=c.id AND m.evaluacionid=e.modelo AND e.id='$this->idevaluacion'
                GROUP BY m.competencia";
        $consulta = $this->db->query($sql);
        $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
        if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
          array_push($datos, $consulta);

          // foto
          $sql = "SELECT IF(e.foto IS NULL OR e.foto = '', 'img/user.jpg', e.foto) AS foto
                  FROM empleados e
                  WHERE e.id='$this->idevaluado'";

          $consulta = $this->db->query($sql);
          $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
          if (count($consulta) >= 1 ) {
            array_push($datos, $consulta);
            // devoluciones
            $sql = "SELECT * FROM devoluciones_evaluacion de WHERE de.supervisor='$this->idsupervisor' AND de.evaluado='$this->idevaluado' AND de.evaluacion='$this->idevaluacion'";
            $consulta = $this->db->query($sql);
            $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
            if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
              array_push($datos, $consulta);
              http_response_code(OK);
              return $datos;
            }
            else {
              http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
              return "consulta vacia";
            }
          }
          else {
            http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
            return "consulta vacia";
          }
        }
        else {
          http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
          return "consulta vacia";
        }
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
   * Devuleve la comparación de la evaluacion de un empleado
   * 
   * @param $idevaluado
   * @param $idevaluacion
   * @return un objeto con toda la informacion del empleado.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
    */  
  public function getCompararEvaluacion() // $idevaluado, $idevaluacion
  {
    try {
      $datos = array();

      // competencias
      $sql = "SELECT ce.id, ce.competencia
              FROM competencias_evaluacion ce, modelo_evaluacion me, evaluacion e
              WHERE me.competencia=ce.id AND me.evaluacionid=e.modelo AND e.id='$this->idevaluacion'
              GROUP BY ce.id ORDER BY ce.id ASC";

      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
        array_push($datos, $consulta);
        
        // supervisores
        $sql = "SELECT t1.supervisorid, t1.supervisor, avg(t1.puntaje) AS promedio
                FROM(
                SELECT pe.id, ce.competencia, ce.id AS idcompetencia, me.comportamiento, pe.supervisor AS supervisorid, pe.puntaje, concat(e.apellido,', ',e.nombre) AS supervisor
                FROM competencias_evaluacion ce, modelo_evaluacion me, puntajes_evaluacion pe, empleados e
                WHERE ce.id=me.competencia AND me.id=pe.modeloid AND pe.evaluacion='$this->idevaluacion' AND pe.evaluado='$this->idevaluado' AND e.id=pe.supervisor AND pe.ignorar='F')
                AS t1
                GROUP BY t1.supervisor ORDER BY promedio DESC";
        $consulta = $this->db->query($sql);
        $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
        if (count($consulta) >= 1 && ($consulta[0]["supervisorid"]!=null)) {
          array_push($datos, $consulta);

          // puntajes
          $sql = "SELECT t1.supervisor,t1.competencia,t1.idcompetencia, avg(t1.puntaje) AS promedio
          FROM(
          SELECT pe.id, ce.competencia, ce.id AS idcompetencia, me.comportamiento, pe.supervisor, pe.puntaje
          FROM competencias_evaluacion ce, modelo_evaluacion me, puntajes_evaluacion pe
          WHERE ce.id=me.competencia AND me.id=pe.modeloid AND pe.evaluacion='$this->idevaluacion' AND pe.evaluado='$this->idevaluado' AND pe.ignorar='F'
          )AS t1
          GROUP BY t1.supervisor,t1.competencia ORDER BY t1.idcompetencia ASC";
          $consulta = $this->db->query($sql);
          $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
          if (count($consulta) >= 1 && ($consulta[0]["supervisor"]!=null)) {
            array_push($datos, $consulta);
            // devoluciones
            $sql = "SELECT de.id, de.supervisor AS idsupervisor, concat(e.apellido,' ',e.nombre) AS supervisor, de.positivo, de.negativo
                    FROM devoluciones_evaluacion de INNER JOIN empleados e ON e.id=de.supervisor
                    WHERE de.evaluacion='$this->idevaluacion' AND de.evaluado='$this->idevaluado'";
            $consulta = $this->db->query($sql);
            $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
            if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
              array_push($datos, $consulta);
              http_response_code(OK);
              return $datos;
            }
            else {
              http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
              return "consulta vacia";
            }
          }
          else {
            http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
            return "consulta vacia";
          }
        }
        else {
          http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
          return "consulta vacia";
        }
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
   * Devuleve las devoluciones de una evaluacion de un empleado
   * 
   * @param $idevaluado
   * @param $idevaluacion
   * @return array con los empleados.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getDevolucionEvaluacion() // $idevaluado, $idevaluacion
  {
    try {
      $sql = "SELECT d.id, d.positivo, d.negativo, d.fecha_generada, d.rtaempleado, d.fecha_entrega, d.entregada, d.supervisor, CONCAT(s.nombre,' ',s.apellido) AS nombresup,
              CONCAT(e.nombre,' ',e.apellido) AS nombreempleado
              FROM devolucion_final d, empleados e, empleados s
              WHERE d.empleado='$this->idevaluado' AND d.evaluacion='$this->idevaluacion' AND e.id=d.empleado AND s.id=d.supervisor";

      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
        http_response_code(OK);
        return $consulta[0];
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

  public function getReporteExcel() {
    try {
      $datos = array();

      //modelo
      $sql = "SELECT s2.id, s2.idcompetencia, s2.competencia, s2.idcomportamiento, s2.comportamiento, s1.promedio
              FROM
              (
                SELECT p.modeloid, AVG(p.puntaje) AS promedio
                FROM comportamientos_evaluacion ce, modelo_evaluacion m, competencias_evaluacion c, evaluacion e, puntajes_evaluacion p
                WHERE e.id='$this->idevaluacion' AND ce.id=m.comportamiento AND m.competencia=c.id AND e.modelo=m.evaluacionid AND p.evaluacion=e.id AND p.evaluado='$this->idevaluado' AND p.ignorar='F'
                GROUP BY p.modeloid
              ) AS s1
              LEFT JOIN
              (
                SELECT c.id AS idcompetencia, c.competencia, m.id, ce.id AS idcomportamiento, ce.comportamiento
                FROM comportamientos_evaluacion ce, modelo_evaluacion m, competencias_evaluacion c, evaluacion e, puntajes_evaluacion p
                WHERE e.id='$this->idevaluacion' AND ce.id=m.comportamiento AND m.competencia=c.id AND e.modelo=m.evaluacionid AND p.evaluacion=e.id AND p.evaluado='$this->idevaluado'
                GROUP BY m.id
              ) AS s2
              ON s1.modeloid=s2.id";

      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
        array_push($datos, $consulta);
        // competencias
        $sql = 	"SELECT m.id, c.competencia
                  FROM modelo_evaluacion m, competencias_evaluacion c, evaluacion e
                  WHERE m.competencia=c.id AND m.evaluacionid=e.modelo AND e.id='$this->idevaluacion'
                  GROUP BY m.competencia";

        $consulta = $this->db->query($sql);
				$consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
        if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
          array_push($datos, $consulta);
         
          // supervisores
          $sql = 	"SELECT p.supervisor AS idsup, CONCAT(sup.nombre,' ',sup.apellido) AS nombresupervisor,
                    CONCAT(SUBSTRING(sup.nombre,1,1),'.',SUBSTRING(sup.apellido,1,1),'.') AS supacortado, AVG(p.puntaje) AS promedio, sup.apellido
                    FROM puntajes_evaluacion p, empleados sup, modelo_evaluacion m
                    WHERE p.evaluacion='$this->idevaluacion' AND p.evaluado='$this->idevaluado' AND sup.id=p.supervisor AND p.ignorar='F'
                    GROUP BY p.supervisor";
  
          $consulta = $this->db->query($sql);
          $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
         
          if (count($consulta) >= 1 && ($consulta[0]["idsup"]!=null)) {
            array_push($datos, $consulta);
            // puntajes
            $sql = 	"SELECT p.id AS idpe, p.supervisor AS idsup, c.id AS idcompetencia, p.modeloid, ce.id AS idcomportamiento, p.puntaje, p.ignorar
                    FROM puntajes_evaluacion p, empleados sup, comportamientos_evaluacion ce , modelo_evaluacion m, competencias_evaluacion c
                    WHERE p.evaluacion='$this->idevaluacion' AND p.evaluado='$this->idevaluado' AND sup.id=p.supervisor AND ce.id=m.comportamiento AND m.competencia=c.id AND p.modeloid=m.id";

            $consulta = $this->db->query($sql);
            $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
            if (count($consulta) >= 1 && ($consulta[0]["idpe"]!=null)) {
              array_push($datos, $consulta);
              
              $competencias = $datos[1];
							$comportamientos = $datos[0];
							$supervisores = $datos[2];
							//echo json_encode($datos);
							// OBTUVE TODOS LOS DATOS
							// LOS PROCESO EN UN EXCEL

							// Se crea el objeto PHPExcel
							$objPHPExcel = new PHPExcel();

							// Se asignan las propiedades del libro
							$objPHPExcel->getProperties()->setCreator("Ocean Servicios OnLine") // Nombre del autor
								->setLastModifiedBy("Ocean Servicios OnLine") //Ultimo usuario que lo modificó
								->setTitle($this->nombreExcel) // Titulo
								->setSubject("Reporte evaluacion de empleado") //Asunto
								->setDescription("Reporte de empleado") //Descripción
								->setKeywords("reporte empleado evaluacion") //Etiquetas
								->setCategory("Reporte excel"); //Categorias

							//$tituloReporte = "Ev. Operadores - SEPTIEMBRE 2017 -22-05-2017";
							$titulosColumnas = array('Competencias', 'Comportamientos observados');

							// Se combinan las celdas A1 hasta D1, para colocar ahí el titulo del reporte
							$objPHPExcel->setActiveSheetIndex(0)
								->mergeCells('A1:D1');
							// Se combinan las celdas A2 hasta D2, para colocar ahí los nombres de los supervisores
							$objPHPExcel->setActiveSheetIndex(0)
								->mergeCells('A2:D2');

							// Se agregan los titulos del reporte
							$objPHPExcel->setActiveSheetIndex(0)
								->setCellValue('A1', $this->tituloReporte) // Titulo del reporte
								->setCellValue('A4', $titulosColumnas[0])  //Titulo de las columnas
								->setCellValue('B4', $titulosColumnas[1]);

							// Se agregan los datos de los evaluadores como nombre de las columnas

							$cant_supervisores = count($supervisores);	//obtengo la cantidad de supervisores a colocar
							$auxSupervisores = '';
							for ($i=0; $i<$cant_supervisores; $i++) {
								$auxSupervisores.=($supervisores[$i]['nombresupervisor'].' - ');
							}
							$objPHPExcel->setActiveSheetIndex(0)->setCellValue('A2', $auxSupervisores);

							// Genero los datos faltantes
							for ($i = 0; $i < count($competencias); $i++) {
								$competencias[$i]['porcentaje'] = 0;
							}

							for ($i = 0; $i < count($competencias); $i++) {
								$cantidad = 0;
								for ($j = 0; $j < count($comportamientos); $j++) {
									if($competencias[$i]['competencia'] == $comportamientos[$j]['competencia'] && ($comportamientos[$j]['promedio']!=-1))
									{
										$cantidad++;
										$competencias[$i]['porcentaje'] += $comportamientos[$j]['promedio'];
									}
								}
								$competencias[$i]['porcentaje'] /= $cantidad;
							}

							$puntaje_final = 0;
							$cantidad = 0;
							for($i = 0; $i < count($comportamientos); $i++){
								if($comportamientos[$i]['promedio']!=-1)
								{
									$cantidad++;
									$puntaje_final += $comportamientos[$i]['promedio'];
								}
							}
							$puntaje_final /= $cantidad;

							// ESTILOS
							// Estilo competencias
							$estiloCompetencias = array(
								'font' => array(
									'bold'      => true,
									'strike'	=> false,
									'size'	=> 10
								),
								'borders' => array(
									'allborders' => array(
										'style' => PHPExcel_Style_Border::BORDER_MEDIUM	// Borde ancho: medio
									)
								),
								'alignment' => array(
									'horizontal' => PHPExcel_Style_Alignment::HORIZONTAL_CENTER,
									'vertical' => PHPExcel_Style_Alignment::VERTICAL_CENTER,
									'rotation' => 0,
									'wrap' => TRUE
								)
							);

							// Estilo para los renglones
							$estiloRenglones = array(
								'borders' => array(
									'bottom' => array(
										'style' => PHPExcel_Style_Border::BORDER_THIN	// Borde ancho: delgado
									)
								)
							);

							// Estilo palabra Puntaje
							$estiloPuntaje = array(
								'font' => array(
									'bold'      => true,
									'italic'	=> false,
									'strike'    => false,
									'size'		=> 10
								),
								'alignment' => array(
									'horizontal' => PHPExcel_Style_Alignment::HORIZONTAL_RIGHT,
									'vertical' => PHPExcel_Style_Alignment::VERTICAL_CENTER,
									'rotation' => 0,
									'wrap' => TRUE
								)
							);

							// Estilo declaracion
							$estiloDeclaracion = array(
								'font' => array(
									'bold'      => true,
									'italic'	=> false,
									'strike'    => false,
									'underline' => PHPExcel_Style_Font::UNDERLINE_SINGLE,
									'size'		=> 10
								),
								'alignment' => array(
									'horizontal' => PHPExcel_Style_Alignment::HORIZONTAL_LEFT,
									'vertical' => PHPExcel_Style_Alignment::VERTICAL_BOTTOM,
									'rotation' => 0,
									'wrap' => TRUE
								)
							);

							// Estilo firma
							$estiloFirma = array(
								'font' => array(
									'bold'      => true,
									'italic'	=> false,
									'strike'    => false,
									'size'		=> 10
								),
								'alignment' => array(
									'horizontal' => PHPExcel_Style_Alignment::HORIZONTAL_LEFT,
									'vertical' => PHPExcel_Style_Alignment::VERTICAL_BOTTOM,
									'rotation' => 0,
									'wrap' => TRUE
								)
							);

							// Estilo titulo evaluacion
							$estiloTituloReporte = array(
								'font' => array(
									'bold'      => true,
									'italic'	=> false,
									'strike'    => false,
									'size'		=> 12
								),
								'borders' => array(
									'allborders' => array(
										'style' => PHPExcel_Style_Border::BORDER_MEDIUM	// Borde ancho: medio
									)
								),
								'alignment' => array(
									'horizontal' => PHPExcel_Style_Alignment::HORIZONTAL_CENTER,
									'vertical' => PHPExcel_Style_Alignment::VERTICAL_CENTER,
									'rotation' => 0,
									'wrap' => TRUE
								)
							);

							// Estilo evaluadores
							$estiloEvaluadoresReporte = array(
								'font' => array(
									'strike'    => false,
									'size'	=> 10
								),
								'borders' => array(
									'allborders' => array(
										'style' => PHPExcel_Style_Border::BORDER_MEDIUM	// Borde ancho: medio
									)
								),
								'alignment' => array(
									'horizontal' => PHPExcel_Style_Alignment::HORIZONTAL_CENTER,
									'vertical' => PHPExcel_Style_Alignment::VERTICAL_CENTER,
									'rotation' => 0,
									'wrap' => TRUE
								)
							);

							// Estilo comportamientos
							$estiloComportamientos = array(
								'font' => array(
									'strike'    => false,
									'size'	=> 8
								),
								'borders' => array(
									'allborders' => array(
										'style' => PHPExcel_Style_Border::BORDER_THIN	// Borde ancho: delgado
									)
								),
								'alignment' => array(
									'horizontal' => PHPExcel_Style_Alignment::HORIZONTAL_CENTER,
									'vertical' => PHPExcel_Style_Alignment::VERTICAL_CENTER,
									'rotation' => 0,
									'wrap' => TRUE
								)
							);

							// Estilo bordes grupo comportamientos
							$estiloBordesComportamientos = array(
								'borders' => array(
									'outline' => array(
										'style' => PHPExcel_Style_Border::BORDER_MEDIUM	// Borde ancho: medio
									)
								)
							);

							// Aplico el estilo a los titulos de las columnas
							$objPHPExcel->getActiveSheet()->getStyle('A4:B4')->applyFromArray($estiloCompetencias);

							// COMPLETO CON LOS DATOS
							$fila_actual = '5';
							for($i=0; $i<count($competencias); $i++)	// competencias
							{
								$fila_inicial_merge = $fila_actual;	// Comienzo del merge para las filas de la competencia
								$comp = $competencias[$i];
								$objPHPExcel->setActiveSheetIndex(0)->setCellValue('A'.$fila_actual, $comp['competencia']);
								$objPHPExcel->setActiveSheetIndex(0)->setCellValue('D'.$fila_actual, $comp['porcentaje']);
								$objPHPExcel->getActiveSheet()->getStyle('D'.$fila_actual)->getNumberFormat()->setFormatCode(PHPExcel_Style_NumberFormat::FORMAT_NUMBER_00);
								for($j=0; $j<count($comportamientos); $j++)	// Comportamientos
								{
									$modelo = $comportamientos[$j];
									if($comp['competencia'] == $modelo['competencia'])
									{
										$objPHPExcel->setActiveSheetIndex(0)->setCellValue('B'.$fila_actual, $modelo['comportamiento']);
										$objPHPExcel->setActiveSheetIndex(0)->setCellValue('C'.$fila_actual, $modelo['promedio']);
										// Aplico el estilo
										$objPHPExcel->getActiveSheet()->getStyle('B'.$fila_actual.':'.'C'.$fila_actual)->applyFromArray($estiloComportamientos);
										// Estilo para numeros formato 0.00
										$objPHPExcel->getActiveSheet()->getStyle('C'.$fila_actual)->getNumberFormat()->setFormatCode(PHPExcel_Style_NumberFormat::FORMAT_NUMBER_00);
										$objPHPExcel->getActiveSheet()->getStyle('C'.$fila_actual)->getFont()->setSize(10);
										$fila_actual++;
									}
								}

								$fila_final_merge = ($fila_actual-1);	// Final del merge para las filas de la competencia
								// Se combinan las celdas Ax hasta Ay, para la competencia
								$objPHPExcel->setActiveSheetIndex(0)->mergeCells('A'.$fila_inicial_merge.':'.'A'.$fila_final_merge);
								$objPHPExcel->setActiveSheetIndex(0)->mergeCells('D'.$fila_inicial_merge.':'.'D'.$fila_final_merge);

								// le aplico el estilo de competencias
								$objPHPExcel->getActiveSheet()->getStyle('A'.$fila_inicial_merge.':'.'A'.$fila_final_merge)->applyFromArray($estiloCompetencias);
								$objPHPExcel->getActiveSheet()->getStyle('B'.$fila_inicial_merge.':'.'C'.$fila_final_merge)->applyFromArray($estiloBordesComportamientos);
								$objPHPExcel->getActiveSheet()->getStyle('D'.$fila_inicial_merge.':'.'D'.$fila_final_merge)->applyFromArray($estiloCompetencias);
								$objPHPExcel->getActiveSheet()->getStyle('D'.$fila_inicial_merge.':'.'D'.$fila_final_merge)->getFont()->setBold(false);
							}

							// Coloco el puntaje final
							// Coloco el texto
							$objPHPExcel->setActiveSheetIndex(0)->setCellValue('B'.$fila_actual, 'Puntaje final');
							// Le alpico el estilo
							$objPHPExcel->getActiveSheet()->getStyle('B'.$fila_actual)->applyFromArray($estiloPuntaje);
							// Uno las celdas para el puntaje
							$objPHPExcel->setActiveSheetIndex(0)->mergeCells('C'.$fila_actual.':'.'D'.$fila_actual);
							// Coloco el valor
							$objPHPExcel->setActiveSheetIndex(0)->setCellValue('C'.$fila_actual, $puntaje_final);
							$objPHPExcel->getActiveSheet()->getStyle('C'.$fila_actual)->getNumberFormat()->setFormatCode(PHPExcel_Style_NumberFormat::FORMAT_NUMBER_00);
							// Le aplico el formato al valor del puntaje
							$objPHPExcel->getActiveSheet()->getStyle('C'.$fila_actual.':'.'D'.$fila_actual)->applyFromArray($estiloCompetencias);

							// Coloco la declaracion de compromisos
							$fila_inicio_declaracion = $fila_actual+2;

							$objPHPExcel->setActiveSheetIndex(0)->mergeCells('A'.$fila_inicio_declaracion.':'.'D'.$fila_inicio_declaracion);
							$objPHPExcel->setActiveSheetIndex(0)->setCellValue('A'.$fila_inicio_declaracion, 'DECLARACION DE COMPROMISOS A ASUMIR POR COLABORADOR EVALUADO:');

							// Aplico el estilo
							$objPHPExcel->getActiveSheet()->getStyle('A'.$fila_inicio_declaracion)->applyFromArray($estiloDeclaracion);

							$fila_inicio_renglones = $fila_inicio_declaracion+1;	// Actualizo el inicio al principio de los renglones
							for($i=0; $i<16; $i++)
							{
								// Uno las celdas
								$objPHPExcel->setActiveSheetIndex(0)->mergeCells('A'.$fila_inicio_renglones.':'.'D'.$fila_inicio_renglones);
								// Aplico el estilo
								$objPHPExcel->getActiveSheet()->getStyle('A'.$fila_inicio_renglones.':'.'D'.$fila_inicio_renglones)->applyFromArray($estiloRenglones);
								$fila_inicio_renglones++;
							}

							$fila_firma = $fila_inicio_declaracion+20;
							$objPHPExcel->setActiveSheetIndex(0)->setCellValue('A'.$fila_firma, 'FIRMA PERSONAL EVALUADO');
							// Aplico el estilo
							$objPHPExcel->getActiveSheet()->getStyle('A'.$fila_firma)->applyFromArray($estiloFirma);

							// FORMATEO LA HOJA

							// Aplico el estilo al titulo
							$objPHPExcel->getActiveSheet()->getStyle('A1:D1')->applyFromArray($estiloTituloReporte);

							// Aplico el estilo al titulo
							$objPHPExcel->getActiveSheet()->getStyle('A2:D2')->applyFromArray($estiloEvaluadoresReporte);

							// Redimenciono las 2 primeras columnas y el alto de las filas
							$objPHPExcel->getActiveSheet()->getDefaultRowDimension()->setRowHeight(25);

							// se da formato de tamaño automatico
							/*$aux = 67+$cant_supervisores+2; // 67 = 'C'
							$aux = chr($aux);
							for($i = 'A'; $i <= $aux; $i++){*/
							for($i = 'A'; $i <= 'D'; $i++){
								$objPHPExcel->setActiveSheetIndex(0)->getColumnDimension($i)->setAutoSize(TRUE);
							}

							// Se asigna el nombre a la hoja
							$objPHPExcel->getActiveSheet()->setTitle('Evaluacion');

							// Se activa la hoja para que sea la que se muestre cuando el archivo se abre
							$objPHPExcel->setActiveSheetIndex(0);

							// Inmovilizar paneles
							//$objPHPExcel->getActiveSheet(0)->freezePane('A4');
							//$objPHPExcel->getActiveSheet(0)->freezePaneByColumnAndRow(0,4);



							// Se manda el archivo al navegador web, con el nombre que se indica, en formato 2007
							$objWriter = PHPExcel_IOFactory::createWriter($objPHPExcel, 'Excel2007');
							$objWriter->save('php://output');

							$file = "file.xlsx";
						 	header('Content-disposition: attachment; filename='.$file);
							header('Content-Length: '.filesize($file));
							header('Content-Transfer-Encoding: binary');
							header('Cache-Control: must-revalidate');
              header('Pragma: public');
              http_response_code(OK);
							return readfile($file);
            }
            else {
              http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
              return "consulta vacia";
            }
          }
          else {
            http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
            return "consulta vacia";
          }
        }
        else {
          http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
          return "consulta vacia";
        }
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
   * Devuleve el listado de evaluaciones
   * 
   * @param $idevaluado
   * @return array con los empleados.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getEvaluaciones() //
  {
    try {
      if($this->idsector == 1)
      {
        $sql = "SELECT e.id, e.modelo, e.titulo, e.area AS idsector, s.area AS sector, e.fecha, e.creadapor, e.activa,
              CASE e.activa
								WHEN 'SI' THEN 0
						    WHEN 'CE' THEN 1
								WHEN 'NO' THEN 2
								WHEN 'BO' THEN 3
							END AS orden
              FROM evaluacion e INNER JOIN sectores s ON s.id=e.area INNER JOIN evaluadores sup ON sup.evaluacion=e.id
              WHERE e.activa!='BO'
              GROUP BY e.id ORDER BY orden ASC, e.fecha DESC";  
      }
      else if($this->idsector == 13 || $this->idsector == 15 || $this->idusuario == 149 || $this->idusuario == 304)
      {
        $sql = "SELECT e.id, e.modelo, e.titulo, e.area AS idsector, s.area AS sector, e.fecha, e.creadapor, e.activa,
              CASE e.activa
                WHEN 'SI' THEN 0
                WHEN 'CE' THEN 1
                WHEN 'NO' THEN 2
                WHEN 'BO' THEN 3
              END AS orden
              FROM evaluacion e INNER JOIN sectores s ON s.id=e.area INNER JOIN evaluadores sup ON sup.evaluacion=e.id
              WHERE e.activa!='BO'
              GROUP BY e.id ORDER BY orden ASC, e.fecha DESC";
      }
      else{
        $sql = "SELECT e.id, e.modelo, e.titulo, e.area AS idsector, s.area AS sector, e.fecha, e.creadapor, e.activa,
              CASE e.activa
                WHEN 'SI' THEN 0
                WHEN 'CE' THEN 1
                WHEN 'NO' THEN 2
                WHEN 'BO' THEN 3
              END AS orden
              FROM evaluacion e INNER JOIN sectores s ON s.id=e.area INNER JOIN evaluadores sup ON sup.evaluacion=e.id
              WHERE sup.evaluador=$this->idusuario AND e.activa!='BO'
              GROUP BY e.id ORDER BY orden ASC, e.fecha DESC";
      }

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
   * Devuleve el resumen de una evaluacion
   * 
   * @param $idevaluacion  
   * @return array con los empleados.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getResumenEvaluacion() //
  {
    try {
      $sql = "SELECT e.id, e.modelo, e.titulo, e.area AS idsector, s.area AS sector, e.fecha, e.activa,
            CASE e.activa
              WHEN 'SI' THEN 0
              WHEN 'CE' THEN 1
              WHEN 'NO' THEN 2
              WHEN 'BO' THEN 3
            END AS orden
            FROM evaluacion e INNER JOIN sectores s ON s.id=e.area INNER JOIN evaluadores sup ON sup.evaluacion=e.id
            WHERE e.id=$this->idevaluacion
            GROUP BY e.id";
      

      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
        http_response_code(OK);
        return $consulta[0];
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
   * Devuleve el resumen de una evaluacion
   * 
   * @param $idevaluacion  
   * @return array con los empleados.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getRanking() //
  {
    try {
      $datos = array();

      // competencias
      $sql = "SELECT ce.id, ce.competencia
      FROM competencias_evaluacion ce, modelo_evaluacion me, evaluacion e
      WHERE me.competencia=ce.id AND me.evaluacionid=e.modelo AND e.id='$this->idevaluacion'
      GROUP BY ce.id ORDER BY ce.id ASC";

      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
        array_push($datos, $consulta);
        
        // evaluados
        $sql = "SELECT t1.evaluadoid, t1.evaluado, avg(t1.puntaje) AS promedio
                FROM(
                SELECT pe.id, ce.competencia, ce.id AS idcompetencia, me.comportamiento, pe.evaluado AS evaluadoid, pe.puntaje, concat(e.apellido,', ',e.nombre) AS evaluado
                FROM competencias_evaluacion ce, modelo_evaluacion me, puntajes_evaluacion pe, empleados e
                WHERE ce.id=me.competencia AND me.id=pe.modeloid AND pe.evaluacion='$this->idevaluacion' AND pe.supervisor='$this->idsupervisor' AND e.id=pe.evaluado AND pe.ignorar='F')
                AS t1
                GROUP BY t1.evaluado ORDER BY promedio DESC";
        $consulta = $this->db->query($sql);
        $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
        if (count($consulta) >= 1 && ($consulta[0]["evaluadoid"]!=null)) {
          array_push($datos, $consulta);

          // puntajes
          $sql = "SELECT t1.evaluado,t1.competencia,t1.idcompetencia, avg(t1.puntaje) AS promedio
                  FROM(
                  SELECT pe.id, ce.competencia, ce.id AS idcompetencia, me.comportamiento, pe.evaluado, pe.puntaje
                  FROM competencias_evaluacion ce, modelo_evaluacion me, puntajes_evaluacion pe
                  WHERE ce.id=me.competencia AND me.id=pe.modeloid AND pe.evaluacion='$this->idevaluacion' AND pe.supervisor='$this->idsupervisor' AND pe.ignorar='F')
                  AS t1
                  GROUP BY t1.evaluado,t1.competencia ORDER BY t1.idcompetencia ASC";
          $consulta = $this->db->query($sql);
          $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
          if (count($consulta) >= 1 && ($consulta[0]["evaluado"]!=null)) {
            array_push($datos, $consulta);
            http_response_code(OK);
            return $datos;            
          }
          else {
            http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
            return "consulta vacia";
          }
        }
        else {
          http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
          return "consulta vacia";
        }
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
   * Devuleve las devoluciones de una evaluacion de un empleado
   * 
   * @param $idevaluado
   * @param $idevaluacion
   * @return array con los empleados.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getListadoVerComoSupervisor() // $idevaluacion, $idevaluador
  {
    try {
      $sql = "SELECT e.id, CONCAT(e.apellido,', ',e.nombre) as nombre, s.area AS sector
              FROM empleados e INNER JOIN sectores s ON s.id=e.sector
              WHERE e.empresa=1 AND e.activo='SI' AND e.id IN (
                SELECT evaluador FROM evaluadores WHERE evaluacion=$this->idevaluacion AND evaluador!=$this->idsupervisor GROUP BY evaluador
              )
              ORDER BY e.apellido ASC";

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
   * Devuleve las devoluciones de una evaluacion de un empleado
   * 
   * @param $idevaluado
   * @param $idevaluacion
   * @return array con los empleados.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getEvaluados() // $idevaluacion, $idevaluador
  {
    try {
      $sql = "SELECT e.id AS empleadoid, concat(e.apellido,', ',e.nombre) AS nombre, IF(e.foto IS NULL OR e.foto = '', 'img/user.jpg', e.foto) AS foto
              FROM empleados e
              WHERE e.activo='SI' AND e.id!='$this->idsupervisor'
              AND e.id IN (SELECT pe0.evaluado AS id FROM puntajes_evaluacion pe0 WHERE pe0.supervisor='$this->idsupervisor' AND pe0.evaluacion='$this->idevaluacion' GROUP BY evaluado)
              ORDER BY nombre ASC";

      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["empleadoid"]!=null)) {
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
   * Devuleve el modelo de una evaluacion
   * 
   * @param $idevaluacion
   * @return un objeto con toda la informacion del modelo.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
    */  
  public function getModelo() // $idevaluacion
  {
    try {
      $datos = array();

      // modelo
      $sql = "SELECT m.id, c.competencia, m.comportamiento AS idcomportamiento, ce.comportamiento
              FROM modelo_evaluacion m, competencias_evaluacion c, comportamientos_evaluacion ce
              WHERE m.competencia=c.id AND ce.id=m.comportamiento AND m.evaluacionid='$this->idmodelo'";

      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
        array_push($datos, $consulta);
        
        // competencias
        $sql = "SELECT m.id, c.competencia
                FROM modelo_evaluacion m, competencias_evaluacion c
                WHERE m.competencia=c.id AND m.evaluacionid='$this->idmodelo'
                GROUP BY m.competencia";

        $consulta = $this->db->query($sql);
        $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
        if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
          array_push($datos, $consulta);
          http_response_code(OK);
          return $datos;
        }
        else {
          http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
          return "consulta vacia";
        }
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
   * Devuleve los empleados que se pueden evaluar
   * 
   * @param $idevaluacion
   * @param $idsector
   * @param $idsupervisor
   * @return array con los empleados.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getEmpleadosAEvaluar() //
  {
    try {
      $sql = "SELECT e.id, concat(e.apellido,', ',e.nombre) AS nombre, e.sector AS idsector, s.area AS sector,
                IF(e.foto IS NULL OR e.foto = '', 'img/user.jpg', e.foto) AS foto
              FROM empleados e INNER JOIN sectores s ON s.id=e.sector
              WHERE e.sector=$this->idsector AND e.activo='SI' AND e.id!='$this->idsupervisor'
              AND e.id NOT IN (
                SELECT pe0.evaluado AS id FROM puntajes_evaluacion pe0 WHERE pe0.supervisor='$this->idsupervisor' AND pe0.evaluacion='$this->idevaluacion'
                GROUP BY evaluado
              )
              ORDER BY nombre ASC";
      

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
   * Devuleve el resumen de una evaluacion
   * 
   * @param $idevaluacion  
   * @return array con los empleados.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getRankingGlobal() //
  {
    try {
      $datos = array();
      $subdatos = array();

      // listado de todos los supervisores para una evaluacion
      $sql = "SELECT pe.supervisor AS idsupervisor, CONCAT(e.apellido,', ', e.nombre) AS supervisor
              FROM puntajes_evaluacion pe INNER JOIN empleados e ON e.id=pe.supervisor
              WHERE pe.evaluacion='$this->idevaluacion'
              GROUP BY pe.supervisor";

      $supervisores = $this->db->query($sql);
      $supervisores = $supervisores->fetchAll(PDO::FETCH_ASSOC);
      if (count($supervisores) >= 1 && ($supervisores[0]["idsupervisor"]!=null)) {
        array_push($datos, $supervisores);
        
        // ranking final para una evaluacion
        $sql = "SELECT s2.evaluadoid, s2.evaluado, AVG(s2.promedio) AS promedio
                FROM
                (
                  SELECT s1.comportamiento, s1.evaluadoid, s1.evaluado, AVG(s1.puntaje) AS promedio
                  FROM
                  (
                    SELECT pe.id, me.comportamiento, pe.evaluado AS evaluadoid, pe.puntaje, concat(e.apellido,', ',e.nombre) AS evaluado,
                      pe.supervisor AS idsupervisor, concat(sup.apellido,', ',sup.nombre) AS supervisor
                    FROM  modelo_evaluacion me, puntajes_evaluacion pe, empleados e, empleados sup
                    WHERE me.id=pe.modeloid AND pe.evaluacion='$this->idevaluacion' AND e.id=pe.evaluado AND pe.supervisor=sup.id AND pe.ignorar='F'
                  ) AS s1
                  GROUP BY s1.evaluadoid, s1.comportamiento
                ) AS s2
                GROUP BY s2.evaluadoid ORDER BY promedio DESC";
        $final = $this->db->query($sql);
        $final = $final->fetchAll(PDO::FETCH_ASSOC);
        if (count($final) >= 1 && ($final[0]["evaluadoid"]!=null)) {
          array_push($datos, $final);

          // ranking para un supervisor especifico para una evaluacion
          $cant = count($supervisores);	// obtengo la cantidad de supervisores
          for($i = 0; $i < $cant; $i++)
	        {
            $supervisor = $supervisores[$i]["idsupervisor"];
            $sql = "SELECT t1.evaluadoid, t1.evaluado, avg(t1.puntaje) AS promedio, t1.idsupervisor, t1.supervisor
                    FROM(
                      SELECT pe.id, me.comportamiento, pe.evaluado AS evaluadoid, pe.puntaje, concat(e.apellido,', ',e.nombre) AS evaluado,
                        pe.supervisor AS idsupervisor, concat(sup.apellido,', ',sup.nombre) AS supervisor
                      FROM modelo_evaluacion me, puntajes_evaluacion pe, empleados e, empleados sup
                      WHERE me.id=pe.modeloid AND pe.evaluacion='$this->idevaluacion' AND pe.supervisor='$supervisor' AND e.id=pe.evaluado AND pe.supervisor=sup.id AND pe.ignorar='F'
                    )AS t1
                    GROUP BY t1.evaluado ORDER BY promedio DESC";

            $subranking = $this->db->query($sql);
            $subranking = $subranking->fetchAll(PDO::FETCH_ASSOC);
            if (count($subranking) >= 1 && ($subranking[0]["evaluadoid"]!=null)) {
              array_push($subdatos, $subranking);
            }
            else {
              http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
              return "consulta vacia";
            }
          }
          array_push($datos, $subdatos);
          http_response_code(OK);
          return $datos;
        }
        else {
          http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
          return "consulta vacia";
        }
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
   * Devuleve el modelo de una evaluacion
   * 
   * @param $idevaluacion
   * @return un objeto con toda la informacion del modelo.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */  
  public function getCompetenciasYComportamientos() // $idevaluacion
  {
    try {
      $datos = array();

      // competencias
      $sql = "SELECT * FROM competencias_evaluacion";

      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
        array_push($datos, $consulta);
        
        // comportamientos
        $sql = "SELECT * FROM comportamientos_evaluacion";

        $consulta = $this->db->query($sql);
        $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
        if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
          array_push($datos, $consulta);
          http_response_code(OK);
          return $datos;
        }
        else {
          http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
          return "consulta vacia";
        }
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
   * Devuleve los posibles usuarios y areas a los que enviar un correo
   * 
   * @param $correorestringido
   * @param $idacceso
   * @return array de objetos con los usuarios array[0] y las areas array[1]
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getAreasYSupervisores() // $correorestringido, $idacceso
  {
    try {
      $datos = array();

      $q = "SELECT u.id, concat(nombre,' ',apellido) AS usuario, a.area AS area
            FROM empleados u, sectores a
            WHERE u.activo='SI' AND u.sector=a.id AND u.acceso IN ($this->listaPermitidos)
            ORDER BY usuario ASC";

      $lista = $this->db->query($q);
      $lista = $lista->fetchAll(PDO::FETCH_ASSOC);

      $q = "SELECT s.id, s.area
            FROM empleados e INNER JOIN sectores s ON e.sector = s.id
            WHERE e.acceso IN ($this->listaPermitidos) AND e.activo='SI'
            GROUP BY s.id ORDER BY s.area ASC";

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
    } catch (Exception $e) {
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
  public function getPlantilla() // $correorestringido, $idacceso
  {
    try {
      $datos = array();

      $sql = "SELECT m.competencia AS idcompe, c.competencia AS competencia, m.comportamiento AS idcompor, ce.comportamiento AS comportamiento
            FROM modelo_evaluacion m, competencias_evaluacion c, comportamientos_evaluacion ce
            WHERE m.competencia=c.id AND ce.id=m.comportamiento AND m.evaluacionid='$this->idmodelo'";

      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);

      if (count($consulta) >= 1 && ($consulta[0]["idcompe"]!=null)){
        array_push($datos, $consulta);

        $sql = "SELECT m.competencia AS idcompe, c.competencia AS competencia
                FROM modelo_evaluacion m, competencias_evaluacion c
                WHERE m.competencia=c.id AND m.evaluacionid='$this->idmodelo'
                GROUP BY c.competencia";
  
        $consulta = $this->db->query($sql);
        $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);

        if(count($consulta) >= 1 && ($consulta[0]["idcompe"]!=null)){
          array_push($datos, $consulta);
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
    } catch (Exception $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }
  }

  /**
   * Devuleve los empleados que se pueden evaluar
   * 
   * @param $idevaluacion
   * @param $idsector
   * @param $idsupervisor
   * @return array con los empleados.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getListaDePlantillas() //
  {
    try {
      $sql = "SELECT e.id, e.titulo AS nombre, e.modelo
              FROM evaluacion e WHERE e.activa !='BO'
              GROUP BY e.modelo
              ORDER BY e.id DESC";      

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
   * Archiva o desarchiva una evaluacion
   * 
   * @param $idevaluacion
   * @param $activa
   * @return OK consulta exitosa.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function setArchivar() //$idevaluacion, $activa
  {
    try {
      if($this->activa == 'SI')
      {
        $this->activa = 'NO';
      }
      else{
        $this->activa = 'SI';
      }
      $sql = "UPDATE evaluacion SET activa=:activa WHERE id=:id";
      $stmt = $this->db->prepare($sql);
      $stmt->bindParam(':id', $this->idevaluacion);
      $stmt->bindParam(':activa', $this->activa);
      if ($stmt->execute())
      {
        $this->logger->info("archivo/desarchivo (" . $this->activa . ") la evaluacion: " . $this->idevaluacion, $this->ARCHIVO, __LINE__);
        http_response_code(OK);
        return "ok";
      }else {
        $this->logger->fatal("hubo un fallo en la consulta", $this->ARCHIVO, __LINE__);
        http_response_code(INTERNAL_SERVER_ERROR);
        return "fallo consulta";
      } 
    } catch (\PDOException $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }    
  }

  /**
   * Abrir o cerrar una evaluacion
   * 
   * @param $idevaluacion
   * @param $activa
   * @return OK consulta exitosa.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function setAbrirCerrar() //$idevaluacion, $activa
  {
    try {
      if($this->activa == 'SI')
      {
        $this->activa = 'CE';
      }
      else{
        $this->activa = 'SI';
      }
      $sql = "UPDATE evaluacion SET activa=:activa WHERE id=:id";
      $stmt = $this->db->prepare($sql);
      $stmt->bindParam(':id', $this->idevaluacion);
      $stmt->bindParam(':activa', $this->activa);
      if ($stmt->execute())
      {
        $this->logger->info("abrio/cerro (" . $this->activa . ") la evaluacion: " . $this->idevaluacion, $this->ARCHIVO, __LINE__);
        http_response_code(OK);
        return "ok";
      }else {
        $this->logger->fatal("hubo un fallo en la consulta", $this->ARCHIVO, __LINE__);
        http_response_code(INTERNAL_SERVER_ERROR);
        return "fallo consulta";
      } 
    } catch (\PDOException $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }    
  }

  /**
   * Borra una evaluacion
   * 
   * @param $idevaluacion
   * @return string ok.
   * @return OK consulta exitosa.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function deleteEvaluacion()  // $idevaluacion
  {
    try {
      $activa = 'BO';
      $q = "UPDATE evaluacion SET activa=:activa WHERE id=:id";
      $stmt = $this->db->prepare($q);
      $stmt->bindParam(':id', $this->idevaluacion);
      $stmt->bindParam(':activa', $activa);
      $stmt->execute();
      if($stmt) {
        $this->logger->info("borro la evaluacion: " . $this->idevaluacion, $this->ARCHIVO, __LINE__);
        http_response_code(OK);
        return "OK";
      } else {
        $this->logger->fatal("hubo un fallo en la consulta", $this->ARCHIVO, __LINE__);
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
   * Devuleve los empleados que se pueden evaluar
   * 
   * @param $idevaluacion
   * @param $idsector
   * @param $idsupervisor
   * @return array con los empleados.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function pushEvaluacion() //
  {
    try {
      $puntaje = null;
      $ignorar = null;
      $idmodelo = null;
      $id = null;
     
      if ($this->editar == false) {
        // corroboro que no exista la evaluacion para evitar duplicados
        $check = $this->db->query("SELECT * FROM puntajes_evaluacion WHERE evaluacion='$this->idevaluacion' AND supervisor='$this->idsupervisor' AND evaluado='$this->idevaluado' ");
        $check = $check->fetchAll();
      
        if (count($check) >= 1) {// && ($chek[0]["id"]!=null)){
          http_response_code(CONFLICT);
          return "ya existe una evaluacion de ese empleado";
        } else {
          $this->db->beginTransaction();

          $stmt = $this->db->prepare("INSERT INTO puntajes_evaluacion (supervisor, evaluado, evaluacion, modeloid, puntaje, ignorar) VALUES (:supervisor, :evaluado, :evaluacion, :modeloid, :puntaje, :ignorar)");
          $stmt->bindParam(':puntaje', $puntaje);
          $stmt->bindParam(':ignorar', $ignorar);
          $stmt->bindParam(':modeloid', $idmodelo);
          $stmt->bindParam(':evaluacion', $this->idevaluacion);
          $stmt->bindParam(':supervisor', $this->idsupervisor);
          $stmt->bindParam(':evaluado', $this->idevaluado);

          $cant = count($this->modelo);
          for ($i=0; $i < $cant; $i++) { 
            $puntaje = $this->modelo[$i]['puntaje'];
            $idmodelo = $this->modelo[$i]['id'];
            $ignorar = $this->modelo[$i]['ignorar'];
            $stmt->execute();
          }

          $stmt = $this->db->prepare("INSERT INTO devoluciones_evaluacion (supervisor, evaluado, evaluacion, positivo, negativo) VALUES (:supervisor, :evaluado, :evaluacion, :positivo, :negativo)");
          $stmt->bindParam(':negativo', $this->negativo);
          $stmt->bindParam(':positivo', $this->positivo);
          $stmt->bindParam(':evaluacion', $this->idevaluacion);
          $stmt->bindParam(':supervisor', $this->idsupervisor);
          $stmt->bindParam(':evaluado', $this->idevaluado);
          $stmt->execute();
          // commit the transaction
          $this->db->commit();

          http_response_code(OK);
          return 'ok';
          // TODO: verificar fallo del commit
          /* 
          else {
            http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
            return "consulta vacia";
          } */
        }
      } else {
        $this->db->beginTransaction();

        $stmt = $this->db->prepare("UPDATE puntajes_evaluacion SET puntaje = :puntaje, ignorar = :ignorar WHERE supervisor = :supervisor AND evaluado = :evaluado AND evaluacion = :evaluacion AND id = :id");
        $stmt->bindParam(':puntaje', $puntaje);
        $stmt->bindParam(':ignorar', $ignorar);
        $stmt->bindParam(':id', $id);
        $stmt->bindParam(':evaluacion', $this->idevaluacion);
        $stmt->bindParam(':supervisor', $this->idsupervisor);
        $stmt->bindParam(':evaluado', $this->idevaluado);
        
        $cant = count($this->modelo);	// obtengo la cantidad de areas destino
        for($i = 0; $i < $cant; $i++)
        {
          $puntaje = $this->modelo[$i]['puntaje'];
          $ignorar = $this->modelo[$i]['ignorar'];
          $id = $this->modelo[$i]['puntajeid'];
          $stmt->execute();
        }

        $stmt = $this->db->prepare("UPDATE devoluciones_evaluacion SET positivo = :positivo, negativo = :negativo WHERE supervisor = :supervisor AND evaluado = :evaluado AND evaluacion = :evaluacion AND id = :id");
    		$stmt->bindParam(':negativo', $this->negativo);
    		$stmt->bindParam(':positivo', $this->positivo);
        $stmt->bindParam(':id', $this->iddevolucion);
        $stmt->bindParam(':evaluacion', $this->idevaluacion);
        $stmt->bindParam(':supervisor', $this->idsupervisor);
        $stmt->bindParam(':evaluado', $this->idevaluado);
        $stmt->execute();

        $this->db->commit();

        http_response_code(OK);
        return 'ok';
      }
    } catch (\PDOException $e) {
      $this->db->rollback();
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }    
  }

  /**
   * Devuleve los empleados que se pueden evaluar
   * 
   * @param $idevaluacion
   * @param $idsector
   * @param $idsupervisor
   * @return array con los empleados.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function pushDevolucion() //
  {
    try {
      if ($this->iddevolucion > 0) {
        $sql = "UPDATE devolucion_final SET	evaluacion=:evaluacion, empleado=:empleado, supervisor=:supervisor, positivo=:positivo, negativo=:negativo,
                  rtaempleado=:rtaempleado, fecha_entrega=CURRENT_TIMESTAMP(), entregada=:entregada
                WHERE id=:id";

        $stmt = $this->db->prepare($sql);
        $stmt->bindParam(':id', $this->iddevolucion);
        $stmt->bindParam(':evaluacion', $this->idevaluacion);
        $stmt->bindParam(':empleado', $this->idevaluado);
        $stmt->bindParam(':supervisor', $this->idsupervisor);
        $stmt->bindParam(':negativo', $this->negativo);
        $stmt->bindParam(':positivo', $this->positivo);
        $stmt->bindParam(':rtaempleado', $this->rtaEmpleado);
        $stmt->bindParam(':entregada', $this->entregada);
        
        $stmt->execute();
        
        if($stmt) {
          http_response_code(OK);
          return "OK";
        } else {
          http_response_code(INTERNAL_SERVER_ERROR);
          return "fallo en la consulta";
        }  

      } else {
        // corroboro que no exista la evaluacion para evitar duplicados
        $check = $this->db->query("SELECT * FROM devolucion_final WHERE evaluacion='$this->idevaluacion' AND supervisor='$this->idsupervisor' AND empleado='$this->idevaluado' ");
        $check = $check->fetchAll();
        if (count($check) >= 1) {// && ($chek[0]["id"]!=null)){
          http_response_code(CONFLICT);
          return "ya existe una evaluacion de ese empleado";
        } else{
          $sql = "INSERT INTO devolucion_final (evaluacion, empleado, supervisor, positivo, negativo, rtaempleado, entregada, fecha_entrega)
					        VALUES (:evaluacion, :empleado, :supervisor, :positivo, :negativo, :rtaempleado, :entregada, CURRENT_TIMESTAMP())";

          $stmt = $this->db->prepare($sql);
          $stmt->bindParam(':evaluacion', $this->idevaluacion);
          $stmt->bindParam(':empleado', $this->idevaluado);
          $stmt->bindParam(':supervisor', $this->idsupervisor);
          $stmt->bindParam(':negativo', $this->negativo);
          $stmt->bindParam(':positivo', $this->positivo);
          $stmt->bindParam(':rtaempleado', $this->rtaEmpleado);
          $stmt->bindParam(':entregada', $this->entregada);

          $stmt->execute();

          if($stmt) {
            http_response_code(OK);
            return "OK";
          } else {
            $this->logger->fatal("hubo un fallo en la consulta", $this->ARCHIVO, __LINE__);
            http_response_code(INTERNAL_SERVER_ERROR);
            return "fallo en la consulta";
          }  
        }
      }
    } catch (\PDOException $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }    
  }

  /**
   * Devuleve los empleados que se pueden evaluar
   * 
   * @param $idevaluacion
   * @param $idsector
   * @param $idsupervisor
   * @return array con los empleados.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function pushNuevoComp() //
  {
    try {
      if ($this->tabla == 'Comportamiento') {
        $sql = "INSERT INTO comportamientos_evaluacion (comportamiento) VALUES (:comportamiento)";

        $stmt = $this->db->prepare($sql);
        $stmt->bindParam(':comportamiento', $this->texto);
        
        $stmt->execute();
        
        if($stmt) {
          http_response_code(OK);
          return "OK";
        } else {
          http_response_code(INTERNAL_SERVER_ERROR);
          return "fallo en la consulta";
        }  

      } else {
        $sql = "INSERT INTO competencias_evaluacion (competencia) VALUES (:competencia)";

        $stmt = $this->db->prepare($sql);
        $stmt->bindParam(':competencia', $this->texto);
        
        $stmt->execute();
        
        if($stmt) {
          http_response_code(OK);
          return "OK";
        } else {
          $this->logger->fatal("hubo un fallo en la consulta", $this->ARCHIVO, __LINE__);
          http_response_code(INTERNAL_SERVER_ERROR);
          return "fallo en la consulta";
        }  
      }
    } catch (\PDOException $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }    
  }

  /**
   * Devuleve los empleados que se pueden evaluar
   * 
   * @param $idevaluacion
   * @param $idsector
   * @param $idsupervisor
   * @return array con los empleados.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function pushNuevaEvaluacion() //
  {
    try {
      //obtengo el ultimo numero de evaluacion en caso de que no se use plantilla y haya que generarla
      if($this->usarplantilla == false)
			{
				$sql = "SELECT * FROM modelo_evaluacion ORDER BY evaluacionid DESC LIMIT 1";
				$consulta = $this->db->query($sql);
				$consulta = $consulta->fetchAll();
				$numeromodelo = $consulta[0]['evaluacionid'];
				$numeromodelo = $numeromodelo+1;
			}			
     
      $idevaluaciones = array();	// arreglo para contener el id de las evaluaciones creadas
      
      $this->db->beginTransaction();

      $area = null;
      // inserto en la tabla evaluacion
      $stmt = $this->db->prepare("INSERT INTO evaluacion (modelo, titulo, area, creadapor) VALUES (:modelo, :titulo, :area, :creadapor)");
      $stmt->bindParam(':titulo', $this->nombreevaluacion);
      $stmt->bindParam(':area', $area);
      $stmt->bindParam(':creadapor', $this->idusuario);

      if($this->usarplantilla == true) {
        $stmt->bindParam(':modelo', $this->numeroplantilla);
      } else {
        $stmt->bindParam(':modelo', $numeromodelo);
      }
      
      $cant = count($this->destinos);
      for ($i=0; $i < $cant; $i++) { 
        $area = $this->destinos[$i]['id'];
        $stmt->execute();
        array_push($idevaluaciones, $this->db->lastInsertId());
      }

      // si no se usa una plantilla preexistente genero una y la inserto en la tabla modelo_evaluacion
      if ($this->usarplantilla == false) {
        $competencia = null;
        $comportamiento = null;
        $cant = count($this->modelo); // obtengo la cantidad de registros que componen la plantilla
        $stmt = $this->db->prepare("INSERT INTO modelo_evaluacion (evaluacionid, competencia, comportamiento) VALUES (:evaluacionid, :competencia, :comportamiento)");
				$stmt->bindParam(':evaluacionid', $numeromodelo);
				$stmt->bindParam(':competencia', $competencia);
				$stmt->bindParam(':comportamiento', $comportamiento);
	
				for($i = 0; $i < $cant; $i++)
				{
					$competencia = $this->modelo[$i]['idcompe'];
					$comportamiento = $this->modelo[$i]['idcompor'];
					$stmt->execute();
				}
      }

      //obtengo la cantidad de evaluaciones creadas
      $cantEvaluaciones = count($idevaluaciones);
      $evaluador = null;
      //inserto los evaluadores para cada evaluacion
      for($j=0; $j<$cantEvaluaciones; $j++)
      {
        $id = $idevaluaciones[$j];
        $cant = count($this->evaluadores);	// obtengo la cantidad de areas destino
        $stmt = $this->db->prepare("INSERT INTO evaluadores (evaluacion, evaluador) VALUES (:evaluacion, :evaluador)");
        $stmt->bindParam(':evaluacion', $id);
        $stmt->bindParam(':evaluador', $evaluador);

        for($i = 0; $i < $cant; $i++)
        {
          $evaluador = $this->evaluadores[$i]['id'];
          $stmt->execute();
        }
      }
      // commit the transaction
      $this->db->commit();

      http_response_code(OK);
      return 'ok';
      // TODO: verificar fallo del commit
      /* 
      else {
        http_response_code(NO_CONTENT);  // No Content (la petición se ha completado con éxito, pero su respuesta no tiene ningun contenido)
        return "consulta vacia";
      } */
    } catch (\PDOException $e) {
      $this->db->rollback();
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }    
  }
}

?>