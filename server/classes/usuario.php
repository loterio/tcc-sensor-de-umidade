<?php
  require_once('../Funcoes.php');

  class Usuario {
    
    function entrar($email, $senha) {
      $comando = "SELECT id 
                    FROM usuario 
                   WHERE email = :email 
                     AND senha = :senha;";
      $arrayStrings = array(":email",":senha");
      $arrayValores = array($email, $senha);
      $busca = executaComandoSql($comando, $arrayStrings, $arrayValores);
      return retornaJsonSelect($busca);
    }

    function cadastrarUsuario($nome, $email, $senha) {
      $arrayStrings = array(":nome",":email",":senha");
      $arrayValores = array($nome, $email, $senha);
      $comando = "INSERT INTO usuario(nome, email, senha) 
                  VALUES (:nome, :email, :senha);";
      executaComandoSql($comando, $arrayStrings, $arrayValores);
      
      $comando = "SELECT id 
                    FROM usuario 
                   WHERE email = :email 
                     AND senha = :senha
                     AND nome  = :nome;";
      $busca = executaComandoSql($comando, $arrayStrings, $arrayValores);
      return retornaJsonSelect($busca);
    }

    function dadosUsuario($id) {
      $comando = "SELECT nome, email
                    FROM usuario
	                 WHERE id = :id;";
      $busca = executaComandoSql($comando, array(":id"), array($id));
      return retornaJsonSelect($busca);
    }

    function alteraDadosUsuario($id, $nome, $email, $senha) {
      $comando = "SELECT id
                    FROM usuario
	                 WHERE id    = :id 
                     AND senha = :senha;";
      $busca = executaComandoSql($comando, array(":id",":senha"), array($id, $senha));
      $validacao = retornaArraySelect($busca);
      if (count($validacao) == 1) {
        $comando = "UPDATE usuario
                     SET nome  = :nome,
                         email = :email
                   WHERE id = :id
                     AND senha = :senha;";
        $arrayStrings = array(":id",":nome",":email",":senha");
        $arrayValores = array($id, $nome, $email, $senha);     
        executaComandoSql($comando, $arrayStrings, $arrayValores);
        return json_encode(array("Sucesso"));
      } else {
        return json_encode(array("Falha"));
      }
    }
  }
?>