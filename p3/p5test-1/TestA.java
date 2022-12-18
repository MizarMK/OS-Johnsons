import java.lang.reflect.InvocationTargetException;
import java.util.List;

/**
 * Test:  Verify non-veg patron will not take
 * any veg slices if there's a vegetarian waiting.
 * 
 * @version 1.0
 */
public class TestA {  
  /**
   * Caitlyn tries to get two veg slices
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable caitlyn(Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        Util.sleep(2);
        Util.printSlices("Caitlyn", buffet.TakeVeg(2));
      }
    };
  }

  /**
   * Evangelina gets there next, and wants two slices of anything
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable evangelina(Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        Util.sleep(3);
        Util.printSlices("Evangelina", buffet.TakeAny(2));
      }
    };
  }

  /**
   * Winnifred puts slices on the buffet, gradually
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable winnifred(Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        Util.sleep(1);
        Util.printAdd("Winnifred", 1, SliceType.Cheese, buffet.AddPizza(1, SliceType.Cheese));
        Util.printAdd("Winnifred", 1, SliceType.Meat, buffet.AddPizza(1, SliceType.Meat));

        Util.sleep(3);
        Util.printAdd("Winnifred", 1, SliceType.Veggie, buffet.AddPizza(1, SliceType.Veggie));
        

        Util.sleep(1);
        Util.printAdd("Winnifred", 1, SliceType.Works, buffet.AddPizza(1, SliceType.Works));
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
    Util.run(List.of(new Thread(caitlyn(buffet)),
        new Thread(evangelina(buffet)), new Thread(winnifred(buffet))));
  }
}
