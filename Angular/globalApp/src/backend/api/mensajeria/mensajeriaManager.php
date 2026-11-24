<?php
include 'mensajeria.php';
$ARCHIVO = 'mensajeria/mensajeriaManager.php';

class MensajeriaManager
{
  private $db;  // propiedad para la conexion a la base de datos
  private $subruta; // propiedad para la subruta
  private $requestMethod; // propiedad para el metodo http
  private $querysurl; // propiedad opcional para algun query_stryng
  private $mensajeria;  // propiedad para la clase Mensajeria
  private $seguimientos;  // propiedad para la clase Seguimientos
  
  /**
   * aca inicializo la clase mensajeriaManager y le indico:
   * la subruta(bandeja-de-entrada, bandeja-de-salida, etc..),
   * el tipo de peticion($_GET, $_POST)
   * así en el constructor de la clase mensajeria se como inicializar los parametros de las sql
   * y algun parametro QUERY_STRING pasado en la url
   */

  public function __construct($db, $subruta, $requestMethod, $querysurl=null)
  {
    $this->db = $db;
    $this->subruta = $subruta;
    $this->requestMethod = $requestMethod;
    $this->querysurl = $querysurl;
    //$this->querysurl = (isset($querysurl)) ? $querysurl : null ;
    $this->mensajeria = new Mensajeria($this->db, $this->requestMethod);
  }

  public function procesarMensajeria()
  {
    switch ($this->subruta[2]) {
      case 'bandeja-de-entrada':
        echo json_encode($this->procesarRequestBandejaDeEntrada(), JSON_NUMERIC_CHECK);
        break;

      case 'bandeja-de-salida':
        echo json_encode($this->procesarRequestBandejaDeSalida(), JSON_NUMERIC_CHECK);
        break;

      case 'nuevo-correo':
        echo json_encode($this->procesarRequestNuevoCorreo(), JSON_NUMERIC_CHECK);
        break;
      
      case 'responder':
        echo json_encode($this->procesarRequestResponder(), JSON_NUMERIC_CHECK);
        break;

      case 'reenviar':
        echo json_encode($this->procesarRequestReenviar(), JSON_NUMERIC_CHECK);
        break;

      default:
        http_response_code(NOT_FOUND);
        break;
    }
  }

  private function procesarRequestBandejaDeEntrada()
  {
    switch ($this->requestMethod) {
      case 'GET':
        $subruta3 = (isset($this->subruta[3])) ? $this->subruta[3] : false ;  // $this->subruta[3] == 'respuestas'
        if ($subruta3 == false) {
          // ingresó a bandeja de entrada
          return $this->mensajeria->getBandejaDeEntradaTodos();
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

  private function procesarRequestBandejaDeSalida()
  {
    switch ($this->requestMethod) {
      case 'GET':
        $subruta3 = (isset($this->subruta[3])) ? $this->subruta[3] : false ;  // $this->subruta[3] == 'detalle'
        if ($subruta3 == false) {
          // ingresó a bandeja de entrada
          return $this->mensajeria->getBandejaDeSalida();
        } else {
          // es el pedido a un correo en particular de la bandeja de entrada
          /**
           * Ejecuto 4 funciones:
           * setLecturaBandejaDeSalida()
           * getDetalleMensaje()
           * getDestinatariosMensaje()
           * getAdjuntos()
           * getBandejaDeEntradaRespuestas()
           */

          $this->mensajeria->setLecturaBandejaDeSalida();

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
                http_response_code(OK);
                return $devolucion;
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
        return $this->mensajeria->setPrioridad();
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

  private function procesarRequestNuevoCorreo()
  {
    switch ($this->requestMethod) {
      case 'GET':
        $subruta3 = (isset($this->subruta[3])) ? $this->subruta[3] : false ;  // $this->subruta[3] == 'seguimiento'
        if ($subruta3 == false) {
          // ingresó a nuevo-correo
          return $this->mensajeria->getUsuariosCorreo();
        } else {
          // tiene permitido hacer seguimientos
          $this->seguimientos = new Seguimientos($this->db, $this->requestMethod);

          $devolucion = array();  // arreglo para la devolucion
          // getEmpleadosSeguimiento
          $varTemp = $this->seguimientos->getEmpleadosParaSeguimiento();
          $varTempRespCode = http_response_code();
          if ($varTempRespCode == OK || $varTempRespCode == NO_CONTENT) {
            array_push($devolucion, $varTemp);
            // getTipoDeObservacion
            $varTemp = $this->seguimientos->getTipoDeObservacion();
            $varTempRespCode = http_response_code();
            if ($varTempRespCode == OK || $varTempRespCode == NO_CONTENT) {
              array_push($devolucion, $varTemp);
              http_response_code(OK);
              return $devolucion;
            } else {
              return $varTemp;
            }
          } else {
            return $varTemp;
          }
        }
        break;
      
      case 'PUT':
        // return $this->mensajeria->setPrioridad();
        break;

      case 'POST':
        return $this->mensajeria->pushNuevoCorreo();
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

  private function procesarRequestReenviar()
  {
    switch ($this->requestMethod) {
      case 'GET':
        $subruta3 = (isset($this->subruta[3])) ? $this->subruta[3] : false ;  // $this->subruta[3] == 'seguimiento'
        if ($subruta3 == false) {
          // ingresó a nuevo-correo
          return $this->mensajeria->getUsuariosCorreo();
        }
        else if ($subruta3 == 'seguimiento') {
          // tiene permitido hacer seguimientos
          $this->seguimientos = new Seguimientos($this->db, $this->requestMethod);

          $devolucion = array();  // arreglo para la devolucion
          // getEmpleadosSeguimiento
          $varTemp = $this->seguimientos->getEmpleadosParaSeguimiento();
          $varTempRespCode = http_response_code();
          if ($varTempRespCode == OK || $varTempRespCode == NO_CONTENT) {
            array_push($devolucion, $varTemp);
            // getTipoDeObservacion
            $varTemp = $this->seguimientos->getTipoDeObservacion();
            $varTempRespCode = http_response_code();
            if ($varTempRespCode == OK || $varTempRespCode == NO_CONTENT) {
              array_push($devolucion, $varTemp);
              http_response_code(OK);
              return $devolucion;
            } else {
              return $varTemp;
            }
          } else {
            return $varTemp;
          }
        }
        else if ($subruta3 == 'detalle') {
          return $this->mensajeria->getDetalleReenvio();
        }
        break;
      
      case 'PUT':
        # code
        break;

      case 'POST':
        return $this->mensajeria->pushNuevoCorreo();
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

  private function procesarRequestResponder()
  {
    switch ($this->requestMethod) {
      case 'GET':
          $devolucion = array();  // arreglo para la devolucion
          // getDetalleMensaje
          $varTemp = $this->mensajeria->getDetalleMensaje();
          $varTempRespCode = http_response_code();
          if ($varTempRespCode == OK) {
            array_push($devolucion, $varTemp);
            // getDestinatariosMensaje
            $varTemp = $this->mensajeria->getDestinatariosMensaje();
            $varTempRespCode = http_response_code();
            if ($varTempRespCode == OK) {
              array_push($devolucion, $varTemp);
              http_response_code(OK);
              return $devolucion;
            } else {
              return $varTemp;
            }
          } else {
            return $varTemp;
          }
        break;
      
      case 'PUT':
        # code
        break;

      case 'POST':
        return $this->mensajeria->pushRespuesta();
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