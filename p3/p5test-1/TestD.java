import java.lang.reflect.InvocationTargetException;
import java.util.List;

/**
 * Test:  Verify patrons only take slices when they 
 * can get all the slices they want.
 * 
 * @version 1.0
 */
public class TestD {
  /**
   * Sade gets there first and starts adding slices
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable sade(Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        // One slice, which Dorothea should get.
        Util.sleep(1);
        Util.printAdd("Sade", 1, SliceType.Cheese, buffet.AddPizza(1, SliceType.Cheese));
        Util.printAdd("Sade", 1, SliceType.Meat, buffet.AddPizza(1, SliceType.Meat));

        // Two slices, which should go to neville.
        Util.sleep(3);
        Util.printAdd("Sade", 1, SliceType.Veggie, buffet.AddPizza(1, SliceType.Veggie));
        Util.sleep(1);
        Util.printAdd("Sade", 1, SliceType.Works, buffet.AddPizza(1, SliceType.Works));
      }
    };
  }

  /**
   * Neville gets there next, but can't have the three slices he wants, so he waits
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable neville(Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        Util.sleep(2);
        Util.printSlices("Neville", buffet.TakeAny(3));
      }
    };
  }

  /**
   * Dorothea gets there next.  She only wants two slices, so she can have them right away
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable dorothea(Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        //System.out.println(Thread.currentThread() + ": Marnie starting");
        Util.sleep(3);
        Util.printSlices("Dorothea", buffet.TakeAny(1));
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

    // Make a few server threads
    Util.run(List.of(new Thread(dorothea(buffet)),
        new Thread(neville(buffet)), new Thread(sade(buffet))));
  }
}
