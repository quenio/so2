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
    int first = (n < 4)? n : 0;
    int second = (n < 4)? n + 1 : 4;

    for(int i = iterations; i > 0; i--) {

        table.lock();
        Display::position(l, c);
        cout << "thinking " << Machine::cpu_id();
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
        cout << " eating  " << Machine::cpu_id();
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
    cout << "  done   " << Machine::cpu_id();
    table.unlock();

    return iterations;
}

int main()
{
    table.lock();
    Display::clear();
    Display::position(0, 0);
    cout << "The Philosopher's Dinner:" << endl;

    for(int i = 0; i < 5; i++)
        chopstick[i] = new Semaphore;

    phil[0] = new Thread(&philosopher, 0,  5, 32);
    phil[1] = new Thread(&philosopher, 1, 10, 44);
    phil[2] = new Thread(&philosopher, 2, 16, 39);
    phil[3] = new Thread(&philosopher, 3, 16, 24);
    phil[4] = new Thread(&philosopher, 4, 10, 20);

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

    for(int i = 0; i < 5; i++) {
        int ret = phil[i]->join();
        table.lock();
        Display::position(20 + i, 0);
        cout << "Philosopher " << i << " ate " << ret << " times " << endl;
        table.unlock();
    }

    for(int i = 0; i < 5; i++)
        delete chopstick[i];
    for(int i = 0; i < 5; i++)
        delete phil[i];

    cout << "The end!" << endl;

    return 0;
}