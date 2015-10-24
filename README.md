# INE5424 - SO II - P2: Parallel Idle Threads

Alunos:
- Glaucia de Pádua da Silva - 09232087
- Quenio Cesar Machado dos Santos - 14100868

## Tarefa

O design atual do EPOS exige que sempre exista uma `thread` para ser executada. Para tal, na versão `monocore`, foi criada uma `Thread Idle`. Esta `thread` é executada sempre que não há outras `threads` para serem executadas, colocando a CPU em modo de baixo consumo de energia até que um evento externo ocorra. Agora, no cenário de `multicore`, o mesmo princípio deve ser preservado ou, alternativamente, o sistema deve ser reprojetado. Esta etapa do projeto deve conduzi-lo até a função `main()` da aplicação mantendo todos os demais cores ativos. Os testes devem ser executados com o QEMU emulando 8 CPUs.

## Solução

### Implementando o programa de teste

O programa de teste apenas causa uma espera de meio-segundo que permite cada um dos `idle threads` executarem antes do programa terminar.

Para poder visualizar quais `idle threads` estão executando num determinado momento, a versão inicial do método `Thread::idle()` foi modificada para imprimir `Machine::cpu_id()` enquanto estiver no `idle loop`:

```cpp
while(_thread_count > 1) {

    db<Thread>(WRN) << Machine::cpu_id(); // identificando a CPU em idle.

    CPU::int_enable();
    CPU::halt();
}
```

Para a execução com apenas uma CPU, a saída do programa é a seguinte:

```
Setting up this machine as follows:
  Processor:    IA32 at 2498 MHz (BUS clock = 125 MHz)
  Memory:       262144 Kbytes [0x00000000:0x10000000]
  User memory:  261800 Kbytes [0x00000000:0x0ffaa000]
  PCI aperture: 44996 Kbytes [0xfc000000:0xfebf1000]
  Node Id:      will get from the network!
  Setup:        19456 bytes
  APP code:     17536 bytes	data: 480 bytes
  CPU count:    1

Esperando...
00000000000000000000000000000000000000000000000000000000000000000000000000000000
00000000000000000000000000000000000000000000000000000000000000000000000000000000
00000000000000000000000000000000000000000000000000000000000000000000000000000000
00000000000000000000000000000000000000000000000000000000000000000000000000000000
00000000000000000000000000000000000000000000000000000000000000000000000000000000
00000000000000000000000000000000000000000000000000000000000000000000000000000000
0000000000000000000...TCHAU!
The last thread has exited!
Rebooting the machine ...
```

Observe que os zeros são impressos dentro do `idle loop` porque existe apenas uma CPU (cpu_id = 0) executando em `idle`.

### Executando com duas CPUs

Ainda usando a versão inicial do EPOS disponibilizada pelo professor para P2, vamos executar o programa de testes com duas CPUs:

```
Setting up this machine as follows:
  Processor:    IA32 at 797 MHz (BUS clock = 125 MHz)
  Memory:       262144 Kbytes [0x00000000:0x10000000]
  User memory:  261784 Kbytes [0x00000000:0x0ffa6000]
  PCI aperture: 44996 Kbytes [0xfc000000:0xfebf1000]
  Node Id:      will get from the network!
  Setup:        20992 bytes
  APP code:     17504 bytes	data: 512 bytes
  CPU count:    2

Esperando...
1
Esperando...
IC::exc_gpf(cs=8,ip=0x00003f50,fl=2)
The running thread will now be termin1ated!
10010101010101010010101010101001010010101010101010101010101011010101010101010101
00101010101010101010101010101010101010101010101010101010101010101010101010101010
10101010110101010101010101010101010101010101010101010101010101010101010101010101
...
10101010101010101010101010101010101010101010101010101010101010100101010101010101
01010101010101010101010101010101010101010101010101010101010101010101010101010101
01010101010101010101010101010110010101010101010101010101010101010101010101010101
0101010101010101010101010101...TCHAU!
00101010101010101010101010101010101010101010101010101010100101010101010101010101
01010101010101010101010101010101010101010101010101010101010101010101010101010101
...
01010101010101010101010101010101010101010101010101010101010101010101010101010101
01010101010100101010101010110101010101010101010101010101010101010101010101010101
010101010101010101010101010101IC::exc_pf[address=0x66333038](cs=8,ip=0x00001b23,
fl=46,err=PR)
The running thread will now be terminated!
IC::exc_pf[address=0x53e5895d](cs=8,ip=0x00002065,fl=2,err=)
The running thread will now be terminated!
IC::exc_pf[address=0x53e58971](cs=8,ip=0x00002dac,fl=97,err=)
The running thread will now be terminated!
IC::exc_pf[address=0x53e58971](cs=8,ip=0x00002dac,fl=97,err=)
...
IC::exc_pf[address=0x53e58971](cs=8,ip=0x00002dac,fl=97,err=)
The running thread will now be terminated!
IC::exc_pf[address=0x53e58971](cs=8,ip=0x00002dac,fl=97,err=)
ThThe last thread has exited!
Rebooting the machine ...
e
```

Na saída do programa acima, existem alguns pontos importantes para se observar:
- O programa foi iniciado e executado duas vezes concorrentemente, uma vez em cada CPU. Observe o texto `Esperando...` sendo impresso duas.
- Logo no inicio da execução do programa, abaixo do segundo `Esperando...`, ocorreu um `general protection fault` (GPF - indicado pelo texto `IC::exc_gpf`). A função do EPOS que cuida de GPFs tenta terminar a execução do programa, mas ele continua executando.
- Logo após o GPF, os `idle threads` das duas CPUs ficam executando em seu `idle loop`, o que é observado através da alternância entre zeros e uns impressos na saída.
- Dado o tempo ainda maior que o meio-segundo de espera programado, a execução do programa termina em ambas as CPUs.
- Antes de terminar a execução, porém, vê-se uma séria de `page faults` (PFs).

As observações acima mostram os seguintes problemas com a versão do EPOS disponilizada para este trabalho:
- A inicialização do sistema não está tomando o cuidado de iniciar a execução do programa em apenas uma CPU.
- A GPF indica acesso de memória fora do seguimento, o que deve estar ocorrendo quando as CPUs tentam acesso concorrente a `heap` do sistema, ou a outros recursos compartilhados.
- Ambos as CPUs provavelmente estavam tentando desalocar memória, ou outros resources, ao término da execução do programa, o que deve ter causado as PFs.

As próximas seções irão mostrar mudanças progressivas sobre a versão inicial do código para resolver estes problemas. 
