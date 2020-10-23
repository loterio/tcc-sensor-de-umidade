<?php
header("Access-Control-Allow-Origin: *");
header("Content-Type: application/json; charset=UTF-8");

require_once('Funcoes.php');
require_once('metodos.php');
require_once('classes/classes.php');

DEFINE('URL', $_GET['url']);
DEFINE('METODO', $_SERVER["REQUEST_METHOD"]);

$usuario = new Usuario;
$resposta = array("resposta" => "nao_autorizado");
$resposta = json_encode($resposta);

if (isset($_SERVER['HTTP_TOKEN'])) {
  DEFINE('TOKEN', $_SERVER['HTTP_TOKEN']);
  DEFINE('ID_USUARIO', extraiIdUsuarioToken(TOKEN));
  DEFINE('VALIDADE_TOKEN', validaToken(TOKEN, $usuario));
}

if (isset($_SERVER['HTTP_TOKENREDE'])) {
  DEFINE('TOKENREDE', $_SERVER['HTTP_TOKENREDE']);
  DEFINE('ID_USUARIO', extraiIdUsuarioToken(TOKENREDE));
  DEFINE('VALIDADE_TOKENREDE', validaTokenRede(TOKENREDE, $usuario));
}

if (isset($_SERVER['HTTP_TOKEN']) and VALIDADE_TOKEN) {
  $quadroTerra = new QuadroTerra(ID_USUARIO);
  switch (METODO) {
    case 'GET':
      switch (URL) {
        case 'usuario/':
          $resposta = $usuario->dadosUsuario(ID_USUARIO);
          break;
        case 'quadroTerra/':
          $resposta = isset($_GET['id'])
            ? $quadroTerra->dadosQuadroDeTerra($_GET['id'])
            : $quadroTerra->listaQuadrosTerra();
          break;
        case 'sensor/':
          $sensor = new Sensor(ID_USUARIO);
          $resposta = $sensor->listaSensores();
          break;
        default:
          # code...
          break;
      }
      break;
    case 'POST':
      switch (URL) {
        case 'quadroTerra/':
          $nome = $_POST['nome'];
          $coordenadas = $_POST['coordenadas'];
          $umidadeIdeal = $_POST['umidadeIdeal'];
          $imagem = $_POST['imagem'];
          $resposta = $quadroTerra->criaQuadroTerra($nome, $coordenadas, $umidadeIdeal, $imagem);
          break;
        case 'leitura/':
          $sensores = $_POST['sensores'];
          $sensores = json_decode($sensores);
          $leitura = new Leitura(ID_USUARIO);
          $resposta = $leitura->listaLeituras($sensores);
          break;
      }
      break;
    case 'PUT':
      switch (URL) {
        case 'usuario/':
          $nome = $_PUT['nome'];
          $email = $_PUT['email'];
          $senha = $_PUT['senha'];
          $resposta = $usuario->alteraDadosUsuario(ID_USUARIO, $nome, $email, $senha);
          break;

        case 'quadroTerra/':
          $idQuadroTerra = $_GET['id'];
          $nome = $_PUT['nome'];
          $coordenadas = $_PUT['coordenadas'];
          $umidadeIdeal = $_PUT['umidadeIdeal'];
          $imagem = $_PUT['imagem'];
          $resposta = $quadroTerra->alteraQuadroTerra($idQuadroTerra, $nome, $coordenadas, $umidadeIdeal, $imagem);
          break;
        default:
          # code...
          break;
      }
      break;
    case 'DELETE':
      switch (URL) {
        case 'quadroTerra/':
          $id = $_GET['id'];
          $resposta = $quadroTerra->excluiQuadroTerra($id);
          break;
      }
      break;
  }
} else if (isset($_SERVER['HTTP_TOKENREDE']) and VALIDADE_TOKENREDE) {
  switch (URL) {
    case 'sensor/':
      $sensor = new Sensor(ID_USUARIO);
      $leitura = new Leitura(ID_USUARIO); 
      $dadosSensor = $_POST['dadosSensor'];
      $dadosSensor = json_decode($dadosSensor);

      $longitude = $dadosSensor->lon;
      $latitude = $dadosSensor->lat;
      $umidade = $dadosSensor->umi;

      $resposta = $sensor->criaSensor($longitude, $latitude);
      $idSensor = json_decode($resposta)->idSensor;
      $leitura->criaLeitura($idSensor, $umidade);
      break;
    case 'leitura/':
      $sensor = new Sensor(ID_USUARIO);
      $leitura = new Leitura(ID_USUARIO);
      $leituras = json_decode($_POST['leitura']);
      foreach ($leituras as $valor) {
        $idSensor = $valor->id;
        $longitude = $valor->lon;
        $latitude = $valor->lat;
        $umidade = $valor->umi;
        $sensor->atualizaSensor($idSensor, $longitude, $latitude);
        $leitura->criaLeitura($idSensor, $umidade);
      }
      $resposta = json_encode(array("resposta" => "sucesso"));
      break;
  }
} else {
  switch (URL) {
    case 'entrar/':
      $email = $_POST['email'];
      $senha = $_POST['senha'];
      $resposta = $usuario->entrar($email, $senha);
      break;
    case 'cadastrar/':
      $nome = $_POST['nome'];
      $email = $_POST['email'];
      $senha = $_POST['senha'];
      $resposta = $usuario->cadastrarUsuario($nome, $email, $senha);
      break;
    default:
      $usuario->sair(ID_USUARIO);
      break;
  }
}

echo $resposta;
