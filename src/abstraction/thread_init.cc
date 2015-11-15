// EPOS Thread Abstraction Initialization

#include <system.h>
#include <thread.h>
#include <alarm.h>

__BEGIN_SYS

void Thread::init()
{
    // Thread::init() deve ser chamado somente pelo BSP.
    assert(Machine::cpu_id() == 0);

    // The installation of the scheduler timer handler must precede the
    // creation of threads, since the constructor can induce a reschedule
    // and this in turn can call timer->reset()
    // Letting reschedule() happen during thread creation is harmless, since
    // MAIN is created first and dispatch won't replace it nor by itself
    // neither by IDLE (which has a lower priority)
    if(Criterion::timed)
        _timer = new (SYSTEM) Scheduler_Timer(QUANTUM, time_slicer);

    Thread::init_rescheduler();
}

// Deve ser chamado pelas APs via System::init_rescheduler()
void Thread::init_rescheduler()
{
  CPU::int_disable();

  // Habilita escalonamento para ipi_send(cpu_id, IC::INT_RESCHEDULER)
  IC::int_vector(IC::INT_RESCHEDULER, time_slicer);

  // Para x86 APIC, na verdade, não há necessidade de habilitar interrupções individualmente.
  // Chamando aqui para manter o padrão e para suportar outras arquiteturas.
  IC::enable(IC::INT_RESCHEDULER);

  CPU::int_enable();
}

__END_SYS
