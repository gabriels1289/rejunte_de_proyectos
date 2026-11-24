<?php
  
  class Logger
  {
    // Declaración de propiedades
    private $servername = "localhost";
    private $username = "root";
    private $password = "";
    private $dbname = "bingointegral";
    private $db2;
    private $ARCHIVO = 'logger.php';

    // Declaración de constantes
    const FATAL = 0;
    const ERROR = 1;
    const WARN = 2;
    const INFO = 3;
    const DEBUG = 4;
    const TRACE = 5;

    private $usuario;

    // Declaración de métodos
    // Constructor de la clase
    public function __construct() {  
      try {
        $this->db2 = new PDO("mysql:host=$this->servername;dbname=$this->dbname;charset=utf8", $this->username, $this->password);
        // set the PDO error mode to exception
        $this->db2->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

        // Actualizo el valor de usuario (el mismo se obtiene en el archivo autorizaciones.php)
        $this->usuario = $GLOBALS["usuarioLog"];
      }
      catch(PDOException $e)
      {
        echo "Error en constructor Logger: " . $e->getMessage();
      }
    }

    // Destructor de la clase
    function __destruct() {
      $this->db2 == null;
    }

    public function getUsuario() {
      return $this->usuario;
    }

    private function guardar($nivel, $mensaje, $archivo, $linea){
      $q = "INSERT INTO logs (nivel, usuario, mensaje, archivo, linea) VALUES ('$nivel', '$this->usuario', '$mensaje', '$archivo', '$linea')";
      $this->db2->query($q); //if($this->db2->query($q)) insertado con exito - lanzar excepion en caso de error?
      $q = "INSERT INTO historicoslogs (nivel, usuario, mensaje, archivo, linea) VALUES ('$nivel', '$this->usuario', '$mensaje', '$archivo', '$linea')";
      $this->db2->query($q); //if($this->db2->query($q)) insertado con exito - lanzar excepion en caso de error?
    }

    public function fatal($_mensaje, $_archivo, $_linea){
      $this->guardar(self::FATAL, $_mensaje, $_archivo, $_linea);
    }

    public function error($_mensaje, $_archivo, $_linea){
      $this->guardar(self::ERROR, $_mensaje, $_archivo, $_linea);
    }

    public function warn($_mensaje, $_archivo, $_linea){
      $this->guardar(self::WARN, $_mensaje, $_archivo, $_linea);
    }

    public function info($_mensaje, $_archivo, $_linea){
      $this->guardar(self::INFO, $_mensaje, $_archivo, $_linea);
    }

    public function debug($_mensaje, $_archivo, $_linea){
      $this->guardar(self::DEBUG, $_mensaje, $_archivo, $_linea);
    }

    public function trace($_mensaje, $_archivo, $_linea){
      $this->guardar(self::TRACE, $_mensaje, $_archivo, $_linea);
    }
  }
?>
