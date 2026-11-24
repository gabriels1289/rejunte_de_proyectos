<?php
$ARCHIVO = 'recursos/recursosManager.php';
class RecursosManager
{
  private $db;  // propiedad para la conexion a la base de datos
  private $subruta; // propiedad para la subruta
  private $requestMethod; // propiedad para el metodo http
  private $querysurl; // propiedad opcional para algun query_stryng
  private $recursos;  // propiedad para la clase Recursos
  
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
    $this->recursos = new Recursos($this->db, $this->requestMethod);
  }

  public function procesarRecursos()
  {
    switch ($this->subruta[2]) {
      case 'biblioteca':
        echo json_encode($this->procesarRequestBiblioteca(), JSON_NUMERIC_CHECK);
        break;

      case 'gestor':
        // $temp = $this->procesarRequestGestor();
        // ChromePhp::log("PHP 2: %o", $temp);
        // echo json_encode($temp, JSON_NUMERIC_CHECK);
        // $temp2 = json_encode($temp, JSON_NUMERIC_CHECK); 
        // ChromePhp::log("PHP 3: %o", $temp2);
        // echo $temp2;        
        echo json_encode($this->procesarRequestGestor(), JSON_NUMERIC_CHECK);
        break;

      default:
        http_response_code(NOT_FOUND);
        break;
    }
  }


  private function procesarRequestBiblioteca()
  {
    switch ($this->requestMethod) {
      case 'GET':
        return $this->recursos->getBiblioteca();
        break;
      
      case 'PUT':
        # code...
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

  private function procesarRequestGestor()
  {
    switch ($this->requestMethod) {
      case 'GET':
        $subruta3 = (isset($this->subruta[3])) ? $this->subruta[3] : false ;  // $this->subruta[3] == 'formulario'
        if ($subruta3 == false) {  // si no existe una subruta
          return $this->recursos->getRecursos();
        } else if ($subruta3 == 'resumen') {
          // pedido para el resumen de una evaluacion
          #code...
        }
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