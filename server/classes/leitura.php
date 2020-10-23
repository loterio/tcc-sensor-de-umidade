<?php
class Leitura
{
  private $idUsuario;

  public function __construct($idUsuario)
  {
    $this->idUsuario = $idUsuario;
  }

  function criaLeitura($idSensor, $umidade)
  {
    $comando = "INSERT INTO leitura(idSensor,umidade) VALUES (:idSensor,:umidade);";
    $arrayStrings = array(":idSensor", ":umidade");
    $arrayValores = array($idSensor, $umidade);
    executaComandoSql($comando, $arrayStrings, $arrayValores);
  }

  function listaLeituras($sensores)
  {
    $leituras = array();
    foreach($sensores as $key){
      $leituras[$key] = $this->pegaLeitura($key);
    }
    return json_encode($leituras);
  }

  private function pegaLeitura($idSensor) {
    $comando = "SELECT dataHora, umidade 
                  FROM leitura 
                 WHERE idSensor = :idSensor 
                ORDER BY dataHora DESC limit 1;";
    $busca = executaComandoSql($comando, array(":idSensor"), array($idSensor));
    $leitura = retornaArraySelect($busca)[0];
    return $leitura;
  }
}
