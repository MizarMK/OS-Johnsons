/***********************************************
 * Author:     Malik Mohamedali
 * Assignment: Program 3
 * Class:       CSI 4337
 ************************************************/

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

/**
 * implements buffet interface using monitor mutex
 */
public class BuffetMonitor implements Buffet{
    private static final int MAXSLICES = 20;

    /** max number of slices in buffet */
    int buffetMax;

    /** list of pizza slice types*/

    ArrayList<SliceType> buffet;

    /** boolean dictating if buffet has closed */

    boolean buffetClosed;

    /** counts active producers so they can be freed if waiting when buffet closes*/
    int producers;

    /** integer conveying how many vegetarians are currently waiting to take slices */
    int vegetariansWaiting;

    /**
     * create buffet monitor obj
     */
    public BuffetMonitor() {
        buffetMax = MAXSLICES;
        buffet = new ArrayList<>();
        buffetClosed = false;
        vegetariansWaiting = 0;
        producers = 0;
    }

    /**
     * create buffetMonitor obj with a requested max slices size
     * @param maxSlices max number of Slices in buffet
     */
    public BuffetMonitor(int maxSlices) {
        this();
        if (maxSlices < 0)
            throw new IllegalArgumentException("maxSlices Input MUST be > 0");
        buffetMax = maxSlices;
    }

    @Override
    public synchronized boolean AddPizza(int count, SliceType stype) {
        if (count <= 0)
            throw new IllegalArgumentException("count Input MUST be > 0");
        Objects.requireNonNull(stype);
        boolean addedSlices = false;

        try {
            if (!this.buffetClosed) {
                producers++;
                for (int i = 0; i < count; i++) {
                    if (this.buffet.size() == buffetMax && !this.buffetClosed) {
                        if (addedSlices) { // only notify others if you've made changes
                            notifyAll();
                            addedSlices = false;
                        }
                        wait();
                    }
                    if (this.buffetClosed) {
                        notifyAll();
                        producers--;
                        return false;
                    }
                    if (this.buffet.size() < buffetMax) {
                        addedSlices = true;
                        this.buffet.add(stype);
                    }
                    else
                        i--;
                }
                notifyAll();
                producers--;
                return true;
            }
        } catch (InterruptedException e) {
            System.err.println("Error adding pizza to buffet");
            e.printStackTrace();
        }
        notifyAll();
        return false;
    }

    @Override
    public void close() {
        buffetClosed = true; // have all idling threads in other method return false via dependency on buffetStatus
        AddPizza(1, SliceType.Cheese); // irrelevant call to notifyAll()
    }

    @Override
    public synchronized List<SliceType> TakeAny(int desired) {
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

            while (sliceIndexes.size() < desired && !this.buffetClosed) {// clear plate and the indexes the thread is watching
                sliceIndexes.clear(); plate.clear();
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
                    notifyAll();
                    wait();
                }
            }
            if (this.buffetClosed)
                return null;
            if (isVeg == 1)
                vegetariansWaiting--;
            return getSliceTypes(plate, sliceIndexes);
        } catch (InterruptedException e) {
            System.err.println("Error taking pizza slices");
            e.printStackTrace();
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
        try {
            for (int i = sliceIndexes.size() - 1; i >= 0; i--) {
                this.buffet.remove((int) sliceIndexes.get(i));
            }
            //System.out.println("Done eating (" + plate + ")!");
            return plate;
        } finally {
            notifyAll();
        }
    }
}
