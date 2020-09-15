**Fabio Vitor Loterio**, estudante do Curso Técnico em Informática Integrado ao Ensino Médio, **IFC - campus Rio do Sul**. 
 E-mail: vitorloterio@gmail.com
 
 **Henrique Borges dos Santos**, estudante do Curso Técnico em Informática Integrado ao Ensino Médio, **IFC - campus Rio do Sul**. 
 E-mail: henrique.10agr@gmail.com.

# Introdução

Este projeto está sendo desenvolvido para atender a demanda do Curso de Técnico em Informática Integrado ao Ensino Médio, ofertado pelo Câmpus Rio do Sul, UU.

O projeto foi iniciado no ano de 2019, e baseia-se na medição de níveis de umidade
Este projeto será desenvolvido com base em uma metodologia de desenvolvimento incremental, onde o 
desenvolvimento ocorre simultâneo as atividades de especificação e validação.
O sistema é dividido em versões, de forma que cada incremento adiciona funcionalidades à versão anterior.
Vale ressaltar que, por mais que a metodologia escolhida seja a incremental, as demais(cascata e reúso) 
também podem estar normalmente mescladas a estas.

O processo segue o seguinte fluxo:

                              Especificação  <---> versão inicial   
                                    ↑↓            
    Descrição do esboço ---> Desenvolvimento <---> versões intermediárias
                                    ↑↓
                                Validação    ----> versão final

Onde entra em loop nas etapas de **Desenvolvimento**, **Especificação** e **Validação**, sendo a principal parte 
a do desenvolvimento. 


## Principais Atividades do Desenvolvimento

- **Especificação:** define as funcionalidades e as restrições de
funcionamento do software.

- **Desenvolvimento:** o software deve ser implementado para
atender às especificações.

- **Validação:** o software deve ser validado para garantir que atenda
às demandas do cliente.

- **Evolução:** o software deve evoluir para atender às mudanças do
cliente.

## Sobre o Software

O projeto conta com a junção de serviços via **Arduino**, **Web Service** e uma **Aplicação Móvel**, ondes estes interagem entre si, fornecendo informações, enviando e recebendo comandos.
Cada serviço assume uma função bastante específica, dependendo uns dos outros para atingirem o resultado final esperado: Uma rede de sensores que meçam a umidade do solo, enviem estes dados a um servidor web(Web Service) e a aplicação móvel possa consultá-los, para aí então disponibilizá-los através de uma interface para o agricultor.

## Commits

Os **commits** do projeto seguem um padrão determinado pelos desenvolvedores. Isso tem o intuito de causar uma melhor localização e organização do projeto.
Basicamente, as mensagens devem ser elaboradas em 3 partes. A primeira deve referênciar qual a funcionalidade que está sendo desenvolvida em questão. A segunda deve especificar em qual estágio está esse desenvolvimento. A terceira e última parte da mensagem é opcional e deve conter detalhes sobre o desenvolvimento.
Os commits devem ser feitos de maneira mais sucinta possível e para isso foram denominados alguns padrões. 
São eles:

|  Prefixo  |   Descrição   |
|:---------:|  :---------:  |  
|   **d**   |desenvolvimento|               
|   **ok**  |  finalizado   |
<!-- |   ****   |  |
|   ****   |  |
|   ****   |  | -->

Deverão, sob as normas acima, ser montados sempre da seguinte forma:

> **{funcionalidade} - {estado do desenvolvimento} (observação)**



