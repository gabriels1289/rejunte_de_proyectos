<?php
// include("logger.php");
// include("ChromePhp.php");
// ChromePhp::log("En database.php");
$ARCHIVO = 'conexion.php';

class DatabaseConnector {
  private $dbConexion = null;
  
  public function __construct()
  {
    //localhost
    $servername = "localhost";
    $username = "root";
    $password = "";
    $dbname = "bingointegral";

    try {
      $this->dbConexion = new PDO("mysql:host=$servername;dbname=$dbname;charset=utf8", $username, $password);
      // set the PDO error mode to exception
      $this->dbConexion->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    }
    catch(PDOException $e)
    {
      exit("error: " . $e->getMessage());
    }
  }

  public function getConexion()
  {
    return $this->dbConexion;
  }
}
?>