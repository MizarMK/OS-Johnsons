/***********************************************
 * Author:     Malik Mohamedali
 * Assignment: Program 3
 * Class:       CSI 4337
 ************************************************/

import java.util.concurrent.ThreadLocalRandom;

/**
 * implements sample producer thread
 */
public class AddSlices implements Runnable{
    SliceType type;
    int numSlices;
    long endTime;
    Buffet buffet;
    public AddSlices(Buffet buffet, SliceType type, int numSlices, long end) {
        this.numSlices = numSlices;
        this.type = type;
        this.endTime = end;
        this.buffet = buffet;
    }
    @Override
    public void run() {
        while (System.currentTimeMillis() < endTime) {
            if ((System.currentTimeMillis() / 1000) % 2 == 0) { // every 2s, try to add slice
                int rand = ThreadLocalRandom.current().nextInt(1,101); // 1% chance to add slice
                if (rand == 1)
                    buffet.AddPizza(numSlices, type);
            }
        }
    }
}
