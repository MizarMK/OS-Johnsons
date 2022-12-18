import java.lang.reflect.InvocationTargetException;
import java.util.List;

/**
 * Test: Verify a non-veg patron can take veg slices 
 * if they get there first.
 * 
 * @version 1.0
 */
public class TestB {
  /**
   * Laurie tries to take two slices of anything
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable laurie(Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        Util.sleep(2);
        Util.printSlices("Laurie", buffet.TakeAny(2));
      }
    };
  }

  /**
   * Ladonna gets to the buffet and wants two veg slices, but laurie already
   * took one of the veg slices
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable ladonna(Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        Util.sleep(3);
        Util.printSlices("Ladonna", buffet.TakeVeg(2));
      }
    };
  }

  /**
   * Trent puts slices on the buffet, gradually
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable trent(Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        Util.sleep(1);
        Util.printAdd("Trent", 1, SliceType.Veggie, buffet.AddPizza(1, SliceType.Veggie));
        Util.printAdd("Trent", 1, SliceType.Meat, buffet.AddPizza(1, SliceType.Meat));

        Util.sleep(3);
        Util.printAdd("Trent", 1, SliceType.Cheese, buffet.AddPizza(1, SliceType.Cheese));

        Util.sleep(1);
        Util.printAdd("Trent", 1, SliceType.Works, buffet.AddPizza(1, SliceType.Works));
      }
    };
  }

  /**
   * Create, run, and join actor threads
   */
  public static void main(String[] args) throws NumberFormatException, NoSuchMethodException, SecurityException, ClassNotFoundException, InstantiationException, IllegalAccessException, IllegalArgumentException, InvocationTargetException {
    if (args.length != 1) {
      throw new IllegalArgumentException("Parameter(s): <Buffet Class>");
    }
    // Initialize the buffet our threads are using
    Buffet buffet = Util.getBuffet(args[0], 20);

    // Make a few actor threads
    List<Thread> threads = List.of(new Thread(ladonna(buffet)),
        new Thread(laurie(buffet)), new Thread(trent(buffet)));

    threads.stream().forEach(Thread::start);

    Util.sleep(6);

    // ladonna will be stuck, since there's only one veg slice left
    buffet.close();

    // When we're done, join with all the threads
    threads.stream().forEach(t -> {
      try {
        t.join();
      } catch (InterruptedException e) {}});
  }
}
