# Guião 6: Variáveis de Condição

![IST](img/IST_DEI.png)  

## Objectivos

No final deste guião, deverá ser capaz de:

- Coordenar tarefas usando variáveis de condição
- Saber como utilizar espera bloqueada em vez de espera ativa


## Requisitos

- Sistema operativo Linux Ubuntu 20.04 LTS (se não o tiverem disponível no vosso computador pessoal, podem utilizar os computadores do laboratório).


## Coordenação entre tarefas usando variáveis de condição

1. Faça clone do repositório e analise o programa [coordination.c](./coordination/coordination.c)
Este programa implementa uma situação simples de coordenação entre tarefas. Neste
caso, a tarefa inicial cria uma nova tarefa. Durante a execução do programa, há dois
momentos de coordenação (em que uma tarefa espera até que outra tarefa execute alguma
alteração ao estado partilhado):

   - A nova tarefa espera até que a variável partilhada `value` seja incrementada pela
tarefa inicial.
   - A tarefa inicial, após ter modificado a variável partilhada, espera pela terminação
da tarefa nova antes de imprimir o valor final no `stdout`.

2. Compile usando a Makefile fornecida e experimente correr o programa usando o
seguinte comando: `time ./coordination`


3. De seguida analise os tempos apresentados no ecrã, em especial a componente `real` (tempo que
passou até ao processo terminar) e a componente `user` (tempo de processador
consumido pelo programa). Os tempos user e real são muito próximos. Isso significa que o processo esteve em
execução (com uma ou outra tarefa) durante quase todo o tempo — apesar de grande
parte desse tempo ter sigo gasto em espera! Ou seja, este programa recorre a espera
ativa.
Considerando as duas esperas referidas acima, qual/quais são esperas ativas e
qual/quais são esperas bloqueadas?


4. Pretende-se corrigir o programa, substituindo a espera ativa que identificou na alínea
anterior por uma espera bloqueada. Para tal, usaremos uma variável de condição.

5. Abra o ficheiro [coordination_condvar.c](./coordination/coordination_condvar.c). Nele já encontra uma variável de condição
declarada (`pthread_cond_t cond`). No entanto, este programa está incompleto pois a
variável de condição não é usada.

6. Inicialize a variável de condição no início da função main, usando a função
`pthread_cond_init`.

7. No local onde antes havia uma espera ativa, implemente uma espera bloqueada
usando a função `pthread_cond_wait`. Não se esqueça que esta função deve ser sempre
chamada de acordo com este padrão:

   ```c
   while (! condicaoParaSairDaEspera) {
      pthread_cond_wait(variavel_de_condicao, trinco);
   }
   ```

   Relembrar que a função `pthread_cond_wait(&cond, &mutex)`:

   - primeiro, desbloqueia o &mutex
   - de seguida espera pelo signal na variável `&cond`, signal enviado com: `pthread_cond_signal(&cond)` ou com `pthread_cond_broadcast(&cond)`
   - finalmente bloqueia o &mutex novamente e continua execução

8. Não se esqueça de adicionar a chamada à função `pthread_cond_signal` no(s)
local(is) onde a condição de espera seja modificada.

9. Compile o novo programa (com `make coordination_condvar`) e execute-o de novo usando o seguinte comando:
`time ./coordination_condvar`. O que mudou nos tempos reportados pelo comando time (relativamente aos tempos reportados para a versão original)? Como explica essa mudança? Esta mudança é positiva? Porquê?

10.  Agora aplique estes conhecimentos no seu projeto, para implementar as situações de
coordenação entre tarefas que lá existam!


----

Contactos para sugestões/correções: [LEIC-Alameda](mailto:leic-so-alameda@disciplinas.tecnico.ulisboa.pt), [LEIC-Tagus](mailto:leic-so-tagus@disciplinas.tecnico.ulisboa.pt), [LETI](mailto:leti-so-tagus@disciplinas.tecnico.ulisboa.pt)
