/***********************************************
 * Author:     Malik Mohamedali
 * Assignment: Program 2
 * Class:       CSI 4337
************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <semaphore.h>

extern sem_t sumlock;         // semaphore lock for threads
int MAXBYTES = 1000000;       // maximum number of items in file
extern long long CURRENT_MAX; // global var for current max calculated from thread
extern int START_NDX;         // start index for current max
extern int END_NDX;           // end index for current max

/**
 * Struct for containing child thread arguments
 * partition - assignment for workload in calculating contiguous sum
 * processes - number of child processes working on array
 * array - list of integers read from file
 * PROCLIST - boolean dictating whether sum for each thread process is printed or not
 */
struct threadArgs {
    int partition;
    int processes;
    int bytes;
    int *array;
    bool PROCLIST;
};

/**
 * Finds if a character array is numeric
 * @param number character array of numbers (and potentially non-numeric chars)
 * @returns true if the char* contains only digits, false otherwise
 */
bool isNumber(char number[]) {
    int i = 0;

    // checking for negative numbers
    if (number[0] == '-')
        i = 1;
    for (; number[i] != 0; i++) {
    // if (number[i] > '9' || number[i] < '0')
        if (!isdigit(number[i]))
            return false;
    }
    return true;
}

/**
 * Sums contiguous series of numbers in array
 * @param a array of integers
 * @param start starting index
 * @param end ending index
 * @return sum of contiguous series
 */
long long* sumRange (int *a, int start, int end) {
    long sum = 0;
    static long long maxSum [] = {INT_MIN, 0, 0};
    int i;
    for (i = start; i <= end; i++) {
        if (a[i] < INT_MIN || a[i] > INT_MAX) {
            return maxSum;
        }
        else {
            sum += a[i];
            if (maxSum[0] < sum) {
                maxSum[0] = sum;
                maxSum[1] = start;
                maxSum[2] = i;
            }
        }
    }
    return maxSum;
}

/**
 * Sums contiguous series of numbers in array
 *  @param a array of integers
 *  @param start starting index
 *  @param end ending index
 * @return sum of contiguous series
 * 
 */
void threadSum(int *a, int start, int end, long long** ms) {
    long sum = 0;
    int i;
    for (i = start; i <= end; i++) {
        if (a[i] < INT_MIN || a[i] > INT_MAX) {
            return ms;
        }
        else {
            sum += a[i];
            if ((*ms)[0] < sum) {
                (*ms)[0] = sum;
                (*ms)[1] = start;
                (*ms)[2] = i;
            }
        }
    }
}

/**
 * Calculates Maximum sum of a number of subsets for a contiguous array sequence
 * @param a integer array
 * @param start_ndx starting index of this partition's subset to work in
 * @param end_ndx ending index of this partition's subset to work on
 * @param range number of items in this partitions subset to work on
 * @param len length of integer array
 * @param p partition
 * @param proc boolean that designates if process output is written
 */
void maxSumCalc(int *a, int *dividedSubsets, int range, int len, int p, int fds[2], bool proc) {
    int i;
    long long maxSum [] = {INT_MIN, 0 , 0};
    long long *ms = malloc(3 * sizeof(long long));
    ms[0] = INT_MIN;
    ms[1] = 0;
    ms[2] = 0;
    for (i = range; i < (range + dividedSubsets[p]); i++) { ///define start index for range that child proc may add sums
	if (fds[0] != -1) {
	    long long *curSum = sumRange(a, i, len);
            if (curSum[0] > maxSum[0]) {
                maxSum[0] = curSum[0];
                maxSum[1] = curSum[1];
                maxSum[2] = curSum[2];
            }
	}
	else {
	    threadSum(a, i, len, &ms);
	    if (ms[0] > maxSum[0]) {
                maxSum[0] = ms[0];
                maxSum[1] = ms[1];
                maxSum[2] = ms[2];
            }   
	}
    }
    int k = (int)maxSum[1];
    int j = (int)maxSum[2];
    free(ms);
    if (proc) {
        printf("Process %d Sum: %lld [%d,%d]\n", p, maxSum[0], k, j);
    }
    if (fds[0] == -1) {
        sem_post(&sumlock); // mutex for print
    }
    if (fds[0] == -1) {
        sem_wait(&sumlock); // mutex for print
        if (CURRENT_MAX < maxSum[0]) {
            CURRENT_MAX = maxSum[0];
            START_NDX = k;
            END_NDX = j;
        }
        sem_post(&sumlock);
        pthread_exit(NULL); // exit thread
    }
    else {
        lockf(fds, F_LOCK, 0);
	write(fds[1], &k, sizeof(int));
	write(fds[1], &j, sizeof(int));
	write(fds[1], &maxSum, sizeof(long long));
	lockf(fds, F_ULOCK, 0); 
    }
}

