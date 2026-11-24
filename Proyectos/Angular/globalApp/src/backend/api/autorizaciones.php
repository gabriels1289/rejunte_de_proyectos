<?php

// variable global utilizada en autorizaciones.php y logger.php
$usuarioLog;

class Autorizaciones
{
  private $db = null; // propiedad para la conexion a la DB
  private $subruta; // propiedad para la subruta
  private $requestMethod; // propiedad para el metodo http
  private $querysurl; // propiedad opcional para algun query_stryng
  private $data;  // propiedad para recuperar los datos pasados por la llamada http
  private $username;  // propiedades para las funciones login y logout
  private $idusuario;
  private $password;
  private $secretKey = 'Binusr08';  // para generar el signature
  private $token;
  private $ARCHIVO = 'autorizaciones.php';
  private $logger;
  
  public function __construct($db, $subruta=null, $requestMethod=null, $querysurl=null)
  {
    $this->db = $db;
    $this->subruta = $subruta;
    $this->requestMethod = $requestMethod;
    $this->querysurl = $querysurl;
    
    // obtengo los datos e intento inicializar las propiedades
    $this->data = json_decode(file_get_contents("php://input"));
    $this->username = (isset($this->data->username)) ? $this->data->username : null ;
    $this->password = (isset($this->data->password)) ? sha1($this->data->password) : null ;
    $this->token = (isset($this->data->token)) ? $this->data->token : null ;

    // Obtengo y seteo  para el resto de los archivos el valor del usuario para los logs
    // El mismo se vuelve a refrescar con los datos incluidos en el token jwt
    $GLOBALS["usuarioLog"] = $this->username;
    $this->logger = new Logger();
  }

  private function buscarHijos($padre, &$resultado, &$arbol)
  {
    if(!empty($arbol))
    {
      $listahijos = array();
      foreach($arbol as $item) //obtengo el conjunto C (hijos del padre pasado como parametro)
      {
        if($item["padre"] == $padre)
        {
          $listahijos[] = $item;
        }
      }
      if(empty($listahijos))
      {
        return;
      }
      else
      {
        foreach($listahijos as $hijo)
        {
          if($this->busquedaIndice($resultado, $hijo)<0)
          {
            $resultado[] = $hijo;
            $this->buscarHijos($hijo["idacceso"], $resultado, $arbol);
          }
        }
      }
    }
  }

	private function busquedaIndice($arbol, $item)
  {
    $arrlength = count($arbol);
    $pos = -1;
    for($i=0; $i<$arrlength; $i++)
    {
      if($arbol[$i]["idacceso"] == $item["idacceso"]) //if($arbol[$i]["id"] == $item["id"])
      {
        $pos = $i;
      }
    }
    return $pos;
  }

