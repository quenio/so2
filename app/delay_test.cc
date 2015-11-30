#include <utility/ostream.h>
#include <alarm.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << endl << "Esperando na CPU " << Machine::cpu_id() << "..." << endl;

    Timer::Tick total_tick;

    total_tick = Timer::tick_count();
    Alarm::delay(1000000 * 10); // um segundo
    total_tick = (Timer::tick_count() - total_tick);

    cout << "...TCHAU! " << total_tick << endl;

    return 0;
}
