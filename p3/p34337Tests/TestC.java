import java.lang.reflect.InvocationTargetException;
import java.util.List;

/**
 * Test:  Verify servers will wait if the
 * buffet is too full.
 * 
 * @version 1.0
 */
public class TestC {
  /**
   * Scot gets there first, with 9 slices
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable scot(final Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        Util.sleep(1);
        Util.printAdd("Scot", 9, SliceType.Meat, buffet.AddPizza(9, SliceType.Meat));
      }
    };
  }

  /**
   * Shirlee gets there next, but can't drop off all her slices
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable shirlee(final Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        Util.sleep(2);
        Util.printAdd("Shirlee", 12, SliceType.Works, buffet.AddPizza(12, SliceType.Works));
      }
    };
  }

  /**
   * Marnie takes some slices after three seconds.  This will let shirlee finish
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable marnie(final Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        Util.sleep(3);
        Util.printSlices("Marnie", buffet.TakeAny(2));
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
    Util.run(List.of(new Thread(marnie(buffet)),
        new Thread(shirlee(buffet)), new Thread(scot(buffet))));
  }
}
