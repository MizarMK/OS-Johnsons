/**
 * Author: Malik Mohamedali
 * Class: CSI 4337
 * Version: V1.0
 * Date: 9.18.2022
 * Copyright notice
 */

import java.util.ArrayList;

/**
 * ChildThread is a thread runnable class that handles the work for calculating what subsections of the array a thread
 * may work on based on the partition passed to it and calculation of the max contiguous sum from the subsections it
 * is delegated
 */
public class ChildThread implements Runnable {
    /**
     * number of integers read from file in main
     *
     */
    private final int bytes;

    /**
     * represent process number and the partition of the data the thread may work on
     */
    private final int partition;

    /**
     * number of child processes working on array of integers
     */
    private final int processes;

    /**
     * list of integers read from file
     */
    private final ArrayList<Integer> array;

    /**
     * boolean dictating whether sum for each thread process is printed or not
     */
    private final boolean proclist;

    /**
     * initializes thread with thread-specific vars
     * @param bytes - number of integers in list
     * @param partition - partition
     * @param processes - number of child processes
     * @param array - list of integers
     * @param PROCLIST - dictates whether to print thread calculation or not
     */
    public ChildThread(int bytes, int partition, int processes, ArrayList<Integer> array, boolean PROCLIST) {
        this.bytes = bytes;
        this.partition = partition;
        this.processes = processes;
        this.array = array;
        this.proclist = PROCLIST;
    }

    /**
     * run Thread (inherited from Runnable)
     */
    @Override
    public void run() {
        readCalc(this.bytes, this.partition, this.processes, this.array, this.proclist);
    }

    /**
     * Sums contiguous series of numbers in array
     * @param a array of integers
     * @param start starting index
     * @param end ending index
     * @return sum of contiguous series
     */
    public long [] sumRange (ArrayList<Integer> a, int start, int end) {
        long sum = 0;
        long [] maxSum = new long[] {a.get(0), 0, 0};
        int i;
        for (i = start; i <= end; i++) {
            if (a.get(i) < Integer.MIN_VALUE || a.get(i) > Integer.MAX_VALUE) {
                return maxSum;
            }
            else {
                sum += a.get(i);
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
     * Method called in thread-safe context to update current maximum sum calculated by threads and corresponding
     * indexes
     * @param maxSum sum calculated by this thread
     * @param i start index for maxSum
     * @param k end index for maxSum
     */
    public void SetMax(long maxSum, int i, int k) {
        Seq.CURRENT_MAX = maxSum;
        Seq.START_NDX = i;
        Seq.END_NDX = k;
    }

    /**
     * Calculates Maximum sum of a number of subsets for a contiguous array sequence
     * @param a integer array
     * @param dividedSubsets array containing number of starting indexes the process works with
     * @param range start index of this partition
     * @param len length of integer array
     * @param p partition
     * @param proc boolean that designates if process output is written
     */
    public void maxSumCalc(ArrayList<Integer> a, int [] dividedSubsets, int range, int len, int p, boolean proc) {
        int i;
        long [] maxSum = new long[] {Integer.MIN_VALUE, 0 , 0};
        for (i = range; i < (range + dividedSubsets[p]); i++) { /// define start index for range that child proc may add sums
            long [] curSum = sumRange(a, i, len);
            if (curSum.length != 3) {
                System.err.println("Error during process calculation of max");
                System.exit(6);
            }
            if (curSum[0] > maxSum[0]) {
                maxSum[0] = curSum[0];
                maxSum[1] = curSum[1];
                maxSum[2] = curSum[2];
            }
        }

        if (proc) {
            System.out.println("Process " + p + " Sum: " + maxSum[0]
                    + "[" + maxSum[1] + "," + maxSum[2] + "]");
        }
        synchronized (ChildThread.class) {
            if (Seq.CURRENT_MAX < maxSum[0]) { // mutex reset max in main prog
                SetMax(maxSum[0], (int) maxSum[1], (int) maxSum[2]);
            }
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
    void readCalc(int bytes, int partition, int numProcs, ArrayList<Integer> array, boolean proc) {
        int len = bytes-1;
        int [] dividedSubsets = new int[numProcs]; // [1, 2, 3, 4]
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
            System.out.println("Process " + partition + " Sum: N/A");
        }
        else {
            maxSumCalc(array, dividedSubsets, range, len, partition, proc);
        }
    }
}
