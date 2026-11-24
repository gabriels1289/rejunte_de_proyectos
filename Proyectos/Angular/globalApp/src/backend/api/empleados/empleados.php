<?php
$ARCHIVO = 'empleados/empleados.php';
// clase para manejar las Sql relacionadas con empleados
// TODO: colocar un esquema de logs y cerrar la conexion en algun lugar fuera de la clase
class Empleados
{
  private $db = null; // propiedad para la conexion a la DB
  private $requestMethod = null; // propiedad para el metodo de peticion http (GET, POST, ...)

  private $data = null;  // propiedad para obtener los datos de entrada

  private $idusuario = null;
  private $idempleado = null;
  private $idacceso = null;
  private $listahijos = null;
  private $legajo = null;
  private $apellido = null;
  private $nombre = null;
  private $direccion = null;
  private $localidad = null;
  private $codigopostal = null;
  private $telefonofijo = null;
  private $telefonomovil = null;
  private $mail = null;
  private $fechanacimiento = null;
  private $fechacontrato = null;
  private $tipodocumento = null;
  private $numerodocumento = null;
  private $cuil = null;
  private $legajoestudio = null;
  private $preocupacional = null;
  private $preocupacionaldetalle = null;
  private $fechaanalisis = null;
  private $basico = null;
  private $sexo = null;
  private $fechabaja = null;
  private $activo = null;
  private $fulltime = null;
  private $foto = null;
  private $art = null;
  private $obrasocial = null;
  private $sector = null;
  private $empresa = null;
  private $actualizarFoto = null;
  private $fechaincorporacion = null;
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
    $this->idempleado = (isset($this->data['idempleado'])) ? filter_var($this->data['idempleado'], FILTER_VALIDATE_INT) : null;
    $this->idacceso = (isset($this->data['idacceso'])) ? filter_var($this->data['idacceso'], FILTER_VALIDATE_INT) : null;
    $this->listahijos = (isset($this->data['listahijos'])) ? $this->data['listahijos'] : null;
    $this->numerodocumento = (isset($this->data['numerodocumento'])) ? $this->data['numerodocumento'] : null;
    $this->legajo = (isset($this->data['legajo'])) ? $this->data['legajo'] : null;
    $this->apellido = (isset($this->data['apellido'])) ? $this->data['apellido'] : null;
    $this->nombre = (isset($this->data['nombre'])) ? $this->data['nombre'] : null;
    $this->direccion = (isset($this->data['direccion'])) ? $this->data['direccion'] : null;
    $this->localidad = (isset($this->data['localidad'])) ? $this->data['localidad'] : null;
    $this->codigopostal = (isset($this->data['codigopostal'])) ? $this->data['codigopostal'] : null;
    $this->telefonofijo = (isset($this->data['telefonofijo'])) ? $this->data['telefonofijo'] : null;
    $this->telefonomovil = (isset($this->data['telefonomovil'])) ? $this->data['telefonomovil'] : null;
    $this->mail = (isset($this->data['mail'])) ? $this->data['mail'] : null;
    $this->fechanacimiento = (isset($this->data['fechanacimiento'])) ? $this->data['fechanacimiento'] : null;
    $this->fechacontrato = (isset($this->data['fechacontrato'])) ? $this->data['fechacontrato'] : null;
    $this->fechaincorporacion = (isset($this->data['fechaincorporacion'])) ? $this->data['fechaincorporacion'] : null;
    $this->tipodocumento = (isset($this->data['tipodocumento'])) ? $this->data['tipodocumento'] : null;
    $this->cuil = (isset($this->data['cuil'])) ? $this->data['cuil'] : null;
    $this->legajoestudio = (isset($this->data['legajoestudio'])) ? $this->data['legajoestudio'] : null;
    $this->preocupacional = (isset($this->data['preocupacional'])) ? $this->data['preocupacional'] : null;
    $this->preocupacionaldetalle = (isset($this->data['preocupacionaldetalle'])) ? $this->data['preocupacionaldetalle'] : null;
    $this->fechaanalisis = (isset($this->data['fechaanalisis'])) ? $this->data['fechaanalisis'] : null;
    $this->basico = (isset($this->data['basico'])) ? $this->data['basico'] : null;
    $this->sexo = (isset($this->data['sexo'])) ? $this->data['sexo'] : null;
    $this->fechabaja = (isset($this->data['fechabaja'])) ? $this->data['fechabaja'] : null;
    $this->activo = (isset($this->data['activo'])) ? $this->data['activo'] : null;
    $this->fulltime = (isset($this->data['fulltime'])) ? $this->data['fulltime'] : null;
    $this->foto = (isset($this->data['foto'])) ? $this->data['foto'] : null;
    $this->art = (isset($this->data['art'])) ? $this->data['art'] : null;
    $this->obrasocial = (isset($this->data['obrasocial'])) ? $this->data['obrasocial'] : null;
    $this->sector = (isset($this->data['sector'])) ? $this->data['sector'] : null;
    $this->empresa = (isset($this->data['empresa'])) ? $this->data['empresa'] : null;
    $this->actualizarFoto = (isset($this->data['actualizarFoto'])) ? filter_var($this->data['actualizarFoto'], FILTER_VALIDATE_BOOLEAN) : null;

