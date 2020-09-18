<?php
  require_once('../Funcoes.php');

  class Usuario {
    
    function entrar($email, $senha) {
      $senhaBD = $this->getSenhaEmail($email);

      if (password_verify($senha, $senhaBD)) {
        $comando = "SELECT id 
                    FROM usuario 
                   WHERE email = :email;";
        $busca = executaComandoSql($comando, array(":email"), array($email));
        return retornaJsonSelect($busca);
      } else 
        return json_encode(array("Dados Incorretos"));
    }

    function cadastrarUsuario($nome, $email, $senha) {

      $senha = password_hash($senha, PASSWORD_DEFAULT);
      $arrayStrings = array(":nome",":email",":senha");
      $arrayValores = array($nome, $email, $senha);

      $comando = "INSERT INTO usuario(nome, email, senha) 
                  VALUES             (:nome, :email, :senha);";
      executaComandoSql($comando, $arrayStrings, $arrayValores);
      
      $arrayStrings = array(":nome",":email");
      $arrayValores = array($nome, $email,);

      $comando = "SELECT id 
                    FROM usuario 
                   WHERE email = :email 
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
      $senhaBD = $this->getSenhaId($id);
      if (password_verify($senha, $senhaBD)) {
        $comando = "UPDATE usuario
                     SET nome  = :nome,
                         email = :email
                   WHERE id = :id;";
        $arrayStrings = array(":id",":nome",":email");
        $arrayValores = array($id, $nome, $email);     
        executaComandoSql($comando, $arrayStrings, $arrayValores);
        return json_encode(array("Sucesso"));
      } else {
        return json_encode(array("Senha incorreta"));
      }
    }

    private function getSenhaId($id){
      $comando = "SELECT senha FROM usuario WHERE id = :id;";
      $busca = executaComandoSql($comando, array(":id"), array($id));
      return $busca->fetch(PDO::FETCH_ASSOC)["senha"];
    }

    private function getSenhaEmail($email){
      $comando = "SELECT senha FROM usuario WHERE email = :email;";
      $busca = executaComandoSql($comando, array(":email"), array($email));
      return $busca->fetch(PDO::FETCH_ASSOC)["senha"];
    }
  }
?>