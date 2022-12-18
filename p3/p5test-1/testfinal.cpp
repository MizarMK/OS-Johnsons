/**
 * Multi-threaded program to simulate servers and patrons at a pizza buffet restaurant
 */
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#include "util.h"
#include "buffet.h"

/** Mary serves cheese pizza */
void *mary(void *arg) {
  Buffet *buffet = (Buffet*) arg;
  while (buffet->AddPizza(6, Cheese)) {
    // Rest for a moment, then add some slices
    usleep(rand() % 150);
  }

  return NULL;
}

/** Nelle serves pepperoni pizza */
void *nelle(void *arg) {
  Buffet *buffet = (Buffet*) arg;
  while (buffet->AddPizza(8, Meat)) {
    // Rest for a moment, then add some slices
    usleep(rand() % 200);
  }

  return NULL;
}

/** John serves veggie pizza */
void *john(void *arg) {
  Buffet *buffet = (Buffet*) arg;
  while (buffet->AddPizza(3, Veggie)) {
    // Rest for a moment, then add some slices
    usleep(rand() % 75);
  }

  return NULL;
}

/** Genevive serves works pizza */
void *genevive(void *arg) {
  Buffet *buffet = (Buffet*) arg;
  while (buffet->AddPizza(4, Works)) {
    // Rest for a moment, then add some slices
    usleep(rand() % 100);
  }

  return NULL;
}

/** Laverne eats any kind of pizza, 2 slices at a time */
void *laverne(void *arg) {
  Buffet *buffet = (Buffet*) arg;
  try {
    while (true) {
      vector<SliceType> slices = buffet->TakeAny(2);
      printSlices("Laverne", slices);

      // Rest for a moment, then have some more
      usleep(rand() % 50);
    }
  } catch (const runtime_error& e) {
  }

  return NULL;
}

/** Michal eats vegetarian pizza, 3 slices at a time */
void *michal(void *arg) {
  Buffet *buffet = (Buffet*) arg;
  try {
    while (true) {
      vector <SliceType> slices = buffet->TakeVeg(3);
      printSlices("Michal", slices);

      // Rest for a moment, then have some more
      usleep(rand() % 75);
    }
  } catch (const runtime_error& e) {
  }

  return NULL;
}

/** Kelsey eats vegetarian pizza, 1 slice at a time */
void *kelsey(void *arg) {
  Buffet *buffet = (Buffet*) arg;
  try {
    while (true) {
      vector <SliceType> slices = buffet->TakeVeg(1);
      printSlices("Kelsey", slices);

      // Rest for a moment, then have some more
      usleep(rand() % 25);
    }
  } catch (const runtime_error& e) {
  }

  return NULL;
}

/** Daron eats any kind of pizza, 2 slices at a time */
void *daron(void *arg) {
  Buffet *buffet = (Buffet*) arg;
  try {
    while (true) {
      vector <SliceType> slices = buffet->TakeAny(2);
      printSlices("Daron", slices);

      // Rest for a moment, then have some more
      usleep(rand() % 50);
    }
  } catch (const runtime_error& e) {
  }

  return NULL;
}

int main() {
  // Initialize the monitor our threads are using
  Buffet buffet(20);

  // Make a few agent threads
  const int NOTHREADS = 8;
  pthread_t thread[NOTHREADS];

  if (pthread_create(thread + 0, NULL, mary, &buffet) != 0 ||
      pthread_create(thread + 1, NULL, nelle, &buffet) != 0 ||
      pthread_create(thread + 2, NULL, john, &buffet) != 0 ||
      pthread_create(thread + 3, NULL, genevive, &buffet) != 0 ||
      pthread_create(thread + 4, NULL, laverne, &buffet) != 0 ||
      pthread_create(thread + 5, NULL, michal, &buffet) != 0 ||
      pthread_create(thread + 6, NULL, kelsey, &buffet) != 0 ||
      pthread_create(thread + 7, NULL, daron, &buffet) != 0) {
    fail("Can't make a thread we need");
  }

  // Buffet is open for 10 seconds, eat fast
  sleep(10);
  buffet.close();

  // When we're done, join with all the threads
  for (int i=0; i < NOTHREADS; i++) {
    pthread_join(thread[i], NULL);
  }

  return 0;
}
