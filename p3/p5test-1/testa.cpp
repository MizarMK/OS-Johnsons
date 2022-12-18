/**
 * Simple test case for the monitor.  Non-veg patron won't take
 * any veg slices if there's a vegetarian waiting
 */
#include <iostream>
#include <stdexcept>
#include <vector>
#include <unistd.h>
#include <pthread.h>

#include "buffetmutex.h"
#include "util.h"

using namespace std;

/** Caitlyn tries to get two veg slices */
void *caitlyn(void *arg) {
  sleep(2);
  Buffet *buffet = (Buffet*) arg;
  printSlices("Caitlyn", buffet->TakeVeg(2));

  return NULL;
}

/** Evangelina gets there next, and wants two slices of anything */
void *evangelina(void *arg) {
  sleep(3);
  Buffet *buffet = (Buffet*) arg;
  printSlices("Evangelina", buffet->TakeAny(2));
  
  return NULL;
}

/** Winnifred puts slices on the buffet, gradually */
void *winnifred(void *arg) {
  sleep(1);
  Buffet *buffet = (Buffet*) arg;
  printAdd("Winnifred", 1, Cheese, buffet->AddPizza(1, Cheese));
  printAdd("Winnifred", 1, Meat, buffet->AddPizza(1, Meat));

  sleep(3);
  printAdd("Winnifred", 1, Veggie, buffet->AddPizza(1, Veggie));

  sleep(1);
  printAdd("Winnifred", 1, Works, buffet->AddPizza(1, Works));

  return NULL;
}

/** Create, run, and join actor threads */
int main() {
  // Initialize the monitor our threads are using
  Buffet buffet(20);

  // Make a few agent threads
  const int NOTHREADS = 3;
  pthread_t thread[NOTHREADS];

  if (pthread_create(thread + 0, NULL, caitlyn, &buffet) != 0 ||
      pthread_create(thread + 1, NULL, evangelina, &buffet) != 0 ||
      pthread_create(thread + 2, NULL, winnifred, &buffet) != 0) {
    fail("Cannot make a thread we need");
  }

  // When we're done, join with all the threads
  for (int i=0; i < NOTHREADS; i++) {
    pthread_join(thread[i], NULL);
  }

  return 0;
}