/**
 * Calculate what subset of the contiguous series the child process is designated to work on
 * @param bytes number of values in array
 * @param partition partition in subsets
 * @param numProcs number of child processes splitting work
 * @param array integer array
 * @param proc boolean that designates if process output is written
 */
void readCalc(int bytes, int partition, int numProcs, int *array, int fds[2], bool proc) { // 4, 2, 3, [1,2,3,4]
    int len = bytes-1;
    int dividedSubsets[numProcs]; // [1, 2, 3, 4]
    int i, range;



    for (i = 0; i < numProcs; i++) {
        dividedSubsets[i] = bytes / numProcs; /// initialize array defining how many subsets each process works on
    }
    for (i = 0; i < bytes % numProcs; i++) {
        dividedSubsets[i]++; /// find division of workload
    }

    range = 0;
    for (i = 0; i < partition; i++) {
        range += dividedSubsets[partition]; /// start index
    }

    if (dividedSubsets[partition] < 1 && proc) {
        printf("Process %d Sum: N/A\n", partition);
    }
    else {
        maxSumCalc(array, dividedSubsets, range, len, partition, fds, proc);
    }
}

/**
 * Read File of integer input or from stdin
 * @param fname filename
 * @param array pointer to array of integers
 * @param argc number of args
 * @return number of bytes read from file (or stdin)
 */
int readFile (char *fname, int **array, int argc) {
    int i = 0, c = 0, args = 0;
    char num[11];
    if (argc > 2) {
    FILE* fp;
        if (!(fp = fopen(fname, "r"))) {
	    fprintf(stderr, "File failed to open; exiting");
	    exit(3);
        }
        *array = malloc(MAXBYTES * (size_t)sizeof(int));
        while (fscanf(fp, "%s", &num) != EOF) {
            //read \n (automatic with fscanf)
            if (!isNumber(num)) {
                fprintf(stderr, "Bad Input; exiting...\n");
                exit(4);
            }
            i = atoi(num);
	        (*array)[c] = i;
	        c++;
        }
        if (c < 1) { 
	    fprintf(stderr, "File was empty; exiting\n");
	    exit(3);
        }
        fclose (fp);
    }
    else if (argc == 2) {
        printf("Enter integers in array (CTRL-D To end input after newline):\n");
        *array = malloc(MAXBYTES * (size_t)sizeof(int));
        // fread(&b, sizeof(char), 1, stdin); //read \n
        if (args < 1) {
	    fprintf(stderr, "Bad Input; exiting");
	    exit(4);
	}
        (*array)[c] = i;
        c++;
	if (c < 1) {
            fprintf(stderr, "No Input; exiting\n");
            exit(3);
        }
    }
    return c;
}

/**
 * Main call from child process in main that calls methods for calculating max contiguous sum for a child's subset
 * @param args - contains arguments for specific child thread in struct form casted to void* object
 */
void *childProcess(void *args) {
    struct threadArgs tArgs = *((struct threadArgs *)args);
    int a[2] = {-1};
    readCalc(tArgs.bytes, tArgs.partition, tArgs.processes, tArgs.array, a, tArgs.PROCLIST);
    return NULL;   
}

/**
 * Main call from child process in main that calls methods for calculating max contiguous sum for a child's subset
 * @param fds pipe to write output to
 * @param partition partition
 * @param procs number of child processes
 * @param bytes number of integers in array
 * @param a integer array
 * @param proc boolean that designates if process output is written
 */
void childProc(int fds[2], int partition, int procs, int bytes, int *a, bool proc) {
    readCalc(bytes, partition, procs, a, fds, proc);
}
