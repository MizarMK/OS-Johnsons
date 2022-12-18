#include <iostream>

void *memReport();

using namespace std;

int main() {
    int *a = (int *) malloc(5 * sizeof(int));
    //cout << "allocated 5 bytes..." << endl;
    int *b = (int *) malloc(5 * sizeof(int));
    //cout << "allocated 5 bytes..." << endl;
    int *c = (int *) malloc(6 * sizeof(int));
    //cout << "allocated 6 bytes..." << endl << endl;
    
    memReport();
	
    free(a);
    free(c);

    memReport();

    //cout << "freed 5 & 6 bytes..." << endl << endl;
    int *d = (int *) malloc(4 * sizeof(int));
    free(d);
    //cout << "allocated and freed 4 bytes..." << endl;
    int *e = (int *) malloc(5 * sizeof(int));
    //cout << "allocated 5 bytes..." << endl;
    int *f = (int *) malloc(1 * sizeof(int));
    //cout << "allocated 1 byte(s)..." << endl;
    free(f);
    //cout << "freed 1 byte..." << endl;
    int *g = (int *) malloc(5 * sizeof(int));
    //cout << "allocated 5 bytes..." << endl << endl;

    memReport();

    free(e);
    free(g);
    free(b);

    memReport();

    int *x = (int *) malloc(5 * sizeof(int));
    int *y = (int *) malloc(5 * sizeof(int));
    int *z = (int *) malloc(5 * sizeof(int));
    free(y);
    y = (int *) malloc(1 * sizeof(int));

    memReport();
   
    free(x);
    free(y);
    free(z);

    memReport();
	
    return 0;
}
