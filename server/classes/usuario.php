<?php

  require_once('../Conexao.php');

// getIdUsuario() - verifica e retorna se um usuário já está cadastrado no sistema;

  class Usuario {
    private $login;
    private $email;
    private $pdo;

    public function __construct($login, $email){
      $this->login = $login;
      $this->email = $email;
      $this->pdo = new PDO(MYSQL, USER, PASSWORD); # não consegui usar o Conexao.php e fiz assim
    }

    function getIdCliente($nome,$email){
      $nome = "%".$nome."%";
      $r;

        $res = $this->pdo->prepare("SELECT login, email 
                                    FROM cliente 
                                    WHERE login LIKE :loginCliente 
                                    AND email = :email;");
        $res->bindparam(":loginCliente",$nome);
        $res->bindparam(":email",$email);
        $res->execute();
        $resultado = $res->fetch(PDO::FETCH_ASSOC);
        $resultado = json_encode($resultado);
        // $resultado = json_decode($resultado);

        // echo count($resultado);      # retorna 2, cliente(login, email)

        // if(count($resultado) = 2){
        //   $r = true;
        //   return $r;               nesse trecho tentei fazer uma verificação
        // }else{                     que ia retornar TRUE se $resultado = 2
        //   $r = false;              o que quer dizer que a consulta do banco 
        //   return $r;               retornaria só um registro, sendo este o único no BD.    
        // }                          mas dá erro kkk

        return $resultado;
    }

    // criaCliente() - com os dados do formulário, cria um novo registro de usuário;

    function criaUsuario($login, $email, $senha) {
      $res = $this->pdo->prepare("INSERT INTO cliente
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

// alteraCliente(emailNovo, nomeNovo, senhaNova, senha) - altera os dados do usuário desejado.
    
    function alteraUsuario($login, $email, $senha) {

      $res = $this->pdo->prepare("UPDATE cliente
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
  //   $res = $pdo->prepare("SELECT * FROM cliente;");
  //   $res->execute();
  //   $resultado = $res->fetch(PDO::FETCH_ASSOC);
  //   $resultado = json_encode($resultado);
  //   echo $resultado;

	// } catch(PDOException $e) {
	// 	echo '<strong>Error:</strong> '.$e->getMessage();
  // }  

# ------------------------ TESTES ------------------------

$us = new Usuario('fabio','vitor@gmail.com');

$us->alteraUsuario('Fabio Loterio','vitorloterio@gmail.com','schneider');

?>
