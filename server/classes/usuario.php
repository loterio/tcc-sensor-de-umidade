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
  }

$us = new Usuario('fabio','vitor@gmail.com');

$us->getIdCliente('fabio','vitor@gmail.com');

// criaCliente() - com os dados do formulário, cria um novo registro de usuário;


// alteraCliente(emailNovo, nomeNovo, senhaNova, senha) - altera os dados do usuário desejado.


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
