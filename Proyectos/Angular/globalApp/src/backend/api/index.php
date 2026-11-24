<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: PUT, GET, POST, DELETE, HEAD, OPTIONS");
header("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept, Authorization");
//header("Access-Control-Allow-Headers: *");

include_once 'conexion.php';
include_once 'logger.php';
include_once 'codigosDeRespuestasHttp.php';
include 'mensajeria/mensajeriaManager.php';
include 'empleados/empleadosManager.php';
include 'evaluaciones/evaluacionesManager.php';
include 'recursos/recursosManager.php';
include 'ajustes/ajustesManager.php';
include_once 'ChromePhp.php';
include_once 'autorizaciones.php';
include 'seguimientos/seguimientos.php';
include 'evaluaciones/evaluaciones.php';
include 'recursos/recursos.php';
include 'ajustes/ajustes.php';
include 'excel/PHPExcel.php';

$ARCHIVO = 'index.php';

$dbConnection = (new DatabaseConnector())->getConexion(); // Me conecto a la base de datos
// $mensajeria = new Mensajeria($dbConnection);

// analizo la URL y devuelvo un array asociativo que contiene aquellos componentes presentes en la URL
$uri = $_SERVER['PATH_INFO']; // ejemplo: /mensajeria/bandeja-de-entrada/
$uri = explode('/', $uri);  // devuelve: ['', 'mensajeria', 'bandeja-de-entrada']
// echo json_encode($var);

// anbalizo los parametros de query en la ruta y los devulevo como un arreglo
$querysurl = (isset($_SERVER['QUERY_STRING'])) ? $_SERVER['QUERY_STRING'] : null ; // ejemplo: idusuario=987
$querysurl = (isset($querysurl)) ? explode('=', $querysurl) : null;  // devuelve: ['idusuario', '987']
// echo json_encode($querys);

$requestMethod = $_SERVER['REQUEST_METHOD'];

// segmento de control de token jwt
$autorizaciones = new Autorizaciones($dbConnection);  // instancio la clase para logueo autentificacion y control de token
$continuarConLaPeticion = false;  // flag para ver si continuo la peticion de ruta
$tokenRecibido = $autorizaciones->getBearerToken();//Autorizaciones::getBearerToken();  // obtengo el resultado del checkeo del token o null si este no existe porque aun no se ha logueado
// si no recibi un token verifico antes de continuar que la ruta sea a un login
if($tokenRecibido == null && $uri[1]=='login')
{
  $continuarConLaPeticion = true;
} else if ($tokenRecibido != null) {
  // existe un token, verifico que sea valido antes de setear el flag de continuacion
  // Obtengo y proceso el codigo de respuesta en funcion al analisis del token
  
  if($tokenRecibido == EXPECTATION_FAILED) {
    // token no coincide con el de la db
    $continuarConLaPeticion = false;
  } else if($tokenRecibido == REQUEST_TIMEOUT) {
    // token expirado
    $continuarConLaPeticion = false;
  } else if($tokenRecibido == UNAUTHORIZED) {
    // UNAUTHORIZED firma invalida
    $continuarConLaPeticion = false;
  } else {
    $continuarConLaPeticion = true;
  }

} else {
  $continuarConLaPeticion = false;
  // No existe un token, genero aviso UNAUTHORIZED
  // http_response_code(UNAUTHORIZED);
}

// Antes de proseguir con las rutas verifico si puedo continuar la peticion, caso contrario devuelvo la alerta por token
if ($continuarConLaPeticion) {
  /**
  * busco que la uri pertenezca a alugna ruta valida
  * en función de la misma puedo verificar sub-rutas
  */
  switch ($uri[1]) {
    case 'mensajeria':
      /**
       * aca inicializo la clase mensajeriaManager y le indico:
       * la subruta(bandeja-de-entrada, bandeja-de-salida, etc..),
       * el tipo de peticion($_GET, $_POST)
       * así en el constructor de la clase mensajeria se como inicializar los parametros de las sql
       * y algun parametro QUERY_STRING pasado en la url
       */
      $mensajeriaManager = new MensajeriaManager($dbConnection, $uri, $requestMethod, $querysurl);
      $mensajeriaManager->procesarMensajeria();
      break;
    
    case 'empleados':
      $empleadosManager = new EmpleadosManager($dbConnection, $uri, $requestMethod, $querysurl);
      $empleadosManager->procesarEmpleados();
      break;

    case 'evaluaciones':
      $evaluacionesManager = new EvaluacionesManager($dbConnection, $uri, $requestMethod, $querysurl);
      $evaluacionesManager->procesarEvaluaciones();
      break;

    case 'recursos':
      $recursosManager = new RecursosManager($dbConnection, $uri, $requestMethod, $querysurl);
      $recursosManager->procesarRecursos();
      break;

    case 'ajustes':
      $ajustesManager = new AjustesManager($dbConnection, $uri, $requestMethod, $querysurl);
      $ajustesManager->procesarAjustes();
      break;

    case 'reportes':
      # code...
      break;

    case 'admin':
      # code...
      break;

    case 'login':
      //$autorizaciones = new Autorizaciones($dbConnection);
      $autorizaciones->login();
      break;

    case 'logout':
      //$autorizaciones = new Autorizaciones($dbConnection);
      $autorizaciones->logout();
      break;

    default:
      // se desconoce la ruta, se devuleve el codigo 404 Not Found
      http_response_code(NOT_FOUND);
      // exit();
      break;
  }
} else {
  ChromePhp::log("token recibido: ".$tokenRecibido);
  if($tokenRecibido == EXPECTATION_FAILED) {
    // token no coincide con el de la db
    $mensajeDeError = "token difiere con la db";
  } else if($tokenRecibido == REQUEST_TIMEOUT) {
    // token expirado
    $mensajeDeError = "token expirado";
  } else if($tokenRecibido == UNAUTHORIZED) {
    // UNAUTHORIZED firma invalida
    $mensajeDeError = "firma de token invalida";
  }

  // deslogueo al usuario por problema de token
  $autorizaciones->logout();
  // envio la respuesta al cliente
  http_response_code($tokenRecibido);
  echo json_encode($mensajeDeError, JSON_NUMERIC_CHECK);
}


?>