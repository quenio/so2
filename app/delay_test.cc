#include <utility/ostream.h>
#include <alarm.h>

using namespace EPOS;

OStream cout;

int main()
{
    db<Application>(WRN) << endl << "Esperando na CPU " << Machine::cpu_id() << "..." << endl;

    Alarm::delay(500000); // meio-segundo

    db<Application>(WRN) << "...TCHAU!" << endl;

    return 0;
}
