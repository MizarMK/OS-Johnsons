/***********************************************
 * Author:     Malik Mohamedali
 * Assignment: Program 3
 * Class:       CSI 4337
 ************************************************/

import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.concurrent.Semaphore;

/**
 * implements buffet interface using semaphore mutex
 */
public class BuffetSemaphore implements Buffet {
    private static final int MAXSLICES = 20;
    /** max number of slices in buffet */
    int buffetMax;

    /** count number of veg slices on buffet **/
    int vegSlices;

    /** count number of idle vegetarian threads */
    int vegetariansWaiting;

    /** count requests for vegetarians that couldn't progress */
    ArrayList<Integer> vegRequests;

    /** count requests for omnivores that couldn't progress */
    ArrayList<Integer> omniRequests;

    /** list of pizza slice types*/
    ArrayList<SliceType> buffet;

    /** boolean dictating if buffet has closed */
    boolean buffetClosed;

    /** mutex lock for adding slices to buffet */
    Semaphore addState;

    /** mutex lock for removing slices from buffet */
    Semaphore buffetState;

    /** lock to prevent multiple vegetarians from seeking slices simultaneously */
    Semaphore vegMutex;

    /** lock to prevent multiple omnivores from seeking slices simultaneously */
    Semaphore omniMutex;

    /**
     * block for producers when buffet is full;
     */
    Semaphore fullBlock;

    /**
     * block for consumers when buffet is empty;
     */
    Semaphore emptyBlock;

    /**
     * block for omnivores that couldn't be satisfied
     */
    Semaphore omniBlock;

    /**
     * block for omnivores that couldn't be satisfied
     */
    Semaphore vegBlock;

    /**
     * create buffetSemaphore obj with a default max slices size
     */
    public BuffetSemaphore() {
        buffetMax = MAXSLICES;
        buffet = new ArrayList<>();
        buffetClosed = false;

        addState = new Semaphore(1);
        buffetState = new Semaphore(1);
        vegMutex = new Semaphore(1);
        omniMutex = new Semaphore(1);

        vegRequests = new ArrayList<>();
        omniRequests = new ArrayList<>();

        fullBlock = new Semaphore(0);
        emptyBlock = new Semaphore(0);
        omniBlock = new Semaphore(0);
        vegBlock = new Semaphore(0);

        vegetariansWaiting = 0;

    }

    /**
     * create buffetSemaphore obj with a requested max slices size
     * @param maxSlices max number of Slices in buffet
     */
    public BuffetSemaphore(int maxSlices) {
        this();
        if (maxSlices < 0)
            throw new IllegalArgumentException("maxSlices Input MUST be > 0");
        buffetMax = maxSlices;
    }

    private boolean satisfyVeg(int desired) {
        if (desired == -1) { // producer checking if it should free omni
            for (int i : vegRequests) { // vegs waiting, only count meat slices
                if (i <= vegSlices)
                    return true;
            }
        }
        else { // consumer checking if it should free itself
            return desired <= vegSlices;
        }
        return false;
    }

    private boolean satisfyOmni(int desired) {
        if (desired == -1) { // producer checking if it should free omni
            if (vegetariansWaiting <= 0) { // no vegs waiting, count all slices
                for (int i : omniRequests) {
                    if (i <= buffet.size()) {
                        return true;
                    }
                }
            } else {
                for (int i : omniRequests) { // vegs waiting, only count meat slices
                    if (i <= buffet.size() - vegSlices) {
                        return true;
                    }
                }
            }
        }
        else { // consumer checking if it should free itself
            if (vegetariansWaiting <= 0) { // no vegs waiting, count all slices
                return desired <= buffet.size();
            } else {
                return desired < buffet.size() - vegSlices;
            }
        }
        return false;
    }
    private void producerRelease() {
        if ((satisfyVeg(-1) && vegBlock.hasQueuedThreads()) || buffetClosed) { // let blocked vegs out if possible
            vegBlock.release();
        }
        else if ((satisfyOmni(-1) && omniBlock.hasQueuedThreads()) || buffetClosed) { // otherwise unblock omnis
            omniBlock.release();
        }
        while (!buffet.isEmpty() && emptyBlock.hasQueuedThreads()) // release all threads waiting on empty
            emptyBlock.release();
        vegMutex.release();
        omniMutex.release();
    }

    @Override
    public boolean AddPizza(int count, SliceType stype) {
        if (count <= 0)
            throw new IllegalArgumentException("count Input MUST be > 0");
        Objects.requireNonNull(stype);
        try {
            if (!this.buffetClosed) {
                addState.acquire();
                vegMutex.acquire();
                omniMutex.acquire();
                for (int i = 0; i < count; i++) {
                    if (this.buffet.size() == buffetMax && !this.buffetClosed) { // SpinLock for maximum Capacity reached
                        producerRelease();
                        fullBlock.acquire(); // wait here until signaled by consumer
                        vegMutex.acquire();
                        omniMutex.acquire();
                    }
                    if (this.buffetClosed) {
                        producerRelease();
                        addState.release();
                        return false;
                    }
                    this.buffet.add(stype);
                    if (stype.isVeg())
                        vegSlices++;
                }
                producerRelease();
                addState.release();
                return true;
            }
        } catch (InterruptedException e) {
            System.err.println("Error adding pizza to buffet");
            e.printStackTrace();
        }
        return false;
    }

