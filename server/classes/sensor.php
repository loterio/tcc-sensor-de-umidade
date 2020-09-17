<?php
  require_once('../Funcoes.php');

  class Sensor {
    private $idQuadroTerra;
    
    public function __construct($idQuadroTerra){
      $this->idQuadroTerra = $idQuadroTerra;
    }

    function listaSensoresQuadroTerra() {
      $comando = "SELECT id, latitude, longitude 
                    FROM sensor 
                   WHERE idQuadroTerra = :idQuadroTerra;";
      $arrayStrings = array(":idQuadroTerra");
      $arrayValores = array($this->idQuadroTerra);
      $busca = executaComandoSql($comando, $arrayStrings, $arrayValores);
      return retornaJsonSelect($busca);
    }
    
    function listaSensoresLivres($idProprietario) {
      $comando = "SELECT idSensor, latitude, longitude
                    FROM sensor
                   WHERE idQuadroTerra IS NULL
                     AND idProprietario = :idProprietario;";
      $busca = executaComandoSql($comando, array(":idProprietario"), array($idProprietario));
      return retornaJsonSelect($busca);
    }

    function selecionaSensor($idSensor) {
      $comando = "SELECT idSensor, latitude, longitude
                    FROM sensor
                   WHERE idSensor = :idSensor;";
      $busca = executaComandoSql($comando, array(":idSensor"), array($idSensor));
      return retornaJsonSelect($busca);
    }
    
    function adicionaSensor($idSensor) {
      $comando = "UPDATE sensor 
                     SET idQuadroTerra = :idQuadroTerra 
                   WHERE idSensor = :idSensor;";
      $arrayStrings = array(":idQuadroTerra", ":idSensor");
      $arrayValores = array($this->idQuadroTerra, $idSensor);
      executaComandoSql($comando, $arrayStrings, $arrayValores);
    }

    function retiraSensor($idSensor) {
      $comando = "UPDATE sensor 
                     SET idQuadroTerra = NULL 
                   WHERE idSensor = :idSensor;";
      $arrayStrings = array(":idSensor");
      $arrayValores = array($idSensor);
      executaComandoSql($comando, $arrayStrings, $arrayValores);
    }

    function criaSensor($latitude, $longitude) {
      $comando = "INSERT INTO sensor
                         (latitude, longitude) 
                  VALUES (:latitude, :longitude);";
      $arrayStrings = array(":latitude", ":longitude");
      $arrayValores = array($latitude, $longitude);
      executaComandoSql($comando, $arrayStrings, $arrayValores);
    }

    function alteraSensor($idSensor, $latitude, $longitude) {
      $comando = "UPDATE sensor 
                     SET latitude = :latitude,
                         longitude = :longitude
                   WHERE idSensor = :idSensor;";
      $arrayStrings = array(":idSensor", ":latitude", ":longitude");
      $arrayValores = array($idSensor, $latitude, $longitude);
      executaComandoSql($comando, $arrayStrings, $arrayValores);
    }

    function excluiSensor($idSensor) {
      $comando = "DELETE 
                    FROM sensor 
                   WHERE idSensor = :idSensor;";
      $arrayStrings = array(":idSensor");
      $arrayValores = array($idSensor);
      executaComandoSql($comando, $arrayStrings, $arrayValores);
    }
  }

  $s = new Sensor(1);
  echo $s->listaSensoresLivres(1);
?>