  public function login()
  {
    try {
      $userInfo = $this->db->query("SELECT e.id AS id, e.nombre, e.apellido, s.area, e.sector AS idarea, e.username, e.acceso AS idacceso, a.acceso as rol, aa.padre AS idaccesopadre, 
                                  aa.correorestringido, IF(e.foto IS NULL OR e.foto = '', '../../assets/img/user.jpg', e.foto) AS foto
                                FROM empleados e
                                INNER JOIN sectores s ON e.sector=s.id INNER JOIN accesos a ON e.acceso=a.id INNER JOIN arbolaccesos aa ON e.acceso=aa.idacceso
                                WHERE e.username='$this->username' AND e.password='$this->password' AND e.activo='SI' LIMIT 1");
      $userInfo = $userInfo->fetchAll(PDO::FETCH_ASSOC);

      $this->token = null;
      if (count($userInfo) == 1){
        //This means that the user is logged in and let's givem a token :D :D :D
        // $this->token = $this->username . "." . uniqid() . "." . uniqid();
            //$token = uniqid() . uniqid() . uniqid();
        
        $this->idusuario = $userInfo[0]['id'];
        
        $this->token = $this->generateToken();

        $q = "UPDATE empleados SET token=:token WHERE username=:username AND password=:password";
        $query = $this->db->prepare($q);
        $execute = $query->execute(array(
            ":token" => $this->token,
            ":username" => $this->username,
            ":password" => $this->password
        ));

        //var_dump($userInfo);
        $userInfo[0]["token"] = $this->token;
        $nodobase = $userInfo[0]["idacceso"];

        // array_push($userInfo, $token);
        // obtengo el arreglo de datos para buscar los hijos del acceso
        $consulta = $this->db->query("SELECT aa.id, aa.idacceso, aa.padre, a.acceso FROM arbolaccesos aa, accesos a WHERE aa.idacceso=a.id");
        $tablaArbol = $consulta->fetchAll(PDO::FETCH_ASSOC);

        $grupoDeHijos = array();
        $this->buscarHijos($nodobase, $grupoDeHijos, $tablaArbol);

        $listaIdHijos = '';
        if(!empty($grupoDeHijos))
        {
          foreach ($grupoDeHijos as $item) {
            $listaIdHijos.=$item["idacceso"].",";
          }
          // elimino la ultima coma: trailing comma
          $listaIdHijos = trim($listaIdHijos, ',');
        }
        // array_push($userInfo, $listaIdHijos);
        $userInfo[0]["listaIdHijos"] = $listaIdHijos;
        $userInfo = $userInfo[0];
        
        $this->logger->info("logueo exitoso", $this->ARCHIVO, __LINE__);
        http_response_code(OK);
        echo json_encode($userInfo, JSON_NUMERIC_CHECK);
      }else {
        $this->logger->warn("intento de logueo fallido", $this->ARCHIVO, __LINE__);
        http_response_code(NOT_FOUND);
        echo "fallo consulta";
      }
    } catch (Exception $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
		  echo "error: ".$e->getMessage();
    }
  }

  public function logout()
  {
    try {
      $ejecutado = $this->db->query("UPDATE empleados SET token='LOGGED OUT' WHERE token='$this->token'");
  
      $ejecutado = $ejecutado->rowCount();
  
      if ($ejecutado >= 1){
        //echo $ejecutado." columnas actualizadas";
        $this->logger->info("deslogueado", $this->ARCHIVO, __LINE__);
        http_response_code(OK);
        // TODO: implementar un servicio de codigos de respuesta
      }
    } catch (Exception $e) {
      $this->logger->fatal($e->getMessage(), $this->ARCHIVO, __LINE__);
      http_response_code(INTERNAL_SERVER_ERROR);
      echo "error: ".$e->getMessage();
    }
  }

  /** 
   * Get header Authorization
   * */
  private static function getAuthorizationHeader(){
    $headers = null;
    if (isset($_SERVER['Authorization'])) {
      $headers = trim($_SERVER["Authorization"]);
    } else if (isset($_SERVER['HTTP_AUTHORIZATION'])) { //Nginx or fast CGI
      $headers = trim($_SERVER["HTTP_AUTHORIZATION"]);
    } else if (function_exists('apache_request_headers')) {
      $requestHeaders = apache_request_headers();
      // Server-side fix for bug in old Android versions (a nice side-effect of this fix means we don't care about capitalization for Authorization)
      $requestHeaders = array_combine(array_map('ucwords', array_keys($requestHeaders)), array_values($requestHeaders));
      //print_r($requestHeaders);
      if (isset($requestHeaders['Authorization'])) {
        $headers = trim($requestHeaders['Authorization']);
      }
    }
    return $headers;
  }

  /**
  * get access token from header
  * */
  public function getBearerToken() {
    $headers = self::getAuthorizationHeader();
    // HEADER: Get the access token from the header
    if (!empty($headers)) {
      if (preg_match('/Bearer\s(\S+)/', $headers, $matches)) {
        //TODO: antes de retornar deberia verificar el token recibido
        return $this->checkToken($matches[1]);
        //return $matches[1];
      }
    }
    return null;
  }

  /**
   * metodos de soporte para base64url_encode y decode
  */
  private static function base64url_encode($data) {
		return rtrim(strtr(base64_encode($data), '+/', '-_'), '=');
	}

	// tener en cuenta que habia varias versiones de esta función.
	private static function base64url_decode($data) {
		return base64_decode(str_pad(strtr($data, '-_', '+/'), strlen($data) % 4, '=', STR_PAD_RIGHT));
  }

  private function generateToken() {
    // Token structure
    // header.payload.signature
    
    // Create token header (ALGORITHM & TOKEN TYPE) as a JSON string
    $header = json_encode(['typ' => 'JWT', 'alg' => 'HS256']);
    // Encode Header to Base64Url String
    $base64UrlHeader = self::base64url_encode($header);

    // Create token payload (DATA) as a JSON string
    $time = time();
    $data = [
      "idusuario" => $this->idusuario,
      "username" => $this->username,
      "iat" => $time, // tiempo de inicio del token
      "exp" => $time + (60*60)  // tiempo de expiracion del token. Una hora (60 segundos * 60 minutos)
    ];
    $payload = json_encode($data);
    // Encode Payload to Base64Url String
    $base64UrlPayload = self::base64url_encode($payload);

    // Create Signature Hash
    $signature = hash_hmac('sha256', $base64UrlHeader . "." . $base64UrlPayload, $this->secretKey, true);
    
    // Encode Signature to Base64Url String
    $base64UrlSignature = self::base64url_encode($signature);
    
    // Create JWT
    $jwt = $base64UrlHeader . "." . $base64UrlPayload . "." . $base64UrlSignature;
    return $jwt;
  }

  private function checkToken($tokenObtenido) {
    // checkSignature
    // Split a string by '.' 
    $jwtValues = explode('.', $tokenObtenido);

    // extracting the signature from the original JWT 
    $recievedSignature = $jwtValues[2];
    
    // concatenating the first two arguments of the $jwt_values array, representing the header and the payload
    $recievedHeaderAndPayload = $jwtValues[0] . '.' . $jwtValues[1];
    
    // creating the Base 64 encoded new signature generated by applying the HMAC method to the concatenated header and payload values
    $resultedSignature = self::base64url_encode(hash_hmac('sha256', $recievedHeaderAndPayload, $this->secretKey, true));

    // checking if the created signature is equal to the received signature
    if($resultedSignature == $recievedSignature) {
      // If everything worked fine, if the signature is ok and the payload was not modified you should get a success message
      
      // trato de recuperar el payload
      $receivedPayload = self::base64url_decode($jwtValues[1]);
      $receivedData = json_decode($receivedPayload, true);
      
      // verifico si el token no se vencio
      if(time() < $receivedData['exp']) {
        // Obtengo y seteo  para el resto de los archivos el valor del usuario para los logs
        $GLOBALS["usuarioLog"] = $receivedData['username'];
        $usuarioTokenAux = $receivedData['username'];
        
        // Comparo el token contra el almacenado en la DB
        $userInfo = $this->db->query("SELECT id, token FROM empleados WHERE username='$usuarioTokenAux'");
        $userInfo = $userInfo->fetchAll(PDO::FETCH_ASSOC);
        
        if($tokenObtenido == $userInfo[0]['token']) {
          // ChromePhp::log("token verificado en la DB");
          return $tokenObtenido;
        } else {
          // ChromePhp::log("token no hallado en la DB");
          return EXPECTATION_FAILED;
        }
      } else {
        // ChromePhp::log("vencido");
        // Me aseguro de no guardar un log sin usuario
        if ($this->logger->getUsuario() != "") {
          $this->logger->warn("token vencido", $this->ARCHIVO, __LINE__);  
        }
        return REQUEST_TIMEOUT;
      }
      
    } else {
      // ChromePhp::log("firma invaliida");
      $this->logger->fatal("firma invalida", $this->ARCHIVO, __LINE__);
      return UNAUTHORIZED;
    }
  }

}

?>