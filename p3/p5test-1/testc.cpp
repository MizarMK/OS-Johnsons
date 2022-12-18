/**
 * Simple test case for the monitor.  A server will have to wait if the
 * buffet is too full
 */
#include <iostream>
#include <stdexcept>
#include <vector>
#include <unistd.h>
#include <pthread.h>

#include "buffetmutex.h"
#include "util.h"

using namespace std;

/** Scot gets there first, with 9 slices. */
void *scot(void *arg) {
  sleep(1);
  Buffet *buffet = (Buffet*) arg;
  if (buffet->AddPizza(9, Meat)) {
    cout << ts() << ": Scot dropped off 9 slices" << endl;
  } else {
    cerr << ts() << ": Scot couldn't drop off everything" << endl;
  }

  return NULL;
}

/** Shirlee gets there next, but can't drop off all her slices. */
void *shirlee(void *arg) {
  sleep(2);
  Buffet *buffet = (Buffet*) arg;
  if (buffet->AddPizza(12, Works)) {
    cout << ts() << ": Shirlee dropped off 12 slices" << endl;
  } else {
    cerr << ts() << ": Shirlee couldn't drop off everything" << endl;
  }

  return NULL;
}

/** Marnie takes some slices after three seconds.  This will let shirlee finish. */
void *marnie(void *arg) {
  sleep(3);
  Buffet *buffet = (Buffet*) arg;
  printSlices("Marnie", buffet->TakeAny(2));

  return NULL;
}

/** Create, run, and join actor threads */
int main() {
  // Initialize the monitor our threads are using
  Buffet buffet(20);

  // Make a few agent threads
  const int NOTHREADS = 3;
  pthread_t thread[NOTHREADS];

  if (pthread_create(thread + 0, NULL, marnie, &buffet) != 0 ||
      pthread_create(thread + 1, NULL, shirlee, &buffet) != 0 ||
      pthread_create(thread + 2, NULL, scot, &buffet) != 0) {
    fail("Can't make a thread we need");
  }

  // When we're done, join with all the threads
  for (int i=0; i < NOTHREADS; i++) {
    pthread_join(thread[i], NULL);
  }

  return 0;
}
