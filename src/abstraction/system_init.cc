// EPOS System Abstraction Initialization

#include <system.h>
#include <alarm.h>

__BEGIN_SYS

void System::init()
{
    if(Traits<Alarm>::enabled)
        Alarm::init();

    if(Traits<Thread>::enabled)
        Thread::init();
}

void System::init_rescheduler()
{
  // Deve ser chamado somente pelas APs.
  assert(Machine::cpu_id() > 0);

  if(Traits<Thread>::enabled)
    Thread::init_rescheduler();
}

__END_SYS
