DROP DATABASE SMUS;
# Sistema de Monitoramento da Umidade do Solo 
CREATE DATABASE SMUS; 
USE SMUS;

CREATE TABLE IF NOT EXISTS usuario (
	id int auto_increment not null,
	login varchar(40) not null unique,
	email varchar(45) not null unique,
    senha varchar(30) not null,
    primary key(id)
);

CREATE TABLE IF NOT EXISTS quadroTerra (
	id int auto_increment not null,
    nome varchar(45) not null,
    proprietario int not null,
    foreign key(proprietario) references usuario(id),
    primary key(id)
);

CREATE TABLE IF NOT EXISTS sensor (
	id int auto_increment not null,
    idQuadroTerra int,
    latitude float not null,
    longitude float not null,
    foreign key(idQuadroTerra) references quadroTerra(id),
    primary key(id)
);

CREATE TABLE IF NOT EXISTS leitura (
	dataHora timestamp not null,
    idSensor int not null,
    umidade float not null,
    primary key(dataHora, idSensor),
    foreign key(idSensor) references sensor(id)
);

INSERT INTO usuario(login,email,senha) VALUES 
('Henrique', 'Henrique@gmail.com', '123'),
('fabio','vitor@gmail.com','ashw'),
('nelso','nl@hotmail.com','abc123'); 

INSERT INTO quadroTerra(proprietario, nome) VALUES 
(1,"roça de baixo"),
(1,"divisa terreno"),
(1,"perto estrada geral");

INSERT INTO sensor(idQuadroTerra, latitude, longitude) VALUES
(1,78.542,-99.274),
(1,80.712,-85.116),
(2,87.854,-94.432),
(2,84.270,-89.844),
(3,90.010,-90.568);


INSERT INTO leitura(dataHora,idSensor,umidade) VALUES 
('2020-09-02 12:05:36',1,31.50),
('2020-09-02 12:00:36',1,30.50),
('2020-09-02 12:05:36',2,32.20),
('2020-09-02 12:10:36',5,25.40),
('2020-09-02 12:15:36',3,22.30),
('2020-09-02 12:15:36',4,28.7),
('2020-09-02 12:15:36',2,47.00),
('2020-09-02 12:10:36',1,42.80),
('2020-09-02 12:00:36',5,62.1);