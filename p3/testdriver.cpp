#include <stdio.h>
#include <pthread.h>
#include "ThreadProcs.cpp"

using namespace std;
using chrono::seconds;

Buffet BUFFET(20);

int main() {
    const int numThreads = 4;
    const int numProd = numThreads, numCons = numThreads; // assign prods and consumers
    srand((unsigned int)time(0));

    pthread_t prodIds[numProd];
    pthread_t consIds[numCons];
    chrono::steady_clock::time_point start = chrono::steady_clock::now();

    struct threadArgs *Pargs = new struct threadArgs[numThreads];
    struct threadArgs *Cargs = new struct threadArgs[numThreads];
    for (int i = 0; i < numThreads; i++) { // create child threads with arguments passed through void* struct
        Pargs[i].type = SliceType(i); // 1 producer of each type
        Pargs[i].numSlices = (rand() % 5) + 1; // add between 1 and 4 slices

        if (pthread_create(prodIds + i, NULL, prodProcess, Pargs + i) != 0) {
            fprintf(stderr, "Bad thread\n");
            exit(5);
        }
        Cargs[i].type = SliceType(rand() % 4); // random consumer type
        Cargs[i].numSlices = (rand() % 5) + 1; // add between 1 and 4 slices & 1 consumer of each type

        if (pthread_create(consIds + i, NULL, consProcess, Cargs + i) != 0) {
            fprintf(stderr, "Bad thread\n");
            exit(5);
        }
    }

    while (chrono::steady_clock::now() - start < chrono::seconds(20)) {} // run for 15s
    BUFFET.close(); //close buffet
    for (int i = 0; i < numThreads; i++) { // wait for all running children
        pthread_join(prodIds[i], NULL);
        pthread_join(consIds[i], NULL);
    }
    delete [] Cargs;
    delete [] Pargs;
    return 0;
}
