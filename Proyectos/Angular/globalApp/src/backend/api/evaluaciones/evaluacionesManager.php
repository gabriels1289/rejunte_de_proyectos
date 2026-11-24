<?php
$ARCHIVO = 'evaluaciones/evaluacionesManager.php';
class EvaluacionesManager
{
  private $db;  // propiedad para la conexion a la base de datos
  private $subruta; // propiedad para la subruta
  private $requestMethod; // propiedad para el metodo http
  private $querysurl; // propiedad opcional para algun query_stryng
  private $evaluaciones;  // propiedad para la clase Evaluaciones
  
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
    $this->evaluaciones = new Evaluaciones($this->db, $this->requestMethod);
  }

  public function procesarEvaluaciones()
  {
    switch ($this->subruta[2]) {
      case 'listado':
        echo json_encode($this->procesarRequestListado(), JSON_NUMERIC_CHECK);
        break;
  
      case 'evaluar':
        echo json_encode($this->procesarRequestEvaluar(), JSON_NUMERIC_CHECK);
        break;

      case 'comparacion':
        echo json_encode($this->procesarRequestComparacion(), JSON_NUMERIC_CHECK);
        break;

      case 'crear-evaluacion':
        echo json_encode($this->procesarRequestCrearEvaluacion(), JSON_NUMERIC_CHECK);
        break;

      default:
        http_response_code(NOT_FOUND);
        break;
    }
  }


  private function procesarRequestListado()
  {
    switch ($this->requestMethod) {
      case 'GET':
        $subruta3 = (isset($this->subruta[3])) ? $this->subruta[3] : false ;  // $this->subruta[3] == 'formulario'
        if ($subruta3 == false) {  // si no existe una subruta
          return $this->evaluaciones->getEvaluaciones();
        } else if ($subruta3 == 'resumen') {
          // pedido para el resumen de una evaluacion
          return $this->evaluaciones->getResumenEvaluacion();
        }
        break;
      
      case 'PUT':
        $subruta3 = (isset($this->subruta[3])) ? $this->subruta[3] : false ;  // $this->subruta[3] == 'formulario'
        if ($subruta3 == false) {  // si no existe una subruta
          return $this->evaluaciones->setArchivar();
        } else if ($subruta3 == 'abrir-cerrar') {
          // pedido para abrir o cerrar una evaluacion
          return $this->evaluaciones->setAbrirCerrar();
        }
        break;

      case 'POST':
        # code...
        break;

      case 'DELETE':
        return $this->evaluaciones->deleteEvaluacion();
        break;

      case 'OPTIONS':
        http_response_code(OK);
        break;

      default:
        http_response_code(NOT_FOUND);
        break;
    }
  }

  private function procesarRequestEvaluar()
  {
    switch ($this->requestMethod) {
      case 'GET':
        $subruta3 = (isset($this->subruta[3])) ? $this->subruta[3] : false ;  // $this->subruta[3] == 'formulario'
        if ($subruta3 == false) {  // si no existe una subruta
          $devolucion = array();
          // getModelo
          $varTemp = $this->evaluaciones->getModelo();
          $varTempRespCode = http_response_code();
          if ($varTempRespCode == OK) {
            array_push($devolucion, $varTemp);
            // getEmpleadosAEvaluar
            $varTemp = $this->evaluaciones->getEmpleadosAEvaluar();
            $varTempRespCode = http_response_code();
            if ($varTempRespCode == OK || $varTempRespCode == NO_CONTENT) { 
              array_push($devolucion, $varTemp);
              return $devolucion;
            }
            else {
              // fallo getEmpleadosAEvaluar
              return $varTemp;
            }
          }
          else {
            // fallo getModelo
            return $varTemp;
          }
        } else if ($subruta3 == 'ranking') {
          // pedido para el ranking de una evaluacion
          return $this->evaluaciones->getRanking();
        } else if ($subruta3 == 'bigBrother') {
          // pedido para el ranking de una evaluacion
          return $this->evaluaciones->getListadoVerComoSupervisor();
        } else if ($subruta3 == 'edicion') {
          // pedido para el ranking de una evaluacion
          return $this->evaluaciones->getDetalleEvaluacionEdicion();
        } else if ($subruta3 == 'comparacion') {
          // pedido para obtener el listado de los empleados evaluados
          return $this->evaluaciones->getEvaluados();
        } else if ($subruta3 == 'ranking-global') {
          return $this->evaluaciones->getRankingGlobal();
        }
        break;
      
      case 'PUT':
        $subruta3 = (isset($this->subruta[3])) ? $this->subruta[3] : false ;  // $this->subruta[3] == 'formulario'
        if ($subruta3 == false) {  // si no existe una subruta
          return $this->evaluaciones->setArchivar();
        } else if ($subruta3 == 'abrir-cerrar') {
          // pedido para abrir o cerrar una evaluacion
          return $this->evaluaciones->setAbrirCerrar();
        }
        break;

      case 'POST':
        return $this->evaluaciones->pushEvaluacion();
        break;

      case 'DELETE':
        return $this->evaluaciones->deleteEvaluacion();
        break;

      case 'OPTIONS':
        http_response_code(OK);
        break;

      default:
        http_response_code(NOT_FOUND);
        break;
    }
  }

  private function procesarRequestComparacion()
  {
    switch ($this->requestMethod) {
      case 'GET':
        $subruta3 = (isset($this->subruta[3])) ? $this->subruta[3] : false ;  // $this->subruta[3] == 'formulario'
        if ($subruta3 == false) {  // si no existe una subruta
          // pedido para obtener el listado de los empleados evaluados
          return $this->evaluaciones->getEvaluados();
        } else if ($subruta3 == 'comparaciones') {
          // pedido para abrir o cerrar una evaluacion
          return $this->evaluaciones->getCompararEvaluacion();
        } else if ($subruta3 == 'devolucion') {
          // pedido para abrir o cerrar una evaluacion
          return $this->evaluaciones->getDevolucionEvaluacion();
        }
        break;
      
      case 'PUT':
        break;

      case 'POST':
        return $this->evaluaciones->pushDevolucion();
        break;

      case 'DELETE':
        break;

      case 'OPTIONS':
        http_response_code(OK);
        break;

      default:
        http_response_code(NOT_FOUND);
        break;
    }
  }

  private function procesarRequestCrearEvaluacion()
  {
    switch ($this->requestMethod) {
      case 'GET':
        $subruta3 = (isset($this->subruta[3])) ? $this->subruta[3] : false ;  // $this->subruta[3] == 'formulario'
        if ($subruta3 == false) {  // si no existe una subruta
          // de momento esta consulta considera que ya hay datos en todas las tablas
          $devolucion = array();
          // obtiene areas y supervisores
          $varTemp = $this->evaluaciones->getAreasYSupervisores();
          $varTempRespCode = http_response_code();
          if ($varTempRespCode == OK) {
            array_push($devolucion, $varTemp);
            // obtiene las competencias y comportamientos
            $varTemp = $this->evaluaciones->getCompetenciasYComportamientos();
            $varTempRespCode = http_response_code();
            if ($varTempRespCode == OK) {
              array_push($devolucion, $varTemp);
              // obtiene el listado de plantillas
              $varTemp = $this->evaluaciones->getListaDePlantillas();
              $varTempRespCode = http_response_code();
              if ($varTempRespCode == OK) {
                array_push($devolucion, $varTemp);
                return $devolucion;
              } else {
                // fallo obtener plantillas
                return $varTemp;
              }
            } else {
              // fallo obtener competencias y comportamientos
              return $varTemp;
            }
          } else {
            // fallo obtener areas y supervisores
            return $varTemp;
          }
        }
        else if ($subruta3 == 'plantilla') {
          // pedido para obtener una plantilla
          return $this->evaluaciones->getPlantilla();
        }
        break;
      
      case 'PUT':
        # code...
        break;

      case 'POST':
        $subruta3 = (isset($this->subruta[3])) ? $this->subruta[3] : false ;  // $this->subruta[3] == 'formulario'
        if ($subruta3 == false) {  // si no existe una subruta
          return $this->evaluaciones->pushNuevaEvaluacion();
        } else if ($subruta3 == 'comp' ) {
          return $this->evaluaciones->pushNuevoComp();
        }
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

}
?>