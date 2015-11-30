// EPOS Thread Abstraction Declarations

#ifndef __thread_h
#define __thread_h

#include <utility/queue.h>
#include <utility/handler.h>
#include <cpu.h>
#include <machine.h>
#include <system.h>
#include <scheduler.h>

extern "C" { void __exit(); }

__BEGIN_SYS

class Thread
{
    friend class Init_First;
    friend class System;
    friend class Scheduler<Thread>;
    friend class Synchronizer_Common;
    friend class Alarm;
    friend class IA32;

protected:
    static const bool smp = Traits<Thread>::smp;
    static const bool preemptive = Traits<Thread>::Criterion::preemptive;
    static const bool reboot = Traits<System>::reboot;

    static const unsigned int QUANTUM = Traits<Thread>::QUANTUM;
    static const unsigned int STACK_SIZE = Traits<Application>::STACK_SIZE;

    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::Context Context;

public:
    // Thread State
    enum State {
        RUNNING,
        READY,
        SUSPENDED,
        WAITING,
        FINISHING
    };

    // Thread Priority
    typedef Scheduling_Criteria::Priority Priority;

    // Thread Scheduling Criterion
    typedef Traits<Thread>::Criterion Criterion;
    enum {
        HIGH    = Criterion::HIGH,
        NORMAL  = Criterion::NORMAL,
        LOW     = Criterion::LOW,
        MAIN    = Criterion::MAIN,
        IDLE    = Criterion::IDLE
    };

    // Thread Configuration
    struct Configuration {
        Configuration(const State & s = READY, const Criterion & c = NORMAL, unsigned int ss = STACK_SIZE)
        : state(s), criterion(c), stack_size(ss) {}

        State state;
        Criterion criterion;
        unsigned int stack_size;
    };

    // Thread Queue
    typedef Ordered_Queue<Thread, Criterion, Scheduler<Thread>::Element> Queue;

    typedef Scheduler_Timer Timer;

public:
    template<typename ... Tn>
    Thread(int (* entry)(Tn ...), Tn ... an);
    template<typename ... Tn>
    Thread(const Configuration & conf, int (* entry)(Tn ...), Tn ... an);
    ~Thread();

    const volatile State & state() const { return _state; }

    const volatile Priority & priority() const { return _link.rank(); }
    void priority(const Priority & p);

    int join();
    void pass();
    void suspend() { suspend(false); }
    void resume();

    int total_tick(int cpu_id) { return _total_tick[cpu_id]; }

    static Thread * volatile self() { return running(); }
    static void yield();
    static void exit(int status = 0);

protected:
    void constructor_prolog(unsigned int stack_size);
    void constructor_epilog(const Log_Addr & entry, unsigned int stack_size);

    static Thread * volatile running() { return _scheduler.chosen(); }

    Queue::Element * link() { return &_link; }

    Criterion & criterion() { return const_cast<Criterion &>(_link.rank()); }

    static void lock()
    {
      CPU::int_disable();
      spinLock();
    }

    static void unlock()
    {
      spinUnlock();
      CPU::int_enable();
    }

    static bool locked() { return CPU::int_disabled(); }

    void suspend(bool locked);

    static void sleep(Queue * q);
    static void wakeup(Queue * q);
    static void wakeup_all(Queue * q);

    static void reschedule();
    static void reschedule_cpu(int cpu_id);
    static void time_slicer(const IC::Interrupt_Id & interrupt);

    static void dispatch(Thread * prev, Thread * next, bool charge = true);

    static int idle();

private:
    void load()
    {
      _tick_count = Timer::tick_count();
      _context->load();
    }

    static void init();
    static void init_rescheduler();

    static void spinLock() { if (smp) _spinLock.acquire(); }
    static void spinUnlock() { if (smp) _spinLock.release(); }

protected:
    char * _stack;
    Context * volatile _context;
    volatile State _state;
    Queue * _waiting;
    Thread * volatile _joining;
    Queue::Element _link;
    Timer::Tick _tick_count, _total_tick[Traits<Build>::CPUS];

    static volatile unsigned int _thread_count;
    static Scheduler_Timer * _timer;
    static Scheduler<Thread> _scheduler;
    static Spin _spinLock;
};


template<typename ... Tn>
inline Thread::Thread(int (* entry)(Tn ...), Tn ... an)
: _state(READY), _waiting(0), _joining(0), _link(this, NORMAL)
{
    constructor_prolog(STACK_SIZE);
    _context = CPU::init_stack(_stack + STACK_SIZE, &__exit, entry, an ...);
    constructor_epilog(entry, STACK_SIZE);
}

template<typename ... Tn>
inline Thread::Thread(const Configuration & conf, int (* entry)(Tn ...), Tn ... an)
: _state(conf.state), _waiting(0), _joining(0), _link(this, conf.criterion)
{
    constructor_prolog(conf.stack_size);
    _context = CPU::init_stack(_stack + conf.stack_size, &__exit, entry, an ...);
    constructor_epilog(entry, conf.stack_size);
}


// An event handler that triggers a thread (see handler.h)
class Thread_Handler : public Handler
{
public:
    Thread_Handler(Thread * h) : _handler(h) {}
    ~Thread_Handler() {}

    void operator()() { _handler->resume(); }

private:
    Thread * _handler;
};

__END_SYS

#endif
