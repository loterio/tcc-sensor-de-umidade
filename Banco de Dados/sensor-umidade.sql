CREATE DATABASE sensorUmidade;

USE sensorUmidade;

CREATE TABLE sensor (
	id int auto_increment not null,
    # umidade float not null,
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
	dataHora date not null,
    idSensor int not null,
    idQuadroTerra int not null,
    umidade float not null,
    primary key(dataHora, idSensor, idQuadroTerra),
    foreign key(idSensor) references sensor(id),
    foreign key(idQuadroTerra) references quadroTerra(id)
);

CREATE TABLE cliente (
	login varchar(40) not null,
	email varchar(45) not null,
    senha varchar(30) not null,
    primary key(login)
);

INSERT INTO sensor(latitude, longitude) VALUES
(78.542,-99.274),
(80.712,-85.116),
(87.854,-94.432),
(84.270,-89.844),
(90.010,-90.568); 

INSERT INTO leitura(dataHora,idSensor,idQuadroTerra,umidade) VALUES 
('28/08/2020',1,1,30.50),
('28/08/2020',2,1,32.20),
('28/08/2020',5,1,25.40),
('28/08/2020',3,1,22.30),
('28/08/2020',4,1,28.7),
('28/08/2020',2,2,47.00),
('28/08/2020',1,2,42.80),
('27/08/2020',5,2,62.1),
('28/08/2020',4,2,53.6),
('28/08/2020',3,2,45.0);

INSERT INTO quadroTerra(nome) VALUES 
("roça de baixo"),
("divisa terreno"),
("perto estrada geral");
