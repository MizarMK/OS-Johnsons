/***********************************************
 * Author:     Malik Mohamedali 
 * Assignment: Program 2  
 * Class:       CSI 4337 
************************************************/
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include "read.c"

long long CURRENT_MAX = INT_MIN;
int START_NDX = 0;
int END_NDX = 0;
sem_t sumlock;
bool PROCLIST = true;

/**
 * Finds largest contiguous sum of series of integers
 * @param argc number of arguments
 * @param b arguments in char* format
 * @returns 0
 */
int main(int argc, char *argv[]) {
    if (argc != 3 && argc != 2) {
         fprintf( stderr,  "Usage: ./seqt <Number of Processes> <Filename (opt)>\n");
         exit(1);
     } 
    int processes = atoi(argv[1]);
    int *array, bytes;
    if (!isNumber(argv[1]) || processes < 1) {
        fprintf( stderr, "Invalid Process Input <Must be positive number>\n");
	exit(2);
    }

    char *fname;
    if (argc == 3) {
        fname = argv[2];
    }
    else {
        fname = "";
    }
    pthread_t tids[processes];
    int i;
    
    bytes = readFile(fname, &array, argc);
    struct threadArgs *args = malloc(sizeof(struct threadArgs) * (size_t)processes);
    sem_init(&sumlock, 0, 1);

    for (i = 0; i < processes; i++) { // create child threads with arguments passed through void* struct
	args[i].partition = i;
	args[i].processes = processes;
	args[i].bytes = bytes;
	args[i].array = array;
	args[i].PROCLIST = PROCLIST;
        if (pthread_create(tids + i, NULL, childProcess, args + i) != 0) { // argument format is as assigned above
	    fprintf(stderr, "Bad thread\n");
            exit(5);
	}
    }
    for (i = 0; i < processes; i++){
	 pthread_join(tids[i], NULL);
    }

    free(array);
    free(args);
    printf("Maximum Sum: %lld [%d,%d]\n", CURRENT_MAX, START_NDX, END_NDX);
    return 0;
}
