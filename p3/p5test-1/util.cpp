#include <sys/time.h>
#include <iostream>
#include <sstream>
#include <pthread.h>

#include "util.h"
#include "buffetmutex.h"

static timeval getTime() {
  timeval rv;
  if (gettimeofday(&rv, NULL) < 0) {
    cerr << "Unable to get time of day" << endl;
  }

  return rv;
}

timeval start = getTime();

double ts() {
  timeval t = getTime();
  double diff = (double) (t.tv_usec - start.tv_usec) / 1000000 +
         (double) (t.tv_sec - start.tv_sec);
  return diff;
}

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

// General purpose fail function.  Print a message and exit
void fail(const char* const message) {
  cerr << message << endl;
  exit(1);
}

string slice2Name(SliceType t) {
  switch(t) {
    case Veggie:
      return "Veggie";
    case Cheese:
      return "Cheese";
    case Meat:
      return "Meat";
    case Works:
      return "Works";
  }
  return NULL;
}

void printSlices(const char* const name, const vector<SliceType> slices) {
  pthread_mutex_lock(&mtx);
  ostringstream ss;
  ss << ts();
  ss << ": " << name << " gets ";
  for(vector<SliceType>::const_iterator i = slices.begin(); i != slices.end(); i++) {
    ss << slice2Name(*i) << ", ";
  }
  cout << ss.str() << endl;
  pthread_mutex_unlock(&mtx);
}

void printAdd(const char* const name, int ct, SliceType t, bool success) {
  if (!success) {
    cerr << "Add failed for " << name << endl;
  } else {
      cout << ts() << ": " << name << " added " << ct << " slices of " << slice2Name(t) << endl;
  }
}

