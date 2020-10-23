<?php
class QuadroTerra
{

  private $idUsuario;

  public function __construct($idUsuario)
  {
    $this->idUsuario = $idUsuario;
  }

  function criaQuadroTerra($nome, $coordenadas, $umidadeIdeal, $imagem = "")
  {
    $coordenadas = $this->retornaArrayCoordenadas($coordenadas);
    $arrayStrings = array(":nome");
    $arrayValores = array($nome);

    $comando = "SELECT id FROM quadroTerra WHERE nome = :nome";
    $busca = executaComandoSql($comando, $arrayStrings, $arrayValores);

    if (retornaJsonSelect($busca) == '[]') {
      $comando = "INSERT INTO quadroTerra(proprietario, nome, imagem, umidadeIdeal) 
                  VALUES (:proprietario, :nome, :imagem, :umidadeIdeal);";
      $arrayStrings = array(":proprietario", ":nome", ":imagem", ":umidadeIdeal");
      $arrayValores = array($this->idUsuario, $nome, $imagem, $umidadeIdeal);
      executaComandoSql($comando, $arrayStrings, $arrayValores);

      $arrayStrings = array(":nome");
      $arrayValores = array($nome);
      $comando = "SELECT id FROM quadroTerra WHERE nome = :nome";
      $busca = executaComandoSql($comando, $arrayStrings, $arrayValores);
      $quadroTerra = retornaArraySelect($busca);
      $idQuadroTerra = $quadroTerra[0]['id'];
      $this->salvaCoordenadas($idQuadroTerra, $coordenadas);

      return json_encode(
        array("resposta" => "sucesso")
      );
    } else {
      return json_encode(
        array("resposta" => "quadro_existente")
      );
    }
  }

  function listaQuadrosTerra()
  {
    $comando = "SELECT id, nome, imagem 
                  FROM quadroTerra 
                  WHERE proprietario = :idUsuario;";
    $arrayStrings = array(":idUsuario");
    $arrayValores = array($this->idUsuario);
    $busca = executaComandoSql($comando, $arrayStrings, $arrayValores);
    return retornaJsonSelect($busca);
  }

  function dadosQuadroDeTerra($idQuadroTerra)
  {
    $comando = "SELECT nome, imagem, umidadeIdeal
                  FROM quadroTerra 
                  WHERE proprietario = :idUsuario
                    AND id = :id;";
    $arrayStrings = array(":idUsuario", ":id");
    $arrayValores = array($this->idUsuario, $idQuadroTerra);
    $busca = executaComandoSql($comando, $arrayStrings, $arrayValores);
    $quadroTerra = retornaArraySelect($busca);
    $quadroTerra[0]['coordenadas'] =
      $this->pegaCoordenadas($idQuadroTerra);
    return json_encode($quadroTerra);
  }

  function alteraQuadroTerra($idQuadroTerra, $nome, $coordenadas, $umidadeIdeal, $imagem = "")
  {
    $comando = "SELECT id, nome FROM quadroTerra WHERE nome = :nome";
    $busca = executaComandoSql($comando, array(":nome"), array($nome));
    $quadroTerra = retornaArraySelect($busca);
    $nomeValido = false;

    if ($quadroTerra == [])
      $nomeValido = true;
    else if ($quadroTerra[0]['id'] == $idQuadroTerra)
      $nomeValido = true;

    if ($nomeValido) {
      $coordenadas = $this->retornaArrayCoordenadas($coordenadas);
      $arrayStrings = array(":nome", ":imagem", ":umidadeIdeal", ":id");
      $arrayValores = array($nome, $imagem, $umidadeIdeal, $idQuadroTerra);
      $comando = "UPDATE quadroTerra SET nome = :nome, umidadeIdeal = :umidadeIdeal, imagem = :imagem WHERE id = :id";
      executaComandoSql($comando, $arrayStrings, $arrayValores);
      $this->deletaCoordenadas($idQuadroTerra);
      $this->salvaCoordenadas($idQuadroTerra, $coordenadas);
      return json_encode(array("resposta" => "sucesso"));
    } else {
      return json_encode(array("resposta" => "nom_existente"));
    }
  }

  function excluiQuadroTerra($idQuadroTerra)
  {
    $this->deletaCoordenadas($idQuadroTerra);
    $comando = "DELETE
                  FROM quadroTerra 
                  WHERE proprietario = :idUsuario
                    AND id = :id;";
    $arrayStrings = array(":idUsuario", ":id");
    $arrayValores = array($this->idUsuario, $idQuadroTerra);
    executaComandoSql($comando, $arrayStrings, $arrayValores);
    return json_encode(array("resposta" => "sucesso"));
  }

  private function salvaCoordenadas($idQuadroTerra, $coordenadas)
  {
    $comando = "INSERT INTO coordsQuadroTerra(idQuadroTerra, idPonto, longitude, latitude) 
                  VALUES (:idQuadroTerra, :idPonto, :longitude, :latitude);";
    $arrayStrings = array(":idQuadroTerra", ":idPonto", ":longitude", ":latitude");

    $idPonto = 1;
    for ($i = 0; $i < count($coordenadas); $i += 2) {
      $y = $i + 1;
      $arrayValores = array($idQuadroTerra, $idPonto, $coordenadas[$i], $coordenadas[$y]);
      $idPonto++;
      executaComandoSql($comando, $arrayStrings, $arrayValores);
    }
  }

  private function pegaCoordenadas($idQuadroTerra)
  {
    $comando = "SELECT longitude,latitude 
                  FROM coordsQuadroTerra 
                  WHERE idQuadroTerra = :idQuadroTerra;";
    $arrayStrings = array("idQuadroTerra");
    $arrayValores = array($idQuadroTerra);
    $busca = executaComandoSql($comando, $arrayStrings, $arrayValores);
    return retornaArraySelect($busca);
  }

  private function deletaCoordenadas($idQuadroTerra)
  {
    $comando = "DELETE 
                  FROM coordsQuadroTerra 
                  WHERE idQuadroTerra = :idQuadroTerra;";
    $arrayStrings = array("idQuadroTerra");
    $arrayValores = array($idQuadroTerra);
    executaComandoSql($comando, $arrayStrings, $arrayValores);
  }

  private function retornaArrayCoordenadas($coordenadas)
  {
    $coordenadas = json_decode($coordenadas);
    return $coordenadas;
  }
}
