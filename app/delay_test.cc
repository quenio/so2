#include <utility/ostream.h>
#include <alarm.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << endl << "Esperando..." << endl;

    Alarm::delay(500000); // meio-segundo

    cout << "...TCHAU!" << endl;

    return 0;
}
