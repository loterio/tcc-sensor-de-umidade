<?php

//===================Classes==================
require_once('Conexao.php');
require_once('hash.php');

function executaComandoSql($comando, $arrayStrings, $arrayValores)
{
  $pdo             = new PDO(MYSQL, USER, PASSWORD);
  $busca           = $pdo->prepare($comando);
  $numeroDeValores = count($arrayStrings);

  for ($i = 0; $i < $numeroDeValores; $i++) {
    $busca->bindparam($arrayStrings[$i], $arrayValores[$i]);
  }

  $busca->execute();
  return $busca;
}

function retornaJsonSelect($busca)
{
  $resultado = array();
  while ($linha = $busca->fetch(PDO::FETCH_ASSOC)) {
    array_push($resultado, $linha);
  }
  return json_encode($resultado);
}

function retornaArraySelect($busca)
{
  $resultado = array();
  while ($linha = $busca->fetch(PDO::FETCH_ASSOC)) {
    array_push($resultado, $linha);
  }
  return $resultado;
}

//===================Token JWT==================

function base64ErlEncode($dado)
{
  return str_replace(['+', '/', '='], ['-', '_', ''], base64_encode($dado));
}

function geraJWT($id, $senha)
{
  $header = base64ErlEncode('{"alg": "HS256", "typ": "JWT"}');
  $payload = base64ErlEncode('{"sub": "' . $id . '", "iat": ' . time() . '}');
  $signature = base64ErlEncode(hash_hmac('sha256', $header . '.' . $payload, $senha, true));
  return $header . '.' . $payload . '.' . $signature;
}

function geraToken($id){
  return geraJWT($id, SENHA_TOKEN);
}
function geraTokenRede($id){
  return geraJWT($id, SENHA_TOKEN_REDE);
}

function validaToken($token, $usuario)
{
  DEFINE('TEMPOVALIDO', 120000000);
  $payload = extraiPayload($token);
  $idUsuario = $payload->sub;
  $momentoCriacaoToken = $payload->iat;

  if ((time() - $momentoCriacaoToken) <= TEMPOVALIDO) {
    $tokenUsuario = $usuario->getTokenId($idUsuario);
    if ($token == $tokenUsuario)
      return true;
    else
      return false;
  } else {
    return false;
  }
}

function validaTokenRede($token, $usuario)
{
  $payload = extraiPayload($token);
  $idUsuario = $payload->sub;
  $tokenUsuario = $usuario->getTokenRedeId($idUsuario);
  if ($token == $tokenUsuario)
    return true;
  else
    return false;
}

function extraiPayload($token)
{
  $partes = explode('.', $token);
  $payload = json_decode(
    base64_decode($partes[1])
  );
  return $payload;
}

function extraiIdUsuarioToken($token)
{
  $partes = explode('.', $token);
  $payload = json_decode(
    base64_decode($partes[1])
  );
  return $payload->sub;
}
