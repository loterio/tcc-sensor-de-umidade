<?php
  require_once('../Conexao.php');
  $pdo = new PDO(MYSQL, USER, PASSWORD);

  $res = $pdo->prepare("SELECT *
                              FROM cliente");
  $res->execute();
  $resultado = $res->fetch(PDO::FETCH_ASSOC);
  $resultado = json_encode($resultado);
  echo $resultado;
?>
