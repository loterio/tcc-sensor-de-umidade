<?php
  require_once('../Funcoes.php');

  class QuadroTerra {

    private $idUsuario;

    public function __construct($idUsuario){
      $this->idUsuario = $idUsuario;
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

    function criaQuadroTerra($proprietario,$nome){
      $comando = "INSERT INTO quadroTerra(proprietario, nome) 
                  VALUES (:proprietario,:nome);";
      $arrayStrings = array(":proprietario",":nome");
      $arrayValores = array($proprietario,$nome);
      $busca = executaComandoSql($comando, $arrayStrings, $arrayValores);
      echo retornaJsonSelect($busca);
    }

    function alteraQuadroTerra($novoProprietario,$novoNome){
      $comando = "UPDATE quadroTerra
                  SET nome = :novoNome,
                      proprietario = :novoProprietario
                  WHERE ";
      $arrayStrings = array(":novoNome",":novoProprietario");
      $arrayValores = array($nome,$proprietario);
      $busca = executaComandoSql($comando, $arrayStrings, $arrayValores);
      echo retornaJsonSelect($busca);
    }

    

  }

  $q = new QuadroTerra(3);

  $q->listaQuadroTerra();

?>
