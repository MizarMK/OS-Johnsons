/**
 * Test to make sure patrons only take slices if they can get all the
 * slices they want
 */
#include <iostream>
#include <stdexcept>
#include <vector>
#include <unistd.h>
#include <pthread.h>

#include "buffetmutex.h"
#include "util.h"

using namespace std;

/** Sade gets there first and starts adding slices */
void *sade(void *arg) {
  // One slice, which Dorothea should get.
  sleep(1);
  Buffet *buffet = (Buffet*) arg;
  printAdd("Sade", 1, Cheese, buffet->AddPizza(1, Cheese));
  printAdd("Sade", 1, Meat, buffet->AddPizza(1, Meat));

  // Two slices, which should go to neville.
  sleep(3);
  printAdd("Sade", 1, Veggie, buffet->AddPizza(1, Veggie));
  sleep(1);
  printAdd("Sade", 1, Works, buffet->AddPizza(1, Works));

  return NULL;
}

/** Neville gets there next, but can't have the three slices he wants, so he waits */
void *neville(void *arg) {
  sleep(2);
  Buffet *buffet = (Buffet*) arg;
  printSlices("Neville", buffet->TakeAny(3));

  return NULL;
}

/** Dorothea gets there next.  She only wants two slices, so she can have them right away. */
void *dorothea(void *arg) {
  sleep(3);
  Buffet *buffet = (Buffet*) arg;
  printSlices("Dorothea", buffet->TakeAny(1));

  return NULL;
}

/** Create, run, and join actor threads */
int main() {
  // Initialize the monitor our threads are using
  Buffet buffet(20);

  // Make a few agent threads
  const int NOTHREADS = 3;
  pthread_t thread[NOTHREADS];

  if (pthread_create(thread + 0, NULL, dorothea, &buffet) != 0 ||
      pthread_create(thread + 1, NULL, neville, &buffet) != 0 ||
      pthread_create(thread + 2, NULL, sade, &buffet) != 0) {
    fail("Can't make a thread we need.\n");
  }

  // When we're done, join with all the threads
  for (int i=0; i < NOTHREADS; i++) {
    pthread_join(thread[i], NULL);
  }

  return 0;
}
