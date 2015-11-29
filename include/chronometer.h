// EPOS Chronometer Abstraction Declarations

#ifndef __chronometer_h
#define __chronometer_h

#include <tsc_chronometer.h>
#include <alarm.h>

__BEGIN_SYS

class Alarm_Chronometer
{
private:
    typedef Alarm::Tick Time_Stamp;

public:
    typedef TSC::Hertz Hertz;
    typedef RTC::Microsecond Microsecond;

public:
    Alarm_Chronometer() : _start(0), _stop(0) {}

    Hertz frequency() { return Alarm::frequency(); }

    void reset() { _start = 0; _stop = 0; }
    void start() { if(_start == 0) _start = Alarm::_elapsed; }
    void lap() { if(_start != 0) _stop = Alarm::_elapsed; }
    void stop() { lap(); }

    Microsecond read() { return ticks() * 1000000 / frequency(); }

private:
    Time_Stamp ticks() {
        if(_start == 0)
            return 0;
        if(_stop == 0)
            return Alarm::_elapsed - _start;
        return _stop - _start;
    }

private:
    TSC tsc;
    Time_Stamp _start;
    Time_Stamp _stop;
};

class Chronometer: public IF<Traits<TSC>::enabled && !Traits<System>::multicore, TSC_Chronometer, Alarm_Chronometer>::Result {};

__END_SYS

#endif
