#include <stdlib.h>
#include <cstdio>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

const static int NOTESTS = 8;

void memReport();

void runTest(int test);

// Test prefix
static const char *TPF = "*********************";
// Error prefix
static const char *EPF = "!!!!!!!!!!!!!!!!!!!!!";

// Test reallocation (Only element in free list)
void test0() {
  cout << TPF << "Test 0:" << endl;
  // Allocate and immediately deallocate
  void *b1 = malloc(50);
  free(b1);
  // Test reallocate
  void *b2 = malloc(50);
  if (b1 != b2)
    cerr << EPF << "Failed!" << endl;
  memReport();
}

// Test smaller then larger reallocation
void test1() {
  cout << TPF << "Test 1:" << endl;
  // Allocate and immediately deallocate
  void *b1 = malloc(50);
  free(b1);
  // Realloc smaller
  void *b2 = malloc(25);
  if (b1 != b2)
    cerr << EPF << "Failed!" << endl;
  free(b2);
  b2 = malloc(49);
  if (b1 != b2)
    cerr << EPF << "Failed!" << endl;

  cout << "PreFree" << endl;
  memReport();
  free(b2);
  cout << "PostFree" << endl;
  memReport();
}

// Reallocate at end of free list
void test2() {
  cout << TPF << "Test 2:" << endl;
  void *b1 = malloc(50);
  malloc(50);
  void *b3 = malloc(100);
  free(b1);
  free(b3);
  void *b4 = malloc(75);

  if (b3 != b4) {
    cerr << EPF << "Failed!" << endl;
  }
  memReport();
}

// Reallocate at middle of free list
void test3() {
  cout << TPF << "Test 3:" << endl;
  void *b1 = malloc(50);
  malloc(50);
  void *b3 = malloc(100);
  malloc(50);
  void *b5 = malloc(50);
  free(b1);
  free(b3);
  free(b5);
  void *b6 = malloc(75);

  if (b3 != b6) {
    cerr << EPF << "Failed!" << endl;
  }
  memReport();
}

// Reallocate at start of free list
void test4() {
  cout << TPF << "Test 4:" << endl;
  void *b1 = malloc(100);
  malloc(50);
  void *b3 = malloc(50);
  free(b1);
  free(b3);
  void *b4 = malloc(75);

  if (b1 != b4) {
    cerr << EPF << "Failed!" << endl;
  }
  memReport();
}

// Coalescing
void test5() {
  cout << TPF << "Test 5:" << endl;
  // Try to test for coalescing first
  void *b1 = malloc(100);
  void *b2 = malloc(50);
  cout << "PreFree" << endl;
  memReport();

  free(b2);
  free(b1);
  cout << "PostFree" << endl;
  memReport();
  
  void *b3 = malloc(108);

  if (b1 != b3) {
    cerr << EPF << "Failed!" << endl;
  }
  cout << "PostNewMalloc" << endl;
  memReport();
}

// Allocate extra
void test6() {
  cout << TPF << "Test 6:" << endl;
  void *b1 = malloc(50);
  free(b1);
  // Should get new block
  malloc(51);
  memReport();

  // Should get b1 block
  void *b3 = malloc(40);
  if (b1 != b3) {
    cerr << EPF << "Failed!" << endl;
  }
}

// Test best-fit
void test7() {
  cout << TPF << "Test 7:" << endl;
  void *b1 = malloc(500);
  malloc(50);
  void *b3 = malloc(106);
  malloc(50);
  void *b5 = malloc(98);
  malloc(50);
  memReport();

  free(b1);
  free(b3);
  free(b5);

  void *b7 = malloc(100);
  if (b3 != b7)
    cerr << EPF << "Failed!" << endl;

  void *b8 = malloc(48);
  if (b8 != b5)
    cerr << EPF << "Failed!" << endl;
}

// Test small allocations
void test8() {
  cout << TPF << "Test 8:" << endl;
  void *b1 = malloc(3);
  void *b2 = malloc(2);
  malloc(3);
  free(b1);
  void *b4 = malloc(2);
  if (b4 != b1)
    cerr << EPF << "Failed!" << endl;
  free(b4);
  free(b2);
  // Should get b1 block
  void *b5 = malloc(5);
  if (b1 != b5) {
    cerr << EPF << "Failed!" << endl;
  }
  memReport();
}

static void (*testList[])() = {test0, test1, test2, test3, test4, test5, test6, test7, test8};
void runTest(int test) {
  testList[test](); 
}

int main(int argc, char *argv[]) {
  if (argc == 2) {
    runTest(atoi(argv[1]));
  } else if (argc == 1) {
    for (size_t i=0; i < sizeof(testList); i++) {
      ostringstream o;
      o << argv[0] << " " << i;
      system(o.str().c_str());
    }
  }
  //test0();
}

