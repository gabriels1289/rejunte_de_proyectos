<?php
$ARCHIVO = 'recursos/recursos.php';
// clase para manejar las Sql relacionadas con evaluaciones
// TODO: colocar un esquema de logs y cerrar la conexion en algun lugar fuera de la clase
class Recursos
{
  private $db = null; // propiedad para la conexion a la DB
  private $requestMethod = null; // propiedad para el metodo de peticion http (GET, POST, ...)

  private $data = null;  // propiedad para obtener los datos de entrada

  private $idusuario = null;
  private $idsector = null;
  private $idrecurso = null;
  private $activo = null;
  private $direccion = null;
  private $tipo = null;
  private $comentario = null;
  private $titulo = null;
  private $destinos = null;
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
    $this->idsector = (isset($this->data['idsector'])) ? filter_var($this->data['idsector'], FILTER_VALIDATE_INT) : null;
    $this->idrecurso = (isset($this->data['idrecurso'])) ? filter_var($this->data['idrecurso'], FILTER_VALIDATE_INT) : null;
    $this->activo = (isset($this->data['activo'])) ? $this->data['activo'] : null;
    $this->direccion = (isset($this->data['direccion'])) ? $this->data['direccion'] : null;
    $this->tipo = (isset($this->data['tipo'])) ? $this->data['tipo'] : null;
    $this->comentario = (isset($this->data['comentario'])) ? $this->data['comentario'] : null;
    $this->titulo = (isset($this->data['titulo'])) ? $this->data['titulo'] : null;
    $this->destinos = (isset($this->data['destinos'])) ? $this->data['destinos'] : null;

    $this->logger = new Logger();
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
  public function getBiblioteca() // $idevaluacion
  {
    try {
      $datos = array();

      // videos
      if ($this->idsector != 1) {
        $sql = "SELECT r.id, r.titulo, r.direccion, r.tipo, r.comentario
                FROM recursos r, areas_recursos ar
                WHERE activo='A' AND tipo='V' AND r.id = ar.idrecurso AND ar.idarea=$this->idsector
                ORDER BY r.id ASC";
      } else {
        $sql = "SELECT r.id, r.titulo, r.direccion, r.tipo, r.comentario
                FROM recursos r
                WHERE activo='A' AND tipo='V'
                ORDER BY r.id ASC";
      }
      

      $videos = $this->db->query($sql);
      $videos = $videos->fetchAll(PDO::FETCH_ASSOC);

      // documentos
      if ($this->idsector !=1) {
        $sql = "SELECT r.id, r.titulo, r.direccion, r.tipo, r.comentario
                FROM recursos r, areas_recursos ar
                WHERE activo='A' AND tipo='D' AND r.id = ar.idrecurso AND ar.idarea=$this->idsector
                ORDER BY r.id ASC";
      } else {
        $sql = "SELECT r.id, r.titulo, r.direccion, r.tipo, r.comentario
                FROM recursos r
                WHERE activo='A' AND tipo='D'
                ORDER BY r.id ASC";
      }
      

      $documentos = $this->db->query($sql);
      $documentos = $documentos->fetchAll(PDO::FETCH_ASSOC);

      if (count($videos) >= 1 && ($videos[0]["id"]!=null)) {
        array_push($datos, $videos);
      }
      else {
        array_push($datos, 'vacio');
      }

      if (count($documentos) >= 1 && ($documentos[0]["id"]!=null)) {
        array_push($datos, $documentos);
      }
      else {
        array_push($datos, 'vacio');
      }

      http_response_code(OK);
      return $datos;
    } catch (\PDOException $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }    
  }

