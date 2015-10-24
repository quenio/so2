// EPOS Alarm Abstraction Test Program

#include <utility/ostream.h>
#include <alarm.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << "Delay Test" << endl;

    Alarm::delay(2000000);

    cout << "I'm done, bye!" << endl;

    return 0;
}
