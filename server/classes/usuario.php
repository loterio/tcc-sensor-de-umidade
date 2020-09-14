<?php

  require_once('../Conexao.php');

// getIdUsuario() - verifica e retorna se um usuário já está cadastrado no sistema;

class Usuario {
  public $email = "";
  public $senha = "";

  function getIdUsuario($id){
    
    try{
      $pdo = new PDO(MYSQL, USER, PASSWORD);

      $res = $pdo->prepare("SELECT * FROM cliente WHERE login = $id;");
      $res->execute();

      $resultado = $res->fetch(PDO::FETCH_ASSOC);
      $resultado = json_encode($resultado);

      echo $resultado;
    
    } catch(PDOException $e) {
    	echo '<strong>Error:</strong> '.$e->getMessage();
    }  

  } 
}

$us = new Usuario();

$us->getIdUsuario('fabio');

// criaUsuario() - com os dados do formulário, cria um novo registro de usuário;


// alteraUsuario(emailNovo, nomeNovo, senhaNova, senha) - altera os dados do usuário desejado.


  // try{

  //   $pdo = new PDO(MYSQL, USER, PASSWORD);
  //   $res = $pdo->prepare("SELECT * FROM cliente;");
  //   $res->execute();
  //   $resultado = $res->fetch(PDO::FETCH_ASSOC);
  //   $resultado = json_encode($resultado);
  //   echo $resultado;

	// } catch(PDOException $e) {
	// 	echo '<strong>Error:</strong> '.$e->getMessage();
  // }  

?>
