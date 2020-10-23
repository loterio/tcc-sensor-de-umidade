<?php
class Usuario
{

  function entrar($email, $senha)
  {
    $senhaBD = $this->getSenhaEmail($email);

    if (password_verify($senha, $senhaBD)) {
      $comando = "SELECT id 
                    FROM usuario 
                   WHERE email = :email;";
      $busca = executaComandoSql($comando, array(":email"), array($email));
      $usuario = retornaArraySelect($busca);

      $id = $usuario[0]['id'];
      $token = geraToken($id);

      $comando = "UPDATE usuario
                      SET token = :token
                    WHERE id = :id;";
      $arrayStrings = array(":token", ":id");
      $arrayValores = array($token, $id);
      executaComandoSql($comando, $arrayStrings, $arrayValores);

      return json_encode(
        array(
          "resposta" => "sucesso",
          "token" => $token
        )
      );
    } else
      return json_encode(
        array(
          "resposta" => "dados_invalidos"
        )
      );
  }

  function cadastrarUsuario($nome, $email, $senha)
  {

    $senha = password_hash($senha, PASSWORD_DEFAULT);

    $arrayStrings = array(":email");
    $arrayValores = array($email);

    $comando = "SELECT id FROM usuario WHERE email = :email";
    $busca = executaComandoSql($comando, $arrayStrings, $arrayValores);

    if (retornaJsonSelect($busca) == '[]') {
      $arrayStrings = array(":nome", ":email", ":senha");
      $arrayValores = array($nome, $email, $senha);

      $comando = "INSERT INTO usuario(nome, email, senha) 
                    VALUES             (:nome, :email, :senha);";
      executaComandoSql($comando, $arrayStrings, $arrayValores);

      $arrayStrings = array(":nome", ":email");
      $arrayValores = array($nome, $email,);

      $comando = "SELECT id 
                      FROM usuario 
                    WHERE email = :email 
                      AND nome  = :nome;";
      $busca = executaComandoSql($comando, $arrayStrings, $arrayValores);
      $usuario = retornaArraySelect($busca);

      $id = $usuario[0]['id'];
      $token = geraToken($id);
      $tokenRede = geraTokenRede($id);

      $comando = "UPDATE usuario
                      SET token = :token,
                          tokenRede = :tokenRede
                    WHERE id = :id;";
      $arrayStrings = array(":token", ":tokenRede", ":id");
      $arrayValores = array($token, $tokenRede, $id);
      executaComandoSql($comando, $arrayStrings, $arrayValores);

      return json_encode(
        array(
          "resposta" => "sucesso",
          "token" => $token
        )
      );
    } else {
      return json_encode(
        array(
          "resposta" => "usuario_existente"
        )
      );
    }
  }

  function sair($id)
  {
    $comando = "UPDATE usuario
                     SET token = null
                   WHERE id = :id;";
    executaComandoSql($comando, array(":id"), array($id));
  }

  function atualizaToken($id)
  {
    $token = geraToken($id);

    $comando = "UPDATE usuario
                     SET token = :token
                   WHERE id = :id;";
    $arrayStrings = array(":token", ":id");
    $arrayValores = array($token, $id);
    executaComandoSql($comando, $arrayStrings, $arrayValores);

    return json_encode(
      array(
        "token" => $token
      )
    );
  }

  function dadosUsuario($id)
  {
    $comando = "SELECT nome, email
                    FROM usuario
	                 WHERE id = :id;";
    $busca = executaComandoSql($comando, array(":id"), array($id));
    return retornaJsonSelect($busca);
  }

  function alteraDadosUsuario($id, $nome, $email, $senha)
  {
    $senhaBD = $this->getSenhaId($id);
    if (password_verify($senha, $senhaBD)) {
      $comando = "UPDATE usuario
                     SET nome  = :nome,
                         email = :email
                   WHERE id = :id;";
      $arrayStrings = array(":id", ":nome", ":email");
      $arrayValores = array($id, $nome, $email);
      executaComandoSql($comando, $arrayStrings, $arrayValores);
      return json_encode(array("resposta" => "sucesso"));
    } else {
      return json_encode(array("resposta" => "senha_incorreta"));
    }
  }

  function getTokenId($id)
  {
    $comando = "SELECT token FROM usuario WHERE id = :id;";
    $busca = executaComandoSql($comando, array(":id"), array($id));
    return $busca->fetch(PDO::FETCH_ASSOC)["token"];
  }

  function getTokenRedeId($id)
  {
    $comando = "SELECT tokenRede FROM usuario WHERE id = :id;";
    $busca = executaComandoSql($comando, array(":id"), array($id));
    return $busca->fetch(PDO::FETCH_ASSOC)["tokenRede"];
  }

  private function getSenhaId($id)
  {
    $comando = "SELECT senha FROM usuario WHERE id = :id;";
    $busca = executaComandoSql($comando, array(":id"), array($id));
    return $busca->fetch(PDO::FETCH_ASSOC)["senha"];
  }

  private function getSenhaEmail($email)
  {
    $comando = "SELECT senha FROM usuario WHERE email = :email;";
    $busca = executaComandoSql($comando, array(":email"), array($email));
    return $busca->fetch(PDO::FETCH_ASSOC)["senha"];
  }
}
