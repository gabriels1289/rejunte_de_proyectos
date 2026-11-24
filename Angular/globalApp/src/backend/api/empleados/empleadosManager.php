<?php
include 'empleados.php';
$ARCHIVO = 'empleados/empleadosManager.php';

class EmpleadosManager
{
  private $db;  // propiedad para la conexion a la base de datos
  private $subruta; // propiedad para la subruta
  private $requestMethod; // propiedad para el metodo http
  private $querysurl; // propiedad opcional para algun query_stryng
  private $empleados;  // propiedad para la clase Empleados
  private $seguimientos;  // propiedad para la clase Seguimientos
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
    $this->empleados = new Empleados($this->db, $this->requestMethod);
  }

  public function procesarEmpleados()
  {
    switch ($this->subruta[2]) {
      case 'listado':
        echo json_encode($this->procesarRequestEmpleados(), JSON_NUMERIC_CHECK);
        break;

      case 'seguimientos':
        echo json_encode($this->procesarRequestSeguimientos(), JSON_NUMERIC_CHECK);
        break;

      case 'datos-personales':
        echo json_encode($this->procesarRequestDatosPersonales(), JSON_NUMERIC_CHECK);
        break;
      
      case 'evaluaciones':
        echo json_encode($this->procesarRequestEvaluaciones(), JSON_NUMERIC_CHECK);
        break;

      default:
        http_response_code(NOT_FOUND);
        break;
    }
  }

  private function procesarRequestEmpleados()
  {
    switch ($this->requestMethod) {
      case 'GET':
        $subruta3 = (isset($this->subruta[3])) ? $this->subruta[3] : false ;  // $this->subruta[3] == 'detalle'
        if ($subruta3 == false) {
          // ingresó al listado
          return $this->empleados->getListadoDeEmpleados();
        } else {
          // es el pedido a un correo en particular de la bandeja de entrada
          /**
           * Ejecuto 5 funciones:
           * setLecturaBandejaDeEntrada()
           * getDetalleMensaje()
           * getDestinatariosMensaje()
           * getAdjuntos()
           * getBandejaDeEntradaRespuestas()
           */

          $this->mensajeria->setLecturaBandejaDeEntrada();

          $devolucion = array();
          // getDetalleMensaje
          $varTemp = $this->mensajeria->getDetalleMensaje();
          $varTempRespCode = http_response_code();
          if ($varTempRespCode == OK) {
            array_push($devolucion, $varTemp);
            // getDestinatariosMensaje
            $varTemp = $this->mensajeria->getDestinatariosMensaje();
            $varTempRespCode = http_response_code();
            if ($varTempRespCode == OK || $varTempRespCode == NO_CONTENT) {
              array_push($devolucion, $varTemp);
              // getAdjuntos
              $varTemp = $this->mensajeria->getAdjuntos();
              $varTempRespCode = http_response_code();
              if ($varTempRespCode == OK || $varTempRespCode == NO_CONTENT) {
                array_push($devolucion, $varTemp);
                // getBandejaDeEntradaRespuestas
                $varTemp = $this->mensajeria->getBandejaDeEntradaRespuestas();
                $varTempRespCode = http_response_code();
                if ($varTempRespCode == OK || $varTempRespCode == NO_CONTENT) {
                  array_push($devolucion, $varTemp);
                  http_response_code(OK);
                  return $devolucion;
                } else {
                  // http_response_code(INTERNAL_SERVER_ERROR);
                  // return 'fallo getBandejaDeEntradaRespuestas';
                  return $varTemp;
                }
              } else {
                // http_response_code(INTERNAL_SERVER_ERROR);
                // return 'fallo getAdjuntos';
                return $varTemp;
              }
            } else {
              // http_response_code(INTERNAL_SERVER_ERROR);
              // return 'fallo getDestinatariosMensaje';
              return $varTemp;
            }
          } else {
            // http_response_code(INTERNAL_SERVER_ERROR);
            // return 'fallo getDetalleMensaje';
            return $varTemp;
          }
        }
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

  private function procesarRequestSeguimientos()
  {
    switch ($this->requestMethod) {
      case 'GET':
        $subruta3 = (isset($this->subruta[3])) ? $this->subruta[3] : false ;  // $this->subruta[3] == 'sala'
        if ($subruta3 == false) {  // si no existe la ruta es un pedido para un empleado
          $this->seguimientos = new Seguimientos($this->db, $this->requestMethod);
          return $this->seguimientos->getHistorialDeSeguimientos();
        } else {
          // pedido para toda la sala
          $this->seguimientos = new Seguimientos($this->db, $this->requestMethod);
          return $this->seguimientos->getSeguimientosGlobales();
        }
        break;
      
      case 'PUT':
        # code...
        break;

      case 'POST':
        $this->seguimientos = new Seguimientos($this->db, $this->requestMethod);
        return $this->seguimientos->pushSeguimiento();
        break;

      case 'DELETE':
        $this->seguimientos = new Seguimientos($this->db, $this->requestMethod);
        return $this->seguimientos->deleteSeguimiento();
        break;

      case 'OPTIONS':
        http_response_code(OK);
        break;

      default:
        http_response_code(NOT_FOUND);
        break;
    }
  }

  private function procesarRequestDatosPersonales()
  {
    switch ($this->requestMethod) {
      case 'GET':
        $subruta3 = (isset($this->subruta[3])) ? $this->subruta[3] : false ;  // $this->subruta[3] == 'formulario'
        if ($subruta3 == false) {  // si no existe la ruta para el formulario
          return $this->empleados->getDetalleEmpleado();
        } else if ($subruta3 == 'formulario') {
          // pedido para un formulario
          return $this->empleados->getDatosSelectsFormulario();
        } else if ($subruta3 == 'resumen') {
          return $this->empleados->getResumenEmpleado();
        }
        break;
      
      case 'PUT':
        return $this->empleados->setPasswordPorDefecto();
        break;

      case 'POST':
        return $this->empleados->abmEmpleado();
        break;

      case 'DELETE':
        #
        break;

      case 'OPTIONS':
        http_response_code(OK);
        break;

      default:
        http_response_code(NOT_FOUND);
        break;
    }
  }

  private function procesarRequestEvaluaciones()
  {
    switch ($this->requestMethod) {
      case 'GET':
        $subruta3 = (isset($this->subruta[3])) ? $this->subruta[3] : false ;  // $this->subruta[3] == 'detalle'
        if ($subruta3 == false) {
          // pide el listado de evaluaciones
          $this->evaluaciones = new Evaluaciones($this->db, $this->requestMethod);
          return $this->evaluaciones->getEvaluacionesDeEmpleado();
        } else if($subruta3 == 'detalle') {
          // es el pedido a una evaluacion en particular
          /**
           * Ejecuto 3 funciones:
           * getDetalleEvaluacionEmpleado()
           * getCompararEvaluacion()
           * getDevolucionEvaluacion()
           */

          $this->evaluaciones = new Evaluaciones($this->db, $this->requestMethod);

          $devolucion = array();
          // getDetalleEvaluacionEmpleado
          $varTemp = $this->evaluaciones->getDetalleEvaluacionEmpleado();
          $varTempRespCode = http_response_code();
          if ($varTempRespCode == OK) {
            array_push($devolucion, $varTemp);
            // getCompararEvaluacion
            $varTemp = $this->evaluaciones->getCompararEvaluacion();
            $varTempRespCode = http_response_code();
            if ($varTempRespCode == OK) {
              array_push($devolucion, $varTemp);
              // getDevolucionEvaluacion
              $varTemp = $this->evaluaciones->getDevolucionEvaluacion();
              $varTempRespCode = http_response_code();
              if ($varTempRespCode == OK) {
                array_push($devolucion, $varTemp);
                http_response_code(OK);
                return $devolucion;
              } else {
                // http_response_code(INTERNAL_SERVER_ERROR);
                // return 'fallo getDevolucionEvaluacion';
                return $varTemp;
              }
            } else {
              // http_response_code(INTERNAL_SERVER_ERROR);
              // return 'fallo getCompararEvaluacion';
              return $varTemp;
            }
          } else {
            // http_response_code(INTERNAL_SERVER_ERROR);
            // return 'fallo getDetalleEvaluacionEmpleado';
            return $varTemp;
          }
        } else if ($subruta3 == 'reporte') {
          $this->evaluaciones = new Evaluaciones($this->db, $this->requestMethod);
          return $this->evaluaciones->getReporteExcel();
        }
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

}
?>