DROP DATABASE SMUS;
# Sistema de Monitoramento da Umidade do Solo 
CREATE DATABASE SMUS; 
USE SMUS;

CREATE TABLE IF NOT EXISTS usuario (
	id int auto_increment not null,
	nome varchar(40) not null,
	email varchar(45) not null unique,
    senha varchar(255) not null,
    token varchar(255),
    tokenRede varchar(255),
    primary key(id)
);

CREATE TABLE quadroTerra (
	id int auto_increment not null,
    nome varchar(45) not null,
    imagem varchar(100),
    proprietario int not null,
    umidadeIdeal float not null,
    foreign key(proprietario) references usuario(id),
    primary key(id)
);

CREATE TABLE coordsQuadroTerra (
	idQuadroTerra int not null,
    idPonto int not null,
    longitude real not null,
    latitude real not null,
    foreign key(idQuadroTerra) references quadroTerra(id)
);


CREATE TABLE sensor (
	idSensor int auto_increment not null,
    idProprietario int,
    longitude real not null,
    latitude real not null,
    foreign key(idProprietario) references usuario(id),
    primary key(idSensor)
);

CREATE TABLE leitura (
	dataHora TIMESTAMP NULL DEFAULT CURRENT_TIMESTAMP,
    idSensor int not null,
    umidade float not null,
    foreign key(idSensor) references sensor(idSensor)
);

INSERT INTO usuario(nome, email, senha, token, tokenRede) VALUES 
/*123*/('Henrique', 'Henrique@gmail.com', '$2y$10$MaRu4gj07b0dzNtljJnJaeMGgfB7CZ1ESPl2DgzQ/kdY5Hy4obwSC', 'eyJhbGciOiAiSFMyNTYiLCAidHlwIjogIkpXVCJ9.eyJzdWIiOiAiMSIsICJpYXQiOiAxNjAzMTU0NzE1fQ.bFQgTqHwWYxqUVaNsPfQwvcIKL4xrm2ywHDdu7HVNmg', 'eyJhbGciOiAiSFMyNTYiLCAidHlwIjogIkpXVCJ9.eyJzdWIiOiAiMSIsICJpYXQiOiAxNjAzMTM0MTIxfQ.iW0ZSAoywXdmGGJLciVCBNoCUn9esgZxpWeJSY2h8F4'),#123
/*abc*/('fabio','vitor@gmail.com','$2y$10$Hhs7O8gsP4oKn89qewvKzuhzv5XeDT2GNLcph4yY1RUatQTrtT1Le', null, 'eyJhbGciOiAiSFMyNTYiLCAidHlwIjogIkpXVCJ9.eyJzdWIiOiAiMSIsICJpYXQiOiAxNjAzMTM0MTIxfQ.iW0ZSAoywXdmGGJLciVCBNoCUn9esgZxpWeJSY2h8F4'),#abc
/*adm*/('nelso','nl@hotmail.com','$2y$10$Qpc.yFh7c5xOSzY2ETTkzekg.gVNlMHfrxM/JnFm8xiEpW08xRNmy', null, 'eyJhbGciOiAiSFMyNTYiLCAidHlwIjogIkpXVCJ9.eyJzdWIiOiAiMSIsICJpYXQiOiAxNjAzMTM0MTIxfQ.iW0ZSAoywXdmGGJLciVCBNoCUn9esgZxpWeJSY2h8F4');#adm

INSERT INTO quadroTerra(proprietario, nome, imagem, umidadeIdeal) VALUES 
(1,"roça de baixo", '../DB/imgQuadroTerra/rocaDeBaixo.jpg', 30.5),
(1,"divisa terreno", '../DB/imgQuadroTerra/divisaTerreno.jpg', 30.5),
(2,"riozinho", '../DB/imgQuadroTerra/riozinho.jpg', 30.5),
(3,"pinheirinho", '../DB/imgQuadroTerra/pinheirinho.jpg', 30.5),
(1,"roça de cima", '../DB/imgQuadroTerra/rocaDecima.jpg', 30.5),
(2,"herança", '../DB/imgQuadroTerra/heranca.jpg', 30.5),
(1,"perto estrada geral",'../DB/imgQuadroTerra/pertoEstradaGeral.jpg', 30.5);

INSERT INTO sensor(idProprietario, longitude, latitude) VALUES
(1,-49.681380326240884,-27.352004134578173),
(2,-49.68157263119886,-27.352093145793468),
(1,-49.68182452360759,-27.3521893740538),
(1,-49.681789312841005,-27.3523433390979),
(1,-49.68156721415767,-27.352242299562093),
(2,-49.68133428139197,-27.352162911291472),
(1,-49.681309810737986,-27.352351641316858),
(2,-49.68151565829851,-27.352414189549286);

INSERT INTO leitura(idSensor,umidade) VALUES 
(1,31.50),
(1,30.50),
(2,32.20),
(5,25.40),
(3,22.30),
(4,28.7),
(2,47.00),
(1,42.80),
(7,25.40),
(5,62.1);


