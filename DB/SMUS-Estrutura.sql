DROP DATABASE SMUS;
# Sistema de Monitoramento da Umidade do Solo 
CREATE DATABASE SMUS; 
USE SMUS;

CREATE TABLE sensor (
	id int auto_increment not null,
    umidade float not null,
    latitude float not null,
    longitude float not null,
    primary key(id)
);

CREATE TABLE quadroTerra (
	id int auto_increment not null,
    nome varchar(45) not null,
    primary key(id)
);

CREATE TABLE leitura (
	dataHora timestamp not null,
    idSensor int not null,
    idQuadroTerra int not null,
    umidade float not null,
    primary key(dataHora, idSensor, idQuadroTerra),
    foreign key(idSensor) references sensor(id),
    foreign key(idQuadroTerra) references quadroTerra(id)
);

CREATE TABLE cliente (
	login varchar(40) not null unique,
	email varchar(45) not null unique,
    senha varchar(30) not null,
    primary key(login)
);

INSERT INTO sensor(umidade, latitude, longitude) VALUES
(24.5,78.542,-99.274),
(25.5,80.712,-85.116),
(23.5,87.854,-94.432),
(27.5,84.270,-89.844),
(26.5,90.010,-90.568);

INSERT INTO quadroTerra(nome) VALUES 
("roça de baixo"),
("divisa terreno"),
("perto estrada geral");

INSERT INTO cliente(login,email,senha) VALUES 
('Henrique', 'Henrique@gmail.com', '123'),
('fabio','vitor@gmail.com','ashw'),
('nelso','nl@hotmail.com','abc123'); 

INSERT INTO leitura(dataHora,idSensor,idQuadroTerra,umidade) VALUES 
('2020-09-02 12:05:36',1,1,31.50),
('2020-09-02 12:00:36',1,1,30.50),
('2020-09-02 12:05:36',2,1,32.20),
('2020-09-02 12:10:36',5,1,25.40),
('2020-09-02 12:15:36',3,1,22.30),
('2020-09-02 12:15:36',4,1,28.7),
('2020-09-02 12:15:36',2,2,47.00),
('2020-09-02 12:10:36',1,2,42.80),
('2020-09-02 12:00:36',5,2,62.1);