  /**
   * Devuleve los mensajes de la bandeja de entrada
   * 
   * @param $idusuario
   * @return array con los mensajes.
   * @return OK consulta exitosa.
   * @return NO_CONTENT consulta vacia.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function getRecursos() //
  {
    try {
      $sql = "SELECT r.id, r.titulo, r.comentario, r.direccion, r.tipo, r.activo, s.area
              FROM areas_recursos ar, sectores s, recursos r
              WHERE ar.idarea=s.id AND ar.idrecurso=r.id AND s.activo='A'
              ORDER BY r.id ASC";

      $consulta = $this->db->query($sql);
      $consulta = $consulta->fetchAll(PDO::FETCH_ASSOC);
      if (count($consulta) >= 1 && ($consulta[0]["id"]!=null)) {
        // ChromePHP::log("en PHP: %o", $consulta);
        $subConjuntoPorId = [];
        $valorId = 0;
        $valorIdAnterior = 0;
        $posicionInicial = 0;
        $stringDeAreas = '';
        $recursos = [];
        $cantidad = count($consulta);
        
        for($i=0; $i<$cantidad; $i++)
        {
          $valorId = $consulta[$i]['id'];
          // verifico si tengo que actualizar la posicion inicial de busqueda y el valor del id
          if($valorId != $valorIdAnterior)
          {
            $valorIdAnterior = $valorId;
            $posicionInicial = $i;
            $stringDeAreas = '';
          
            for($j=$posicionInicial; $j<$cantidad; $j++)
            {
              if($consulta[$j]['id'] == $valorId)
              {
                $stringDeAreas .= $consulta[$j]['area'] . ', ';
              }
            }
            // elimino espacios en blanco al principio y al final
            $stringDeAreas = trim($stringDeAreas);
            // elimino la ultima coma: trailing comma
            $stringDeAreas = trim($stringDeAreas, ',');

            // agrego a un arreglo el recurso
            $recursos[] = array('id' => $valorId,
                                'titulo' => $consulta[$i]['titulo'],
                                'comentario' => $consulta[$i]['comentario'],
                                'direccion' => $consulta[$i]['direccion'],
                                'areas' => $stringDeAreas,
                                'tipo' => $consulta[$i]['tipo'],
                                'activo' => $consulta[$i]['activo']);
          }
        }
        // elimino duplicados
        // $recursos = array_unique($recursos);

        http_response_code(OK);
        return $recursos;
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
   * Dar de alta o baja un recurso
   * 
   * @param $idrecuros
   * @param $activo
   * @return OK consulta exitosa.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function deleteRecurso() //$idrecurso, $activo
  {
    try {
      $this->db->beginTransaction();

      // borro el recurso
      $sql = "DELETE FROM recursos WHERE id=:id";
      $stmt = $this->db->prepare($sql);
      $stmt->bindParam(':id', $this->idrecurso);
      $stmt->execute();

      // borro las areas a donde se destino el recurso
      $sql = "DELETE FROM areas_recursos WHERE idrecurso=:id";
      $stmt = $this->db->prepare($sql);
      $stmt->bindParam(':id', $this->idrecurso);
      $stmt->execute();

      $this->db->commit();

      // borro en caso de ser un documento
      if($this->tipo == 'D')
			{
				chdir("../"); // subo un nivel en el directorio
				unlink('../assets/'.$this->direccion);  // elimino el archivo
			}
      
      $this->logger->info("borro recurso: " . $this->idrecurso, $this->ARCHIVO, __LINE__);
      http_response_code(OK);
      return "ok";
    } catch (\PDOException $e) {
      $this->db->rollback();
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }    
  }

  /**
   * Dar de alta o baja un recurso
   * 
   * @param $idrecuros
   * @param $activo
   * @return OK consulta exitosa.
   * @return INTERNAL_SERVER_ERROR hubo un error en la ejecucion, devuelve el mensaje de la excepcion.
   */
  public function setAltaBaja() //$idrecurso, $activo
  {
    try {
      if($this->activo == 'A')
      {
        $this->activo = 'N';
      }
      else{
        $this->activo = 'A';
      }
      $sql = "UPDATE recursos SET activo=:activo WHERE id=:id";
      $stmt = $this->db->prepare($sql);
      $stmt->bindParam(':id', $this->idrecurso);
      $stmt->bindParam(':activo', $this->activo);
      if ($stmt->execute())
      {
        $this->logger->info("cambio alta/baja: (" . $this->activo . ") del recurso: " . $this->idrecurso, $this->ARCHIVO, __LINE__);
        http_response_code(OK);
        return "ok";
      }else {
        $this->logger->fatal("hubo un fallo al dar de alta/baja: " . $this->idrecurso, $this->ARCHIVO, __LINE__);
        http_response_code(INTERNAL_SERVER_ERROR);
        return "fallo consulta";
      } 
    } catch (\PDOException $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      return "error: ".$e->getMessage();
    }    
  }

