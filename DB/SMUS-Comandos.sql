USE SMUS;

SELECT * FROM usuario;

UPDATE usuario
SET login = 'Fabio Loterio',
	email = 'vitorloterio@gmail.com',
	senha = 'schneider'
 WHERE login LIKE 'fabio';