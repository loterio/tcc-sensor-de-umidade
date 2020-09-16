USE SMUS;

SELECT id as "# Quadro", nome, proprietario, idSensor, latitude, longitude 
FROM quadroTerra
	INNER JOIN sensor
WHERE proprietario = 1
AND id = idQuadroTerra;

UPDATE usuario
SET login = 'Fabio Loterio',
	email = 'vitorloterio@gmail.com',
	senha = 'schneider'
 WHERE login LIKE 'fabio';