import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

public class BuffetReentrantLock implements Buffet{
    private static final int MAXSLICES = 20;

    /** max number of slices in buffet */
    int buffetMax;

    /** list of pizza slice types*/

    ArrayList<SliceType> buffet;

    /** boolean dictating if buffet has closed */

    boolean buffetClosed;

    /** mutex lock for all methods w/ various conditions associated */
    ReentrantLock lock;

    /** cond var for blocking consumer if it can't get enough slices*/
    Condition consumerBlock;

    /** cond var for blocking producer buffet full */
    Condition buffetFull;

    /** cond var for blocking producer buffet empty */
    Condition buffetEmpty;

    /** integer conveying how many vegetarians are currently waiting to take slices */
    int vegetariansWaiting;

    /**
     * create buffet monitor obj
     */
    public BuffetReentrantLock() {
        buffetMax = MAXSLICES;
        buffet = new ArrayList<>();
        buffetClosed = false;
        vegetariansWaiting = 0;


        lock = new ReentrantLock();

        buffetFull = lock.newCondition();
        buffetEmpty = lock.newCondition();
        consumerBlock = lock.newCondition();
    }

    /**
     * create buffetMonitor obj with a requested max slices size
     * @param maxSlices max number of Slices in buffet
     */
    public BuffetReentrantLock(int maxSlices) {
        this();
        if (maxSlices < 0)
            throw new IllegalArgumentException("maxSlices Input MUST be > 0");
        buffetMax = maxSlices;
    }

    @Override
    public boolean AddPizza(int count, SliceType stype) {
        if (count <= 0)
            throw new IllegalArgumentException("count Input MUST be > 0");
        Objects.requireNonNull(stype);
        boolean acquired = false;

        try {
            if (!this.buffetClosed) {
                lock.lock();
                acquired = true;
                for (int i = 0; i < count; i++) {
                    while (buffet.size() >= buffetMax) {
                        if (!buffet.isEmpty()) { // only notify others if you've made changes
                            consumerBlock.signal();
                            buffetEmpty.signal();
                        }
                        buffetFull.await();
                    }
                    if (this.buffetClosed) {
                        buffetFull.signal();
                        buffetEmpty.signal();
                        return false;
                    }
                    this.buffet.add(stype);
                }
                consumerBlock.signal();
                buffetEmpty.signal();
                return true;
            }
        } catch (InterruptedException e) {
            System.err.println("Error adding pizza to buffet");
            e.printStackTrace();
        } finally {
            if (acquired)
                lock.unlock();
        }
        lock.lock();
        consumerBlock.signalAll();
        lock.unlock();
        return false;
    }

    @Override
    public void close() {
        buffetClosed = true; // have all idling threads in other method return false via dependency on buffetStatus
        AddPizza(1, SliceType.Cheese);
    }

    @Override
    public List<SliceType> TakeAny(int desired) {
        return getSliceTypes(desired, 0);
    }

    /**
     * This method is like takeAny(), but it returns the oldest
     * vegetarian slices in the slice list. As with takeAny(),
     * it will block the caller until they can have all
     * the slices they want.  Once the buffet is closed, this method
     * will throw an exception.
     * \param desired number of desired slices
     * \return list of slices
     * \throws invalid_argument if desired < 0 or desired > maxSlices
     * \throws runtime_error if buffet is closed
     */
    @Override
    public synchronized List<SliceType> TakeVeg(int desired) {
        vegetariansWaiting++;
        return getSliceTypes(desired, 1);
    }

    /**
     * gets slicetypes from specific indexes, clears buffet, and unlocks mutex
     * @param desired number of slices wanted
     * @param isVeg integer showing what type of slices the consumer wants (0: all, 1: vegetarian, 2: meat/works
     * @return list of slices to consumer thread
     */
    private List<SliceType> getSliceTypes(int desired, int isVeg) {
        try {
            if (desired < 0 || desired > this.buffetMax)
                throw new IllegalArgumentException("count Input MUST be > 0");
            if (this.buffetClosed)
                return null;

            if (vegetariansWaiting > 0 && isVeg == 0) // if vegetarians are waiting, only take meat pizzas
                isVeg = 2;

            ArrayList<SliceType> plate = new ArrayList<>();       // slices being atomically taken
            ArrayList<Integer> sliceIndexes = new ArrayList<>();  // index of slices to be taken

            lock.lock();
            while (sliceIndexes.size() < desired && !this.buffetClosed) {// clear plate and the indexes the thread is watching
                while (buffet.isEmpty())
                    buffetEmpty.await();
                if (this.buffetClosed) {
                    buffetFull.signal();
                    consumerBlock.signal();
                    return null;
                }
                sliceIndexes.clear();
                plate.clear();
                for (int i = 0; i < buffet.size() && plate.size() < desired; i++) {
                    if (isVeg == 0) {
                        sliceIndexes.add(i); // add the index and the pizza to the plate; removed if non-atomic
                        plate.add(this.buffet.get(i));
                    }
                    else if (isVeg == 1) {
                        if (this.buffet.get(i).equals(SliceType.Cheese) || this.buffet.get(i).equals(SliceType.Veggie)) {
                            sliceIndexes.add(i);
                            plate.add(this.buffet.get(i));
                        }
                    }
                    else if (isVeg == 2) {
                        if (this.buffet.get(i).equals(SliceType.Meat) || this.buffet.get(i).equals(SliceType.Works)) {
                            sliceIndexes.add(i);
                            plate.add(this.buffet.get(i));
                        }
                    }
                }
                if (sliceIndexes.size() < desired) { // let other threads go if this one can't
                    consumerBlock.await();
                }
            }
            if (isVeg == 1)
                vegetariansWaiting--;
            if (buffetClosed)
                return null;
            return getSliceTypes(plate, sliceIndexes);
        } catch (InterruptedException e) {
            System.err.println("Error taking pizza slices");
            e.printStackTrace();
        } finally {
            if (lock.isHeldByCurrentThread())
                lock.unlock();
        }
        return null;
    }

    /**
     * gets slicetypes from specific indexes, clears buffet, and notifies waiting threads
     * @param plate vector to add slices to
     * @param sliceIndexes indexes of slices to add
     * @return list of slices to consumer thread
     */
    private List<SliceType> getSliceTypes(ArrayList<SliceType> plate, ArrayList<Integer> sliceIndexes) {
        for (int i = sliceIndexes.size() - 1; i >= 0; i--) {
            this.buffet.remove((int) sliceIndexes.get(i));
        }
        //System.out.println("Done eating (" + plate + ")!");
        buffetFull.signal();
        return plate;
    }
}
