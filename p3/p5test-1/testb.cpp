/**
 * Simple test case for the monitor.  A non-veg patron can take
 * veg slices, if they get there first
 */
#include <iostream>
#include <stdexcept>
#include <vector>
#include <unistd.h>
#include <pthread.h>

#include "buffetmutex.h"
#include "util.h"

using namespace std;

/** Laurie tries to take two slices of anything */
void *laurie(void *arg) {
  sleep(2);
  Buffet *buffet = (Buffet*) arg;
  printSlices("Laurie", buffet->TakeAny(2));

  return NULL;
}

/** Ladonna gets to the buffet and wants two veg slices, but laurie
    already took one */
void *ladonna(void *arg) {
  sleep(3);
  Buffet *buffet = (Buffet*) arg;
  try {
    printSlices("Ladonna", buffet->TakeVeg(2));
  } catch (const runtime_error& e) {
    cerr << ts() << ": Ladonna gets nothing" << endl;
  }

  return NULL;
}

/** Trent puts slices on the buffet, gradually */
void *trent(void *arg) {
  sleep(1);
  Buffet *buffet = (Buffet*) arg;
  printAdd("Trent", 1, Veggie, buffet->AddPizza(1, Veggie));
  printAdd("Trent", 1, Meat, buffet->AddPizza(1, Meat));

  sleep(3);
  printAdd("Trent", 1, Cheese, buffet->AddPizza(1, Cheese));

  sleep(1);
  printAdd("Trent", 1, Works, buffet->AddPizza(1, Works));

  return NULL;
}

/** Create, run, and join actor threads */
int main() {
  // Initialize the monitor our threads are using
  Buffet buffet(20);

  // Make a few agent threads
  const int NOTHREADS = 3;
  pthread_t thread[NOTHREADS];

  if (pthread_create(thread + 0, NULL, ladonna, &buffet) != 0 ||
      pthread_create(thread + 1, NULL, laurie, &buffet) != 0 ||
      pthread_create(thread + 2, NULL, trent, &buffet) != 0) {
    fail("Can't make a thread we need");
  }

  sleep(6);

  // ladonna will be stuck, since there's only one veg slice left
  buffet.close();

  // When we're done, join with all the threads
  for (int i=0; i < NOTHREADS; i++) {
    pthread_join(thread[i], NULL);
  }

  return 0;
}
