USE SMUS;

SELECT * FROM cliente;

UPDATE cliente
SET login = 'Fabio Loterio',
	email = 'vitorloterio@gmail.com',
	senha = 'schneider'
 WHERE login LIKE 'fabio';