  public function pushRecurso()
  {
    try {
      if($this->idrecurso > 0) // es una edición
			{
        // begin the transaction
        $this->db->beginTransaction();
        // Elimino las areas que tenia asigandas el recurso
        $stmt = $this->db->prepare("DELETE FROM areas_recursos WHERE idrecurso=:idrecurso");
        $stmt->bindParam(':idrecurso', $this->idrecurso);
        $stmt->execute();

        // Cargo las nuevas areas
        $aux = null;
        $q = "INSERT INTO areas_recursos (idrecurso, idarea) VALUES (:id, :idarea)";
        $stmt = $this->db->prepare($q);
        $stmt->bindParam(':idarea', $aux);
        $stmt->bindParam(':id', $this->idrecurso);
        
        $cant = count($this->destinos);	// obtengo la cantidad de areas destino
        for($i = 0; $i < $cant; $i++)
        {
          $aux = $this->destinos[$i]['id'];
          $stmt->execute();
        }

        // Actualizo los cambios en el recurso
        $stmt = $this->db->prepare("UPDATE recursos SET comentario=:comentario, titulo=:titulo WHERE id=:id");
        $stmt->bindParam(':comentario', $this->comentario);
        $stmt->bindParam(':titulo', $this->titulo);
        $stmt->bindParam(':id', $this->idrecurso);
        $stmt->execute();

        // commit the transaction
        $this->db->commit();
        http_response_code(OK);
        return $this->idrecurso;
       /*  if($execute > 0)
        {
          http_response_code(OK);
          return $this->idempleado;
        }
        else {
          http_response_code(INTERNAL_SERVER_ERROR);
          return "fallo consulta";
        } */
			}
			else
			{
        # region archivo
        if ($this->tipo == 'D') {
          $directorio = "../assets/pdf/";
          $directorioDB = "pdf/";
          chdir("../");   // subo un nivel de directorio
          if(!is_dir($directorio)) {
            mkdir($directorio, 0777);
          }
      
          /* if($this->archivo!=null) {	// ya existe un nombre de archivo, utilizo ese
            unlink('../assets/'.$this->archivo);  // elimino el documento viejo
          } */
      
          //genero un nombre nuevo
          $temp = explode(".", $_FILES["archivo"]["name"]);
          $this->direccion = $directorioDB.  uniqid() . "_" . uniqid() . '.' . end($temp);
          move_uploaded_file($_FILES["archivo"]["tmp_name"], '../assets/'.$this->direccion);
        }
        #endregion archivo

        // begin the transaction
        $this->db->beginTransaction();

				// Cargo el recurso
        $stmt = $this->db->prepare("INSERT INTO recursos (titulo, direccion, tipo, comentario) VALUES (:titulo, :direccion, :tipo, :comentario)");
        $stmt->bindParam(':titulo', $this->titulo);
        $stmt->bindParam(':direccion', $this->direccion);
        $stmt->bindParam(':tipo', $this->tipo);
        $stmt->bindParam(':comentario', $this->comentario);
        $stmt->execute();

        $id = $this->db->lastInsertId();

        // Cargo las areas
        $aux = null;
        $q = "INSERT INTO areas_recursos (idrecurso, idarea) VALUES (:id, :idarea)";
        $stmt = $this->db->prepare($q);
        $stmt->bindParam(':idarea', $aux);
        $stmt->bindParam(':id', $id);
        
        $cant = count($this->destinos);	// obtengo la cantidad de areas destino
        for($i = 0; $i < $cant; $i++)
        {
          $aux = $this->destinos[$i]['id'];
          $stmt->execute();
        }

        // commit the transaction
        $this->db->commit();
        http_response_code(OK);
        return $id;

        /*
          if($execute > 0)
          {
            $lastid = $this->db->lastInsertId();
            http_response_code(OK);
            return $lastid;
          }
          else {
            http_response_code(INTERNAL_SERVER_ERROR);
            return "fallo consulta";
          }
        */
			}
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