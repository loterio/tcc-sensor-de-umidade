<?php
  require_once('../Conexao.php');

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
      $resultado = $res->fetch(PDO::FETCH_ASSOC);

      

      // while ($row = $res->fetch(PDO::FETCH_ASSOC)) {
      //   // echo "{$row['id']} - {$row['nome']}<br/>";

      //   json_encode($row);

      //   echo $row;
      // }

      
      // $resultado = json_encode($resultado);

      // echo $resultado;
      // return $resultado;
    }

    function detalhaQuadroTerra($id){

      $res = $this->pdo->prepare("SELECT * FROM :loginUsuario;");
      $res->bindparam(":loginUsuario",$nome);
      $res->execute();

      $resultado = $res->fetch(PDO::FETCH_ASSOC);
      $resultado = json_encode($resultado);

      // echo $resultado;
      return $resultado;
    }

  }

  $q = new QuadroTerra(3);

  $q->listaQuadrosTerra();

 

?>
