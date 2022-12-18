/***********************************************
 * Author:     Malik Mohamedali
 * Assignment: Program 3
 * Class:       CSI 4337
 ************************************************/

import java.util.concurrent.*;

/**
 * "randomly" tests activity of consumer and producer threads with various habits
 */
public class TestDriver {
    /**
     * mandate thready activity and exit after X seconds
     * @param args none
     * @throws InterruptedException if thread in threadpool cannot wait
     */
    public static void main(String[] args) throws InterruptedException {
        final Buffet buffet = new BuffetSemaphore();
        // final Buffet buffet = new BuffetMonitor();
        //final Buffet buffet = new BuffetReentrantLock();

        int numProd = 20; // assign prods and consumers
        int numCons = 40;
        ThreadPoolExecutor prodThreads = (ThreadPoolExecutor) Executors.newFixedThreadPool(numProd);
        ThreadPoolExecutor consThreads = (ThreadPoolExecutor) Executors.newFixedThreadPool(numCons);

        long start = System.currentTimeMillis();
        long end = start + (10 * 1000); /// end after 10s of execution
        prodThreads.setKeepAliveTime(1, TimeUnit.SECONDS);
        consThreads.setKeepAliveTime(1, TimeUnit.SECONDS);
        prodThreads.allowCoreThreadTimeOut(true);
        consThreads.allowCoreThreadTimeOut(true);

        for (int i = 0; i < numProd; i++) { // initialize all producer threads
            // execute random producer thread
            SliceType type = SliceType.values()[ThreadLocalRandom.current().nextInt(0,SliceType.values().length)];
            int numSlices = ThreadLocalRandom.current().nextInt(1,2); // try and add 1 slices
            prodThreads.execute(new AddSlices(buffet, type, numSlices, end - 1000)); /// FIXME implement runnable to mandate call to buffet methods
        }
        for (int i = 0; i < numCons; i++) { // initialize all consumer threads
            // execute random consumer thread
            SliceType type = SliceType.values()[ThreadLocalRandom.current().nextInt(0,SliceType.values().length)];
            int numSlices = ThreadLocalRandom.current().nextInt(1,6); // try and take between 1 and 5 slices wanted
            consThreads.execute(new TakeSlices(buffet, type, numSlices, end - 1000)); /// FIXME implement runnable to mandate call to buffet methods
        }
        while (System.currentTimeMillis() < end); // allow prod/cons threads to execute for 30s before closing
        buffet.close();
        // Thread.sleep(2000); // delay main so child threads finish
    }
}
