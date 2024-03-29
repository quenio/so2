// EPOS PC Timer Mediator Implementation

#include <machine.h>
#include <ic.h>
#include <machine/pc/timer.h>

__BEGIN_SYS

// Class attributes
PC_Timer * PC_Timer::_channels[CHANNELS];
volatile Timer::Tick PC_Timer::_tick_count[Traits<Build>::CPUS];

// Class methods
void PC_Timer::int_handler(const Interrupt_Id & i)
{
    _tick_count[Machine::cpu_id()]++;

    if(_channels[SCHEDULER] && (--_channels[SCHEDULER]->_current[Machine::cpu_id()] <= 0)) {
        _channels[SCHEDULER]->_current[Machine::cpu_id()] = _channels[SCHEDULER]->_initial;
        _channels[SCHEDULER]->_handler(i);
    }

    if((!Traits<System>::multicore || (Traits<System>::multicore && (Machine::cpu_id() == 0))) && _channels[ALARM]) {
        _channels[ALARM]->_current[0] = _channels[ALARM]->_initial;
        _channels[ALARM]->_handler(i);
    }

    if((!Traits<System>::multicore || (Traits<System>::multicore && (Machine::cpu_id() == 0))) && _channels[USER]) {
        if(_channels[USER]->_retrigger)
            _channels[USER]->_current[0] = _channels[USER]->_initial;
        _channels[USER]->_handler(i);
    }
}

__END_SYS
