<?php
  require_once('../Funcoes.php');

  class QuadroTerra {

    private $idUsuario;
    private $pdo;

    public function __construct($idUsuario){
      $this->idUsuario = $idUsuario;
      $this->pdo = new PDO(MYSQL, USER, PASSWORD);
    }

    function listaQuadroTerra(){
      $comando = "SELECT id, nome, proprietario 
                  FROM quadroTerra 
                  WHERE proprietario = :idUsuario;";
      $arrayStrings = array(":idUsuario");
      $arrayValores = array($this->idUsuario);
      $busca = executaComandoSql($comando, $arrayStrings, $arrayValores);
      echo retornaJsonSelect($busca);
    }

    function detalhaQuadroTerra(){
      $comando = "SELECT id as '# Quadro', nome, proprietario, idSensor, latitude, longitude 
                  FROM quadroTerra
                  INNER JOIN sensor
                  WHERE proprietario = :idUsuario
                  AND id = idQuadroTerra;";
      $arrayStrings = array(":idUsuario");
      $arrayValores = array($this->idUsuario);
      $busca = executaComandoSql($comando, $arrayStrings, $arrayValores);
      echo retornaJsonSelect($busca);
    }

    function criaQuadroTerra($proprietario,$nome){
      $comando = "INSERT INTO quadroTerra(proprietario, nome) 
                  VALUES (:proprietario,:nome);";
      $arrayStrings = array(":proprietario",":nome");
      $arrayValores = array($proprietario,$nome);
      $busca = executaComandoSql($comando, $arrayStrings, $arrayValores);
      echo retornaJsonSelect($busca);
    }

  }

  $q = new QuadroTerra(3);

  $q->listaQuadroTerra();

 

?>
