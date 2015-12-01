// EPOS Semaphore Abstraction Test Program

#include <utility/ostream.h>
#include <thread.h>
#include <mutex.h>
#include <semaphore.h>
#include <alarm.h>
#include <display.h>

using namespace EPOS;

const int iterations = 4;

Mutex table;

volatile Timer::Tick s[5];
Thread * phil[5];
Semaphore * chopstick[5];

OStream cout;

bool is_even(int n)
{
  bool result = true;
  while (n > 0) {
      n--;
      result = !result;
  }
  return result;
}

int count_odd_numbers()
{
  int *n = new (SYSTEM) int;
  *n = 0;
  for(int i = 0; i < 20000; i++) {
    if (!is_even(i)) (*n)++;
  }

  int result = *n;

  delete n;

  return result;
}

int count_even_numbers()
{
  int n = 0;
  for(int i = 0; i < 30000; i++) {
    if (is_even(i)) n++;
  }
  return n;
}

int philosopher(int n, int l, int c)
{
    s[n] = (Timer::tick_count() - s[n]);

    int first = (n < 4)? n : 0;
    int second = (n < 4)? n + 1 : 4;

    for(int i = iterations; i > 0; i--) {

        table.lock();
        Display::position(l, c);
        cout << "thinking " << Machine::cpu_id()
             << " - " << Thread::self()->ready_time();
        table.unlock();

        int n = count_odd_numbers();
        table.lock();
        Display::position(3, 0);
        cout << " odd numbers: " << n << "-" << Machine::cpu_id();
        table.unlock();

        chopstick[first]->p();    // get first chopstick
        chopstick[second]->p();   // get second chopstick

        table.lock();
        Display::position(l, c);
        cout << " eating  " << Machine::cpu_id()
             << " - " << Thread::self()->ready_time();
        table.unlock();

        n = count_even_numbers();
        table.lock();
        Display::position(3, 0);
        cout << "even numbers: " << n << "-" << Machine::cpu_id();
        table.unlock();

        chopstick[first]->v();    // release first chopstick
        chopstick[second]->v();   // release second chopstick
    }

    table.lock();
    Display::position(l, c);
    cout << "  done   " << Machine::cpu_id()
         << " - " << Thread::self()->ready_time();
    table.unlock();

    return iterations;
}

struct Spaced
{
public:
  Spaced(const unsigned long & number): _number(number) {}

  unsigned long number() const { return _number; }

private:
  unsigned long _number;
};

OStream & operator << (OStream & os, const Spaced & spaced)
{
  unsigned long n = spaced.number();
  int space_count = (n >= 10000 ? 0 : (n >= 1000 ? 1 : (n >= 100 ? 2 : (n >= 10 ? 3 : 4))));
  for (int i = 0; i < space_count; i++) cout << " ";
  os << n;
}

int main()
{
    Timer::Tick program_exec_time = Timer::tick_count();

    table.lock();
    Display::clear();
    Display::position(0, 0);
    cout << "The Philosopher's Dinner:" << endl;

    for(int i = 0; i < 5; i++)
        chopstick[i] = new Semaphore;

    s[0] = Timer::tick_count(0);
    phil[0] = new Thread(Thread::Configuration(Thread::READY, Thread::Criterion(Thread::NORMAL)), &philosopher, 0,  5, 32);

    s[1] = Timer::tick_count(1);
    phil[1] = new Thread(Thread::Configuration(Thread::READY, Thread::Criterion(Thread::NORMAL)), &philosopher, 1, 10, 44);

    s[2] = Timer::tick_count(2);
    phil[2] = new Thread(Thread::Configuration(Thread::READY, Thread::Criterion(Thread::NORMAL)), &philosopher, 2, 16, 39);

    s[3] = Timer::tick_count(3);
    phil[3] = new Thread(Thread::Configuration(Thread::READY, Thread::Criterion(Thread::NORMAL)), &philosopher, 3, 16, 24);

    s[4] = Timer::tick_count(0);
    phil[4] = new Thread(Thread::Configuration(Thread::READY, Thread::Criterion(Thread::NORMAL)), &philosopher, 4, 10, 20);

    cout << "Philosophers are alive and hungry!" << endl;

    Display::position(7, 44);
    cout << '/';
    Display::position(13, 44);
    cout << '\\';
    Display::position(16, 35);
    cout << '|';
    Display::position(13, 27);
    cout << '/';
    Display::position(7, 27);
    cout << '\\';
    Display::position(19, 0);

    cout << "The dinner is served ..." << endl;
    table.unlock();

    Timer::Tick total_cpu_tick[Traits<Build>::CPUS];
    for(int i = 0; i < 5; i++) {
        int ret = phil[i]->join();
        table.lock();
        Display::position(20 + i, 0);
        cout << "Philosopher " << i;
        Timer::Tick total_thread_tick = 0;
        for (int cpu_id = 0; cpu_id < Machine::n_cpus(); cpu_id++) {
          Timer::Tick tick_per_cpu = phil[i]->total_tick(cpu_id);
          total_thread_tick += tick_per_cpu;
          total_cpu_tick[cpu_id] += tick_per_cpu;
          cout << " | " << cpu_id << ": " << Spaced(tick_per_cpu);
        }
        cout << " | T: " << Spaced(total_thread_tick);
        cout << " | S: " << s[i] << endl;
        table.unlock();
    }
    table.lock();
    Display::position(25, 0);
    cout << "CPU Totals   ";
    Timer::Tick total_tick = 0;
    for (int cpu_id = 0; cpu_id < Machine::n_cpus(); cpu_id++) {
      Timer::Tick tick_per_cpu = total_cpu_tick[cpu_id];
      total_tick += tick_per_cpu;
      cout << " | " << cpu_id << ": " << Spaced(tick_per_cpu);
    }
    cout << " | T: " << Spaced(total_tick) << endl << endl;
    table.unlock();

    for(int i = 0; i < 5; i++)
        delete chopstick[i];
    for(int i = 0; i < 5; i++)
        delete phil[i];

    program_exec_time = (Timer::tick_count() - program_exec_time);
    cout << "The end! Program Exec Time = " << program_exec_time
         << " - CPU: " << Machine::cpu_id() << endl;

    return 0;
}
