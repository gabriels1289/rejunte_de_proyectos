<?php
$ARCHIVO = 'ajustes/ajustesManager.php';
class AjustesManager
{
  private $db;  // propiedad para la conexion a la base de datos
  private $subruta; // propiedad para la subruta
  private $requestMethod; // propiedad para el metodo http
  private $querysurl; // propiedad opcional para algun query_stryng
  private $ajustes;  // propiedad para la clase Recursos
  
  /**
   * aca inicializo la clase empleadoManager y le indico:
   * la subruta(empleado-detalle, listado, etc..),
   * el tipo de peticion($_GET, $_POST)
   * así en el constructor de la clase empleado se como inicializar los parametros de las sql
   * y algun parametro QUERY_STRING pasado en la url
   */

  public function __construct($db, $subruta, $requestMethod, $querysurl=null)
  {
    $this->db = $db;
    $this->subruta = $subruta;
    $this->requestMethod = $requestMethod;
    $this->querysurl = $querysurl;
    //$this->querysurl = (isset($querysurl)) ? $querysurl : null ;
    $this->ajustes = new Ajustes($this->db, $this->requestMethod);
  }

  public function procesarAjustes()
  {
    switch ($this->subruta[2]) {
      case 'perfil':
        echo json_encode($this->procesarRequestPerfil(), JSON_NUMERIC_CHECK);
        break;

      case 'logs':
        echo json_encode($this->procesarRequestLogs(), JSON_NUMERIC_CHECK);
        break;

      default:
        http_response_code(NOT_FOUND);
        break;
    }
  }


  private function procesarRequestPerfil()
  {
    switch ($this->requestMethod) {
      case 'GET':
        # code...
        break;
      
      case 'PUT':
        return $this->ajustes->setPassword();
        break;

      case 'POST':
        # code...
        break;

      case 'DELETE':
        # code...
        break;

      case 'OPTIONS':
        http_response_code(OK);
        break;

      default:
        http_response_code(NOT_FOUND);
        break;
    }
  }

  private function procesarRequestLogs()
  {
    switch ($this->requestMethod) {
      case 'GET':
        return $this->ajustes->getLogs();
        break;
      
      case 'PUT':
        return $this->recursos->setAltaBaja();
        break;

      case 'POST':
        return $this->recursos->pushRecurso();
        break;

      case 'DELETE':
        return $this->recursos->deleteRecurso();
        break;

      case 'OPTIONS':
        http_response_code(OK);
        break;

      default:
        http_response_code(NOT_FOUND);
        break;
    }
  } 

}
?>