USE SMUS;

select * from quadroTerra;
SELECT id as "# Quadro", nome, proprietario, idSensor, latitude, longitude 
FROM quadroTerra
	INNER JOIN sensor
WHERE proprietario = 1
AND id = idQuadroTerra;

SELECT idSensor, latitude, longitude
                    FROM sensor
                   WHERE idQuadroTerra IS NULL
                     AND idProprietario = 1;
