USE SMUS;

INSERT INTO quadroTerra(proprietario, nome) 
                  VALUES (3,'paiol');

select * from quadroTerra;
SELECT id as "# Quadro", nome, proprietario, idSensor, latitude, longitude 
FROM quadroTerra
	INNER JOIN sensor
WHERE proprietario = 1
AND id = idQuadroTerra;

UPDATE usuario
SET login = 'Fabio Loterio',
	email = 'vitorloterio@gmail.com',
	senha = 'schneider'
 WHERE login LIKE ;