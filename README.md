# INE5424 - SO II - P2: Parallel Idle Threads

Alunos:
- Glaucia de Pádua da Silva - 09232087
- Quenio Cesar Machado dos Santos - 14100868

## Tarefa

O design atual do EPOS exige que sempre exista uma `thread` para ser executada. Para tal, na versão `monocore`, foi criada uma `Thread Idle`. Esta `thread` é executada sempre que não há outras `threads` para serem executadas, colocando a CPU em modo de baixo consumo de energia até que um evento externo ocorra. Agora, no cenário de `multicore`, o mesmo princípio deve ser preservado ou, alternativamente, o sistema deve ser reprojetado. Esta etapa do projeto deve conduzi-lo até a função `main()` da aplicação mantendo todos os demais cores ativos. Os testes devem ser executados com o QEMU emulando 8 CPUs.

## Solução

### Programa de Teste

O programa de teste apenas causa uma espera de meio-segundo que permite cada um dos `idle threads` executarem antes do programa terminar.

Para pode visualizar quais `idle threads` estão executando num determinado momento, a versão inicial do método `Thread::idle()` foi modificada para imprimir `Machine::cpu_id()` enquanto estiver no `loop`:

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
