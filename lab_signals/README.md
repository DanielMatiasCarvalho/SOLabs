# Guião sobre _signals_

![IST](img/IST_DEI.png)  

## Objetivos

No final deste guião, deverá ser capaz de:

- Explicar o que é um _signal_ e como se usa para controlar processos do sistema operativo;
- Programar o tratamento de sinais assíncronos.

## Introdução

Um sinal (_signal_) é uma mensagem **assíncrona** que pode ser enviada a qualquer momento para um processo ativo.
O sistema operativo interrompe o fluxo normal de execução do processo para este tratar o sinal.
O objetivo do envio do _signal_ é provocar um comportamento específico, como a sua terminação, por exemplo.
O tratamento de um sinal está predefinido, mas pode ser criada uma rotina para tratar este sinal como vamos ver a seguir.

Qualquer chamada a uma função durante a rotina de tratamento do sinal deverá ser segura ([_Async-Signal-Safe_](https://man7.org/linux/man-pages/man7/signal-safety.7.html) ou abreviadamente [_AS-Safe_](https://man7.org/linux/man-pages/man7/signal-safety.7.html)).  
Por exemplo, a função [`printf`](https://man7.org/linux/man-pages/man3/printf.3.html) mantém dados alocados, como ponteiros e indíces para efetuar _buffered I/O_.
Se, durante a execução do programa, uma chamada à função [`printf`](https://man7.org/linux/man-pages/man3/printf.3.html) for interrompida por um sinal, uma nova chamada à função [`printf`](https://man7.org/linux/man-pages/man3/printf.3.html) durante a rotina de tratamento do _signal_ resultará em comportamento indefinido, ou seja, poderá criar situações de erro inesperadas.

### 1. Comando `kill`

O envio de sinais pode ser feito com o comando [`kill`](https://man7.org/linux/man-pages/man1/kill.1.html) que, por omissão, envia um sinal para pedir ao processo para terminar (daí o nome do comando).
O nome do comando poderia ser algo como _send signal_, dado que permite enviar qualquer sinal, mas o nome original permanece.
O envio de sinais é apenas permitido ao utilizador dono do processo ou ao super-utilizador.

O envio de sinais pode ser feito com o comando [`kill`](https://man7.org/linux/man-pages/man1/kill.1.html) das seguintes formas:

```sh
$ kill -s TERM 1234
$ kill -TERM 1234
```

`TERM` corresponde ao sinal que pretendemos enviar.  
Pode consultar a lista de possíveis sinais com o comando: `kill -L`.
A lista vai variar entre diferentes versões de Unix.

`1234` corresponde ao identificador do processo (_Process ID_ ou apenas _PID_) a que pretendemos enviar o sinal.  
Pode consultar a lista de processos ativos com o comando: [`ps`](https://man7.org/linux/man-pages/man1/ps.1p.html) (ou [`top`](https://man7.org/linux/man-pages/man1/top.1.html) ou [`pgrep`](https://man7.org/linux/man-pages/man1/pgrep.1.html)).

Pergunta: Qual é o sinal enviado por omissão com o comando `kill 1234`?  
(Sugestão: procure a resposta a página no manual, com o comando: `man kill`)

Caso um programa não responda ao sinal de terminação ordeira (`SIGTERM`), pode ser usado o sinal para terminação forçada (`SIGKILL`), normalmente associado ao comando `kill -9`.

### 2. Exemplo `intquit.c`

1. Clone este repositório, usando o git: `git clone https://github.com/tecnico-so/lab_signals.git`
2. Estude o programa `intquit.c`:
    - Repare na definição da função `sig_handler()`, que será chamada assincronamente para tratar o sinal, quando o processo o receber;
    - Repare nos tratamentos do `SIGINT` (linha 16) e do `SIGQUIT` (linha 27);
    - A função [`signal()`](https://man7.org/linux/man-pages/man2/signal.2.html) regista a rotina de tratamento para estes sinais (linhas 37 e 39).
    Neste caso, a mesma rotina `sig_handler()` (nome escolhido pelo programador), vai ser registada para tratar dois sinais diferentes (`SIGINT` e `SIGQUIT`), mas poderiam ser configuradas rotinas diferentes;
3. Compile o programa com a ferramenta `make` (dentro da pasta `src/`);
4. Corra o programa (`./intquit`);
5. Observe que o programa inicia e fica num ciclo infinito ou parado.
6. Experimente fazer `CTRL-C` (que envia `SIGINT`) e observe o resultado.
7. Repita o passo anterior.
8. Experimente fazer `CTRL-\` (que envia `SIGQUIT`) e observe o resultado.

### 3. Exercício SIGTERM

1. Adicione o tratamento do sinal `SIGTERM` numa nova função chamada `term_handler()`.
2. Compile e teste.

Notas: Não é possível enviar o sinal `SIGTERM` com um atalho como outros sinais. Para testar o código deveram:
1. Parar o processo com o sinal `SIGSTOP` através do atalho `CTRL-Z`.
2. Inspecionar o `pid` do vosso processo com o comando `ps`.
3. Envir o sinal `SIGTERM` com o comando `kill pid`.
4. Regressar ao vosso processo com o comando `fg`.

----

## Conclusão

Neste guião vimos o que são _signals_ e como são úteis para envio de comandos assíncronos para os processos em execução.  
O comando [`kill`](https://man7.org/linux/man-pages/man1/kill.1.html) permite enviar sinais para processos, sendo o mais frequente o sinal que indica ao programa que deve terminar ordeiramente (`SIGTERM`).  
Vimos também um exemplo de código com registo e chamada de uma rotina de tratamento de sinais assíncronos.

### Sugestão

Os _signals_ são também usados para o controlo de execução de vários programas numa única sessão de _shell_, o que se designa por _job control_.
Desta forma é possível ter vários programas a executar, um em primeiro plano (_foreground_) e outros em segundo plano (_background_).
Fica a sugestão de explorar estas capacidades com os comandos [`jobs`](https://man7.org/linux/man-pages/man1/jobs.1p.html), [`bg`](https://man7.org/linux/man-pages/man1/bg.1p.html), [`fg`](https://man7.org/linux/man-pages/man1/fg.1p.html), e com o operador `&` no fim dos comandos que permite executá-los em _background_.

----

Contactos para sugestões/correções: [LEIC-Alameda](mailto:leic-so-alameda@disciplinas.tecnico.ulisboa.pt), [LEIC-Tagus](mailto:leic-so-tagus@disciplinas.tecnico.ulisboa.pt), [LETI](mailto:leti-so-tagus@disciplinas.tecnico.ulisboa.pt)