    @Override
    public void close() {
        buffetClosed = true; // have all idling threads in other method return false via dependency on buffetStatus
        producerRelease();

        while (buffetState.availablePermits() <= 0) {
            buffetState.release();
        }
        while(vegMutex.availablePermits() <= 0) {
            vegMutex.release();
        }
        while(omniMutex.availablePermits() <= 0) {
            omniMutex.release();
        }
        while(addState.availablePermits() <= 0) {
            addState.release();
        }
    }

    @Override
    public List<SliceType> TakeAny(int desired) {
        try {
            if (desired <= 0 || desired > this.buffetMax)
                throw new IllegalArgumentException("count Input MUST be > 0");
            if (this.buffetClosed)
                return null;

            ArrayList<SliceType> plate = new ArrayList<>();       // slices being atomically taken
            ArrayList<Integer> sliceIndexes = new ArrayList<>();      // index of slices to be taken

            while (sliceIndexes.size() < desired && !this.buffetClosed) {
                if (buffet.isEmpty())
                    emptyBlock.acquire();
                omniMutex.acquire(); // lock omnivores and buffet take state
                buffetState.acquire();
                // System.out.println(buffet);

                plate.clear();       // clear plate and the indexes the thread is watching
                sliceIndexes.clear();

                for (int i = 0; i < buffet.size() && plate.size() < desired; i++) {
                    if (vegetariansWaiting <= 0) {
                        sliceIndexes.add(i); // add the index and the pizza to the plate; removed if non-atomic
                        plate.add(this.buffet.get(i));
                    }
                    else {
                        if (!this.buffet.get(i).isVeg()) {
                            sliceIndexes.add(i);
                            plate.add(this.buffet.get(i));
                        }
                    }
                }
                if (sliceIndexes.size() < desired) { // let other threads go if this one can't
                    boolean entry = true; // only allows threads woken by producer to wake other consumer threads
                    buffetState.release();
                    omniMutex.release();
                    omniRequests.add(desired); // couldn't be satisfied, leave unblocking to producers
                    do { // while can't be satisfied: release special mutex, then wait on it
                        if (omniBlock.hasQueuedThreads() && !entry)
                            omniBlock.release();
                        omniBlock.acquire();
                        entry = false;
                    } while (!satisfyOmni(desired) && !buffetClosed);
                    // System.out.println("(before) OmniRequests: " + omniRequests);
                    omniRequests.remove((Object)desired);
                    // System.out.println("Omnivore re-counting slices now...");
                    // System.out.println("(after) OmniRequests: " + omniRequests);
                }
            }
            if (this.buffetClosed) {
                // System.out.println("Omnivore failed to eat :(");
                return null;
            }
            return getSliceTypes(plate, sliceIndexes, omniMutex);

        } catch (InterruptedException e) {
            System.err.println("Error taking (any) pizza slices");
            e.printStackTrace();
        }
        return null;
    }

    @Override
    public List<SliceType> TakeVeg(int desired) {
        try {
            if (desired <= 0 || desired > this.buffetMax)
                throw new IllegalArgumentException("count Input MUST be > 0");
            if (this.buffetClosed)
                return null;

            buffetState.acquire();
            vegetariansWaiting++; // mutex increase veg idle count
            buffetState.release();

            ArrayList<SliceType> plate = new ArrayList<>();       // slices being atomically taken
            ArrayList<Integer> sliceIndexes = new ArrayList<>();      // index of slices to be taken

            while(sliceIndexes.size() < desired && !this.buffetClosed) {
                if (buffet.isEmpty())
                    emptyBlock.acquire();
                vegMutex.acquire();
                buffetState.acquire();
                // System.out.println(".");

                plate.clear();
                sliceIndexes.clear();

                for (int i = 0; i < buffet.size() && plate.size() < desired; i++) {
                    if (this.buffet.get(i).isVeg()) {
                        sliceIndexes.add(i);
                        plate.add(this.buffet.get(i));
                    }
                }
                if (sliceIndexes.size() < desired) {
                    boolean entry = true;
                    buffetState.release();
                    vegMutex.release();
                    vegRequests.add(desired); // couldn't be satisfied, leave unblocking to producers
                    do { // while can't be satisfied: release special mutex, then wait on it
                        if (vegBlock.hasQueuedThreads() && !entry)
                            vegBlock.release();
                        vegBlock.acquire();
                        entry = false;
                    } while (!satisfyVeg(desired) && !buffetClosed);
                    vegRequests.remove((Object)desired);
                }
            }
            vegetariansWaiting--;
            if (this.buffetClosed)
                return null;
            return getSliceTypes(plate, sliceIndexes, vegMutex);

        } catch (InterruptedException e) {
            System.err.println("Error taking (veg) pizza slices");
            e.printStackTrace();
        }
        return null;
    }

    /**
     * gets slicetypes from specific indexes, clears buffet, and unlocks mutex
     * @param plate vector to add slices to
     * @param sliceIndexes indexes of slices to add
     * @param sem mutex to clear
     * @return list of slices to consumer thread
     */
    private List<SliceType> getSliceTypes(ArrayList<SliceType> plate, ArrayList<Integer> sliceIndexes, Semaphore sem) {
        for (int i = sliceIndexes.size() - 1; i >= 0; i--) {
            if (buffet.get(sliceIndexes.get(i)).isVeg())
                vegSlices--;
            buffet.remove((int) sliceIndexes.get(i));
            // System.out.println(buffet);
        }
        buffetState.release();
        if (!buffet.isEmpty())
            emptyBlock.release();
        fullBlock.release();
        sem.release();
        return plate;
    }
}
