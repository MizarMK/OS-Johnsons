/***********************************************
 * Author:     Malik Mohamedali
 * Assignment: Program 3
 * Class:       CSI 4337
************************************************/
import java.util.concurrent.ThreadLocalRandom;

/**
 * implements sample consumer thread
 */
public class TakeSlices implements Runnable {
    SliceType type;
    int numSlices;
    long endTime;
    Buffet buffet;
    public TakeSlices(Buffet buffet, SliceType type, int numSlices, long end) {
        this.numSlices = numSlices;
        this.type = type;
        this.endTime = end;
        this.buffet = buffet;
    }
    @Override
    public void run() {
        while (System.currentTimeMillis() < endTime) {
            if ((System.currentTimeMillis() / 1000) % 1 == 0) {
                if (type.equals(SliceType.Cheese) || type.equals(SliceType.Veggie)) {
                    System.out.println("Done eating: " + buffet.TakeVeg(numSlices));
                }
                else
                    System.out.println("Done eating: " + buffet.TakeAny(numSlices));
            }
        }
    }
}
