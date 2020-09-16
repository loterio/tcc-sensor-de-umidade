<?php
  require_once('../Funcoes.php');

  class QuadroTerra {

    private $idUsuario;
    private $pdo;

    public function __construct($idUsuario){
      $this->idUsuario = $idUsuario;
      $this->pdo = new PDO(MYSQL, USER, PASSWORD);
    }

    function listaQuadrosTerra(){
      $res = $this->pdo->prepare("SELECT * FROM quadroTerra;");
      $res->execute();
      return retornaJsonSelect($res);
      // echo $resultado;
    }

    function detalhaQuadroTerra($id){
      $comando = "SELECT id, nome, proprietario 
                  FROM quadroTerra 
                  WHERE proprietario = :idUsuario;";
      $arrayStrings = array(":idUsuario");
      $arrayValores = array($id);
      $busca = executaComandoSql($comando, $arrayStrings, $arrayValores);
      $resultado = retornaJsonSelect($busca);

      echo $resultado;
    }

  }

  $q = new QuadroTerra(2);

  $q->detalhaQuadroTerra(3);

 

?>
