<?php
class Sensor
{
  private $idUsuario;

  public function __construct($idUsuario)
  {
    $this->idUsuario = $idUsuario;
  }

  function criaSensor($longitude, $latitude)
  {
    $comando = "INSERT INTO sensor(idProprietario, latitude, longitude) 
                VALUES (:idProprietario, :latitude, :longitude);";
    $arrayStrings = array(":idProprietario", ":latitude", ":longitude");
    $arrayValores = array($this->idUsuario, $latitude, $longitude);
    executaComandoSql($comando, $arrayStrings, $arrayValores);

    $comando = "SELECT idSensor FROM sensor 
                 WHERE idProprietario = :idProprietario 
                 ORDER BY idSensor DESC limit 1;";
    $busca = executaComandoSql(
      $comando,
      array(":idProprietario"),
      array($this->idUsuario)
    );
    $idSensor = retornaArraySelect($busca)[0]['idSensor'];
    return json_encode(array("idSensor" => $idSensor));
  }

  function listaSensores()
  {
    $comando = "SELECT idSensor, longitude, latitude FROM sensor WHERE idProprietario = :idProprietario;";
    $busca = executaComandoSql($comando, array(":idProprietario"), array($this->idUsuario));
    return retornaJsonSelect($busca);
  }

  function atualizaSensor($idSensor, $longitude, $latitude)
  {
    $comando = "UPDATE sensor 
                   SET latitude = :latitude, 
                       longitude = :longitude
                 WHERE idSensor = :idSensor;";
    $arrayStrings = array(":idSensor", ":latitude", ":longitude");
    $arrayValores = array($idSensor, $latitude, $longitude);
    executaComandoSql($comando, $arrayStrings, $arrayValores);
  }
  
}
