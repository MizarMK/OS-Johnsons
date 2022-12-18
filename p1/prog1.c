/***********************************************
 * Author:     Malik Mohamedali 
 * Assignment: Program 1  
 * Class:       CSI 4337 
************************************************/

#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <limits.h>

#include "read.c"

bool PROCLIST = true;
sem_t sumlock;
long long CURRENT_MAX = INT_MIN;
int START_NDX = 0;
int END_NDX = 0;
/**
 * Finds largest contiguous sum of series of integers
 * @param argc number of arguments
 * @param b arguments in char* format
 * @returns 0
 */
int main(int argc, char *argv[]) {
    if (argc != 3 && argc != 2) {
         fprintf( stderr,  "Usage: ./pairs <Number of Processes> <Filename>\n");
         exit(1);
     } 
    int i, processes = atoi(argv[1]);
    int *array, bytes;
    if (!isNumber(argv[1]) || processes < 1) {
        fprintf(stderr, "Invalid Process Input <Must be positive number>\n");
        exit(2);
    }
    char *fname;
    if (argc == 3) {
        fname = argv[2];
    }
    else {
        fname = "";
    }

    
    pid_t pid;
    int status, fds[2], s_ndx, e_ndx;
    long long num;
    
    
    bytes = readFile(fname, &array, argc);
 
    pid = getpid();
    pipe(fds);

    for (i = 0; i < processes && pid != 0; i++) {
        pid = fork();
    }


    if (pid == 0) {
        int partition = i-1;
        close(fds[0]);
        childProc(fds, partition, processes, bytes, array, PROCLIST);
    	return 0;
    }
    else {
        for (i = 0; i < processes; i++) {
            close(fds[1]);
        }
        for (i = 0; i < processes; i++) {
            pid = wait(&status);
            read(fds[0], &s_ndx, sizeof(int));
	        read(fds[0], &e_ndx, sizeof(int));
	        read(fds[0], &num, sizeof(long long));
            if (CURRENT_MAX < num) {
                CURRENT_MAX = num;
		    START_NDX = s_ndx;
		    END_NDX = e_ndx;
            }
        }
        printf("Maximum Sum: %lld [%d,%d]\n", CURRENT_MAX, START_NDX, END_NDX);
        close(fds[0]);
        free(array);
    }
    return 0;
}
