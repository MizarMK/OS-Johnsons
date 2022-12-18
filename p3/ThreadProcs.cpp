#include "buffet.h"
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iostream>
#include <thread>

struct threadArgs {
    SliceType type;
    int numSlices;
};
extern Buffet BUFFET;

using namespace std;
using chrono::seconds;

void *prodProcess (void *args) {
    struct threadArgs tArgs = *((struct threadArgs *)args);
    int numSlices = tArgs.numSlices;
    SliceType type = tArgs.type;

    //cout << "Producer adding: " << type << " starting!" << endl;

    for (int i = 0; i < 15; i++) { // run for 15s 
	this_thread::sleep_for(chrono::seconds(2));
	//cout << "Prod thread entered"<< endl;
        if (i % 2 == 0) {
            BUFFET.AddPizza(numSlices, type);
	    //cout << numSlices << " slices added!" << endl; 
	}
	//cout << "after sleep..." << endl;
    }
    return NULL;
}

void *consProcess(void *args) {
    struct threadArgs tArgs = *((struct threadArgs *)args);
    int numSlices = tArgs.numSlices;
    SliceType type = tArgs.type;
  
    //cout << "Consumer wanting: " << type << " starting!" << endl;

    for (int i = 0; i < 15; i++) { // run for 15s
       this_thread::sleep_for(chrono::seconds(1));
       if (type == Cheese or type == Veggie) {
	//   cout << "taking veg..." << endl;
           BUFFET.TakeVeg(numSlices);
       }
       else
	  // cout << "taking any..." << endl;
           BUFFET.TakeAny(numSlices);
       cout << "done eating (" << type << ")!" << endl;
     
    }
    return NULL;
}
