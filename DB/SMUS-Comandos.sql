USE SMUS;

SELECT * FROM quadroTerra;

UPDATE usuario
SET login = 'Fabio Loterio',
	email = 'vitorloterio@gmail.com',
	senha = 'schneider'
 WHERE login LIKE 'fabio';