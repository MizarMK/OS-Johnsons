#include <vector>

#include "buffetmutex.h"

using namespace std;

// General purpose fail function.  Print a message and exit
void fail(const char* const message);
// Print slices taken by name
void printSlices(const char* const name, const vector<SliceType> slices);
// Print addition of slices by name
void printAdd(const char* const name, int ct, SliceType t, bool success);
// Convert SliceType to string (gross)
string slice2Name(SliceType t);
// Return current timestamp
double ts();