    $this->logger = new Logger();
  }

  /**
   * las funciones que comienzan con GET son propias del método GET de HTTP
   * las funciones que comienzan con SET son propias del método PUT de HTTP
   * las funciones que comienzan con PUSH son propias del método POST de HTTP
   * las funciones que comienzan con DELETE son propias del método DELETE de HTTP
   */

  /**
   * Devuleve el listado de empleados
   * 
   * @param $idacceso
   * @param $listahijos
   * @return array con los empleados.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getListadoDeEmpleados() // $idacceso, $listahijos
  {
    if($this->idacceso == 1 || $this->idacceso == 3 || $this->idacceso == 10){
          $sql = "SELECT e.id, e.legajo, e.apellido, e.nombre, e.activo, e.sector AS idsector,
                IF(e.foto IS NULL OR e.foto = '', 'img/user.jpg', e.foto) AS foto, s.area AS sector
              FROM empleados e INNER JOIN sectores s ON s.id=e.sector
              WHERE e.empresa=1
              ORDER BY e.apellido ASC";
    }
    else {
      $sql = "SELECT e.id, e.legajo, e.apellido, e.nombre, e.activo, e.sector AS idsector, 
            IF(e.foto IS NULL OR e.foto = '', 'img/user.jpg', e.foto) AS foto, s.area AS sector
          FROM empleados e  INNER JOIN sectores s ON s.id=e.sector
          WHERE e.acceso IN ($this->listahijos) AND e.empresa=1 AND e.activo='SI'
          ORDER BY e.apellido ASC";
    }

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
   * Devuleve el detalle de un empleado
   * 
   * @param $idempleado
   * @return un objeto con toda la informacion del empleado.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getDetalleEmpleado() // $idempleado
  {
    $sql = "SELECT e.id, e.legajo, e.nombre, e.apellido, e.direccion, e.localidad, e.cpostal AS codigopostal, e.telefonofijo, e.telefonomovil, e.mail, e.fechanacimiento, e.fechaincorporacion,
              e.fechacontrato, e.tipodocumento, e.numerodocumento, e.cuil, e.legajoestudio, e.preocupacional, e.preocupacionaldetalle, e.fechaanalisis,
                e.basico, e.sexo, e.fechabaja, e.activo, e.fulltime, IF(e.foto IS NULL OR e.foto = '', 'img/user.jpg', e.foto) AS foto, art.id AS idart, art.nombre AS art, o.id AS idobrasocial, o.nombre AS obrasocial,
                em.id AS idempresa, em.razonsocial, s.id AS idsector, s.area AS sector, e.acceso AS idacceso, a.acceso
            FROM empleados e, empresas em, art, osocial o, sectores s, accesos a
            WHERE e.id='$this->idempleado' AND em.id=e.empresa AND art.id=e.art AND o.id=e.osocial AND s.id=e.sector AND e.acceso=a.id";

    try {
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
   * Devuleve el detalle de un empleado
   * 
   * @param $idempleado
   * @return un objeto con toda la informacion del empleado.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getResumenEmpleado() // $idempleado
  {
    $sql = "SELECT e.id, e.legajo, e.nombre, e.apellido, e.activo, IF(e.foto IS NULL OR e.foto = '', 'img/user.jpg', e.foto) AS foto,
                s.id AS idsector, s.area AS sector, e.acceso AS idacceso, a.acceso
            FROM empleados e, sectores s, accesos a
            WHERE e.id='$this->idempleado' AND s.id=e.sector AND e.acceso=a.id";

    try {
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
   * Devuleve los datos para los selects del formulario
   * 
   * @param $idacceso
   * @return array con los arreglos de datos para los selects del formulario.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getDatosSelectsFormulario() // $idacceso
  {
    try {
      $datos = array();
      $sql = "SELECT * FROM empresas WHERE activo='SI'";
      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
        array_push($datos, $consulta);
        if ($this->idacceso == 1) {
          $sql = "SELECT * FROM sectores WHERE activo='A'";
        } else {
          $sql = "SELECT * FROM sectores WHERE activo='A' AND id!=1";
        }
        $consulta = $this->db->query($sql);
        $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
        if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
          array_push($datos, $consulta);
          $sql = "SELECT * FROM osocial";
          $consulta = $this->db->query($sql);
          $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
          if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
            array_push($datos, $consulta);
            $sql = "SELECT * FROM art";
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
   * Resetea la contraseña de un usuario
   * 
   * 
   * @param $idmensaje
   * @param $idusuario
   * @param $numeroLecturas
   * @param $tabla
   * @return string ok.
   * @return OK consulta exitosa.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function setPasswordPorDefecto()  // $idmensaje, $idusuario, $numeroLecturas, $tabla
  {
    try {
      $newpass = sha1($this->numerodocumento);
			$username = $this->numerodocumento;
      
      $sql = "UPDATE empleados SET username=:username, password=:password WHERE id=:id";
      $stmt = $this->db->prepare($sql);
      $stmt->bindParam(':id', $this->idempleado);
      $stmt->bindParam(':username', $username);
      $stmt->bindParam(':password', $newpass);
      if ($stmt->execute())
      {
        $this->logger->info("reseteo el password de: " . $this->idempleado, $this->ARCHIVO, __LINE__);
        http_response_code(OK);
        return "ok";
      }else {
        $this->logger->fatal("hubo un fallo en la consulta", $this->ARCHIVO, __LINE__);
        http_response_code(INTERNAL_SERVER_ERROR);
        return "fallo consulta";
      }      
    }
    catch(PDOException $e)
    {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }
  }

  /**
   * Inserta o edita un empleado
   */
  public function abmEmpleado()
  {
    try {
      // recupera el acceso para un sector determinado
			$q = "SELECT s.id as idsector, s.area, a.id as idacceso, a.acceso
      FROM sectores s INNER JOIN empleados e ON e.sector=s.id INNER JOIN accesos a ON e.acceso=a.id
      WHERE s.id='$this->sector' AND e.activo='SI'
      GROUP BY s.area ORDER BY a.id";
      $consulta = $this->db->query($q);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      
      if(count($consulta)>=1 && ($consulta[0]["idsector"]!=null))
      {
        $acceso = $consulta[0]["idacceso"];
      } else {
        $this->logger->fatal("hubo un fallo en la consulta", $this->ARCHIVO, __LINE__);
        http_response_code(INTERNAL_SERVER_ERROR);
        return "fallo obtencion de acceso";
      }

      if($this->idempleado > 0) // es una edición
			{
				// corroboro que no exista otro empleado con el dni o legajo
				$check = $this->db->query("SELECT * FROM empleados WHERE id!='$this->idempleado' AND (legajo='$this->legajo' OR numerodocumento='$this->numerodocumento') AND activo='SI' ");
				$check = $check->fetchAll();
				
				if (count($check) >= 1) {// && ($chek[0]["id"]!=null)){
          //echo "existe un empleado con alguno de esos datos";
          http_response_code(CONFLICT);
					return "existe un empleado con el legajo o documento en uso";
				} else {
          // no existe la posibilidad de duplicar los legajo o dni
          # region foto
          if ($this->actualizarFoto == true) {
            $directorio = "../assets/img/perfiles/";
            $directorioDB = "img/perfiles/";
            chdir("../");   // subo un nivel de directorio
            if(!is_dir($directorio)) {
              mkdir($directorio, 0777);
            }
        
            if($this->foto!=null || $this->foto!='') {	// ya existe un nombre de foto, utilizo ese
              unlink('../assets/'.$this->foto);  // elimino la imagen vieja
            }
        
            //genero un nombre nuevo
            $temp = explode(".", $_FILES["foto"]["name"]);
            $this->foto = $directorioDB.  $this->numerodocumento . "_" . uniqid() . '.' . end($temp);
            move_uploaded_file($_FILES["foto"]["tmp_name"], '../assets/'.$this->foto);

          }
          #endregion foto

					$q = "UPDATE empleados SET
					sexo=:sexo, empresa=:empresa, sector=:sector, osocial=:osocial, art=:art, fechanacimiento=:fechanacimiento, fechaincorporacion=:fechaincorporacion,
					fechacontrato=:fechacontrato, fechabaja=:fechabaja, fechaanalisis=:fechaanalisis, nombre=:nombre, apellido=:apellido, tipodocumento=:tipodocumento, numerodocumento=:numerodocumento,
					localidad=:localidad, cpostal=:cpostal, direccion=:direccion, telefonomovil=:telefonomovil, telefonofijo=:telefonofijo,
					activo=:activo, legajo=:legajo, cuil=:cuil, basico=:basico, fulltime=:fulltime, preocupacional=:preocupacional, preocupacionaldetalle=:preocupacionaldetalle, mail=:mail,
					legajoestudio=:legajoestudio, acceso=:acceso, foto=:foto
					WHERE id=:id";

					$query = $this->db->prepare($q);
					$execute = $query->execute(array(
								":id" => $this->idempleado,
								":sexo" => $this->sexo,
								":empresa" => $this->empresa,
								":sector" => $this->sector,
								":osocial" => $this->obrasocial,
								":art" => $this->art,
								":fechanacimiento" => $this->fechanacimiento,
								":fechaincorporacion" => $this->fechaincorporacion,
								":fechacontrato" => $this->fechacontrato,
								":fechabaja" => $this->fechabaja,
								":fechaanalisis" => $this->fechaanalisis,
								":nombre" => $this->nombre,
								":apellido" => $this->apellido,
								":tipodocumento" => $this->tipodocumento,
								":numerodocumento" => $this->numerodocumento,
								":localidad" => $this->localidad,
								":cpostal" => $this->codigopostal,
								":direccion" => $this->direccion,
								":telefonomovil" => $this->telefonomovil,
								":telefonofijo" => $this->telefonofijo,
								":activo" => $this->activo,
								":legajo" => $this->legajo,
								":cuil" => $this->cuil,
								":basico" => $this->basico,
								":fulltime" => $this->fulltime,
								":preocupacional" => $this->preocupacional,
								":preocupacionaldetalle" => $this->preocupacionaldetalle,
								":legajoestudio" => $this->legajoestudio,
								":mail" => $this->mail,
                ":acceso" => $acceso,
                ":foto" => $this->foto
							));

					if($execute > 0)
					{
            $this->logger->info("edito al empleado: " . $this->idempleado, $this->ARCHIVO, __LINE__);
            http_response_code(OK);
						return $this->idempleado;
					}
					else {
            $this->logger->fatal("hubo un fallo en la consulta", $this->ARCHIVO, __LINE__);
            http_response_code(INTERNAL_SERVER_ERROR);
						return "fallo consulta";
					}
				}	
			}
			else
			{
				// corroboro que no exista otro empleado con el dni o legajo
				$check = $this->db->query("SELECT * FROM empleados WHERE (legajo='$this->legajo' OR numerodocumento='$this->numerodocumento') AND activo='SI' ");
				$check = $check->fetchAll();
				if (count($check) >= 1) {// && ($chek[0]["id"]!=null)){
          http_response_code(CONFLICT);
					return "existe un empleado con el legajo o documento en uso";
				} else {
          //echo "no existe un empleado con alguno de esos datos" efectua la operacion de carga
          # region foto
          if ($this->actualizarFoto == true) {
            $directorio = "../assets/img/perfiles/";
            $directorioDB = "img/perfiles/";
            chdir("../");   // subo un nivel de directorio
            if(!is_dir($directorio)) {
              mkdir($directorio, 0777);
            }
        
            if($this->foto!=null) {	// ya existe un nombre de foto, utilizo ese
              unlink('../assets/'.$this->foto);  // elimino la imagen vieja
            }
        
            //genero un nombre nuevo
            $temp = explode(".", $_FILES["foto"]["name"]);
            $this->foto = $directorioDB.  $this->numerodocumento . "_" . uniqid() . '.' . end($temp);
            move_uploaded_file($_FILES["foto"]["tmp_name"], '../assets/'.$this->foto);
          }
          #endregion foto
          $this->fechaincorporacion = "0000-00-00";

					$q = "INSERT INTO empleados (sexo, empresa, sector, osocial, art, fechanacimiento, fechaincorporacion, fechacontrato, fechabaja, fechaanalisis, nombre, apellido, tipodocumento, numerodocumento,
					localidad, cpostal, direccion, telefonomovil, telefonofijo,	activo, legajo, cuil, basico, fulltime, preocupacional, preocupacionaldetalle, legajoestudio, mail, acceso, username, password, foto)
					VALUES (:sexo, :empresa, :sector, :osocial, :art, :fechanacimiento, :fechaincorporacion, :fechacontrato, :fechabaja, :fechaanalisis, :nombre, :apellido, :tipodocumento, :numerodocumento,
					:localidad, :cpostal, :direccion, :telefonomovil, :telefonofijo,	:activo, :legajo, :cuil, :basico, :fulltime, :preocupacional, :preocupacionaldetalle, :legajoestudio, :mail, :acceso, :username, :password, :foto)";

					$query = $this->db->prepare($q);
					$execute = $query->execute(array(
								":sexo" => $this->sexo,
								":empresa" => $this->empresa,
								":sector" => $this->sector,
								":osocial" => $this->obrasocial,
								":art" => $this->art,
								":fechanacimiento" => $this->fechanacimiento,
								":fechaincorporacion" => $this->fechaincorporacion,
								":fechacontrato" => $this->fechacontrato,
								":fechabaja" => $this->fechabaja,
								":fechaanalisis" => $this->fechaanalisis,
								":nombre" => $this->nombre,
								":apellido" => $this->apellido,
								":tipodocumento" => $this->tipodocumento,
								":numerodocumento" => $this->numerodocumento,
								":localidad" => $this->localidad,
								":cpostal" => $this->codigopostal,
								":direccion" => $this->direccion,
								":telefonomovil" => $this->telefonomovil,
								":telefonofijo" => $this->telefonofijo,
								":activo" => $this->activo,
								":legajo" => $this->legajo,
								":cuil" => $this->cuil,
								":basico" => $this->basico,
								":fulltime" => $this->fulltime,
								":preocupacional" => $this->preocupacional,
								":preocupacionaldetalle" => $this->preocupacionaldetalle,
								":legajoestudio" => $this->legajoestudio,
								":mail" => $this->mail,
								":acceso" => $acceso,
								":username" => $this->numerodocumento,
                ":password" => sha1($this->numerodocumento),
                ":foto" => $this->foto
							));

					if($execute > 0)
					{
						$lastid = $this->db->lastInsertId();
						http_response_code(OK);
						return $lastid;
					}
					else {
            $this->logger->fatal("hubo un fallo en la consulta", $this->ARCHIVO, __LINE__);
						http_response_code(INTERNAL_SERVER_ERROR);
						return "fallo consulta";
					}
				}
			}
    }
    catch(PDOException $e)
    {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }
  }

}

?>