// EPOS Thread Test Program

#include <utility/ostream.h>
#include <thread.h>
#include <alarm.h>

using namespace EPOS;

const int iterations = 10;

int func_a(void);
int func_b(void);
int func_c(void);
int func_d(void);

Thread * a;
Thread * b;
Thread * c;
Thread * d;
Thread * m;

OStream cout;

int main()
{
    cout << "Thread test" << endl;

    m = Thread::self();

    cout << "I'm the first thread of the first task created in the system." << endl;
    cout << "I'll now create two threads and then wait for them to finish ..." << endl;

    a = new Thread(&func_a);
    b = new Thread(&func_b);
    c = new Thread(&func_c);
    d = new Thread(&func_d);

    int status_a = a->join();
    int status_b = b->join();
    int status_c = c->join();
    int status_d = d->join();

    cout << "Thread A exited with status " << status_a
         << " and thread B exited with status " << status_b
         << " and thread C exited with status " << status_c
         << " and thread D exited with status " << status_d << "" << endl;

    delete a;
    delete b;
    delete c;
    delete d;

    cout << "It should not be shown on the display!" << endl;

    return 0;
}

int func_a(void)
{
    for(int i = iterations; i > 0; i--) {
        for(int i = 0; i < 19; i++)
            cout << "a" << Machine::cpu_id();
        cout << "" << endl;
        for(int i = 0; i < 50000; i++) {
          Alarm::delay(10);
        }
    }

    return 'A';
}

int func_b(void)
{
    for(int i = iterations; i > 0; i--) {
        for(int i = 0; i < 19; i++)
            cout << "b" << Machine::cpu_id();
        cout << "" << endl;
        for(int i = 0; i < 50000; i++) {
          Alarm::delay(10);
        }
    }

    return 'B';
}

int func_c(void)
{
    for(int i = iterations; i > 0; i--) {
        for(int i = 0; i < 19; i++)
            cout << "c" << Machine::cpu_id();
        cout << "" << endl;
        for(int i = 0; i < 50000; i++) {
          Alarm::delay(10);
        }
    }

    return 'C';
}

int func_d(void)
{
    for(int i = iterations; i > 0; i--) {
        for(int i = 0; i < 19; i++)
            cout << "d" << Machine::cpu_id();
        cout << "" << endl;
        for(int i = 0; i < 50000; i++) {
          Alarm::delay(10);
        }
    }

    return 'D';
}
