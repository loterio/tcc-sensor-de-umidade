<?php
  require_once('Conexao.php');

  function executaComandoSql($comando, $arrayStrings, $arrayValores) {
    $pdo             = new PDO(MYSQL, USER, PASSWORD);
    $busca           = $pdo->prepare($comando);
    $numeroDeValores = count($arrayStrings);

    for ($i=0; $i < $numeroDeValores; $i++) { 
      $busca->bindparam($arrayStrings[$i], $arrayValores[$i]);
    }

    $busca->execute();
    return $busca;
  }

  function retornaJsonSelect($busca) {
    $resultado = array();
    while($linha = $busca->fetch(PDO::FETCH_ASSOC)){
      array_push($resultado, $linha); 
    }
    return json_encode($resultado);
  }

  function retornaArraySelect($busca) {
    $resultado = array();
    while($linha = $busca->fetch(PDO::FETCH_ASSOC)){
      array_push($resultado, $linha); 
    }
    return $resultado;
  }
