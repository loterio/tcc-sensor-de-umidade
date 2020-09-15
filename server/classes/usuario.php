<?php

  require_once('../Conexao.php');

  class Usuario {
    private $login;
    private $email;
    private $senha;
    private $pdo;

    public function __construct($login, $email, $senha){
      $this->login = $login;
      $this->email = $email;
      $this->senha = $senha;
      $this->pdo = new PDO(MYSQL, USER, PASSWORD); # não consegui usar o Conexao.php e fiz assim
    }

    // getIdUsuario() - verifica e retorna se um usuário já está cadastrado no sistema;
    
    function getIdUsuario($nome){

        $res = $this->pdo->prepare("SELECT id, login, email 
                                    FROM usuario 
                                    WHERE login = :loginUsuario;");
        $res->bindparam(":loginUsuario",$nome);
        $res->execute();
        $resultado = $res->fetch(PDO::FETCH_ASSOC);
        $resultado = json_encode($resultado);kkk
        // echo $resultado;
        return $resultado;
    }

    // criaUsuario() - com os dados do formulário, cria um novo registro de usuário;

    function criaUsuario($login, $email, $senha) {
      $res = $this->pdo->prepare("INSERT INTO usuario
                                ( login,
                                  email,
                                  senha )
                                  VALUES (:login, 
                                          :email, 
                                          :senha)");
      $res->bindparam(":login", $login);
      $res->bindparam(":email", $email);
      $res->bindparam(":senha", $senha);
      $res->execute();
    }

// alteraUsuario(emailNovo, nomeNovo, senhaNova, senha) - altera os dados do usuário desejado.
    
    function alteraUsuario($login, $email, $senha) {

      $res = $this->pdo->prepare("UPDATE usuario
                              SET login = :login,
                                  email = :email,
                                  senha = :senha                    # atualiza, passando os dados que estão nos parâmetros da func
                                  WHERE login LIKE :loginAtual      # para o registro que tiver os dados iguais aos valores
                                  AND email LIKE :emailAtual;");    # privados da Classe(login e email).
      $res->bindparam(":login", $login);
      $res->bindparam(":email", $email);
      $res->bindparam(":senha", $senha);
      $res->bindparam(":loginAtual", $this->login);
      $res->bindparam(":emailAtual", $this->email);
      $res->execute();
    }

  } #fecha a classe Usuario



  // try{

  //   $pdo = new PDO(MYSQL, USER, PASSWORD);
  //   $res = $pdo->prepare("SELECT * FROM usuario;");
  //   $res->execute();
  //   $resultado = $res->fetch(PDO::FETCH_ASSOC);
  //   $resultado = json_encode($resultado);
  //   echo $resultado;

	// } catch(PDOException $e) {
	// 	echo '<strong>Error:</strong> '.$e->getMessage();
  // }  

# ------------------------ TESTES ------------------------

$us = new Usuario('fabio','vitor@gmail.com','ashw');

// $us->alteraUsuario('Fabio Loterio','vitorloterio@gmail.com','schneider');
// $us->criaUsuario('mario','super@gmail.com','111');
$us->getIdUsuario('fabio');

?>
