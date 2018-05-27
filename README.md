# Projeto Switch
# Author: Arthur S. Couto
# Matricula: 160002575

# Uso do Artefato

## Compilação
```
compilador: Apple LLVM version 9.0.0 (clang-900.0.39.2)
no terminal, navegue até a pasta onde está localizado o source code e digite (gcc switch.c)
```
## Execução
```
na pasta onde está presente o executável, digite no terminal (./a.out)
```

# Ideia do Problema

## Proposta Para o Projeto
		Para este projeto, deve ser implementado um sistema que simula o funcionamento de um switch de 
	enlace, onde maquinas diferentes se comunicam por meio de um unico canal, porém com segmentos 
	diferentes para cada maquina, havendo necessidade de controlar interferencias entre mensagens.
		Além disso, é necessário a possibilidade de processar mensagens broadcast de uma maquina para 
	as outras, para não saturar o enlace com pacotes redundantes e salvar desempenho.

## O Que Efetivamente Foi Feito
	Foi implementado neste projeto um programa multi-thread que permite threads se comunicarem e mandarem 
	dados uma para as outras utilizando os modelos single-cast e broad-cast. O projeto permite que threads se
	comuniquem entre si sempre que as mesmas não estejam enviando nem recebendo uma mensagem nesse exato
	momento. Dessa forma, ele multiplexa e demultiplexa mensagens entre elas de modo a não desperdiçar recursos que poderiam estar sendo usados para mandar mais mensagens.
		Já que o conteudo das mensagens e a semantica por tras delas não é relevante para o projeto, já que esse só se foca na concorrencia, as threads geram mensagens sem semantica nem contexto e enviam para outras threads quaisquer apenas por motivos de testabilidade.

# Contexto Teórico

## Semelhança a Problemas Classicos
	O problema em questão é extremamente analogo ao problema do jantar dos filosofos, onde um determinado filosofo utiliza dois garfos para se alimentar, e deve ser garantido que apenas um filosofo utilize este garfo de cada vez.
		Por lidar com competição de recursos iguais de forma que não haja desperdicio (um filosofo pode comer enquanto outro come, desde que não use os mesmos garfos) esse problema pode servir de base para a solução do switch também.

## Analogia
	Pense que um filosofo usando dois garfos está fazendo uma comunicação entre eles. substituindo os garfos por maquinas, e o filosofo por transferencia de pacotes, nosso problema poderia ser traduzido a uma mesa com N garfos e M filosofos, onde para cada par de garfos existe um filosofo, e dois filosofos não podem usar um só garfo ao mesmo tempo.

# Contexto de Implementação

## Base Para Implementação
	Analogamente as soluções do jantar dos filosofos discutidas em sala, uma possibilidade de implementação é garantir que as operações coletem todos os recursos necessarios para seu funcionamento de forma transacional (ou pega todos os recursos ou não pega nenhum). Isso foi feito usando uma serie de locks e variaveis de condição (uma para cada máquina virtual pra ser mais especifico).

## Descrição das Variaveis de Concorrencia
	O programa usa um lock global para garantir que não haja problemas na alocação de recurssos concorrentes caso várias threads tentem alocar ao mesmo tempo. Existe um vetor de locks com um lock para cada maquina para garantir que uma mesma maquina (semelhante ao garfo) não seja utilizada ao mesmo tempo para comunicações diferentes (interferencia nas mensagens, erro de enlace). Há também um vetor de condições para permitir que uma mensagem espere a liberação de uma maquina para que possa ser transmitida.

# Funcionamento Concorrente

## Single-Cast
	Para fazer uma transferencia single cast, o sistema aloca a porta de enlace das duas threads envolvidas enquanto ainda tem posse do lock global, após isso usa as portas para mandar a mensagem e depois as libera.
		Dessa forma uma mensagem single cast não vai gerar interferencia e não usa mais recursos do que o necessário.

## Broad-Cast
	A mensagem broadcast aloca primeiro a porta da maquina de envio, e após a mensagem ser enviada, libera essa porta e aloca outras portas individualmente a medida que estiverem livres. Dessa forma, o sistema pode receber mensagens enquanto a mensagem broadcast ainda não se espalhou e também não é necessário que todas as portas estejam livres ao mesmo tempo para dissipar a mensagem.
		Por motivos de desempenho, cada maquina recebe o broadcast a partir de uma nova thread, o que permite que não haja ordem de envio desses pacotes, e permite que eles sejam enviados assim que os recursos estiverem disponiveis.

# Observações

## Analise Estática
	Não foi detectado nenhuma possibilidade de falta de exclusão mutua ou deadlock no código fonte do projeto após uma análise metódica porém não cientifica por parte do autor, que no contexto do projeto foi o melhor que pode ser feito.

## Funcionamento do Executável
	Após duas horas de mensagens sendo trocadas entre as threads, não houve nenhuma ocasião em que as mensagens de descrição de comportamento (colocadas no programa para debug) foram incondizentes com o comportamento esperado pelo programa de permitir comunicação sem interferencias.

# Conclusões

## Corretude
	Devido aos testes e analizes, a corretude do programa foi verificada da melhor forma possivel com os recursos disponiveis. Para melhorar a analise de corretude, seria necessário utilizar metodos de "peer-reviewing", aumentar a quantidade de testes ou então (o mais ideal) produzir uma prova formal de que ele está de fato correto, todas as opções não foram condizentes devido a natureza individual e de curto prazo do projeto.

## Portabilidade
	É plausivel o reuso desse código para coordenar threads em projetos futuros, já que esse se mostrou capaz até certo nivel de garantir comunicações sem interferencia entre threads que so tenham um ponto de acesso, apesar de o seu funcionamento ser demasiadamente básico, ainda é adequado a sistemas pequenos e simples.# Switch
