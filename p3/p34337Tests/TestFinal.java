import java.lang.reflect.InvocationTargetException;
import java.util.List;

/**
 * Test multiple servers and patrons
 * 
 * @version 1.0
 */
public class TestFinal {
  /**
   * Mary serves cheese pizza
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable mary(Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        while (buffet.AddPizza(6, SliceType.Cheese)) {
          Util.randSleep(150);
        }
      }
    };
  }

  /**
   * Nelle serves meat pizza
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable nelle(Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        while (buffet.AddPizza(8, SliceType.Meat)) {
          Util.randSleep(200);
        }
      }
    };
  }

  /**
   * John serves veggie pizza
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable john(Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        while (buffet.AddPizza(3, SliceType.Veggie)) {
          Util.randSleep(75);
        }
      }
    };
  }

  /**
   * Genevive serves works pizza
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable genevive(Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        while (buffet.AddPizza(4, SliceType.Works)) {
          Util.randSleep(100);
        }
      }
    };
  }

  /**
   * Laverne eats any kind of pizza, 2 slices at a time
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable laverne(Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        List<SliceType> slices;
        while ((slices = buffet.TakeAny(2)) != null) {
          Util.printSlices("Laverne", slices);
          Util.testSlices(slices, 2, false);
          Util.randSleep(50);
        }
      }
    };
  }

  /**
   * Michael eats vegetarian pizza, 3 slices at a time
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable michael(Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        List<SliceType> slices;
        while ((slices = buffet.TakeVeg(3)) != null) {
          Util.printSlices("Michael", slices);
          Util.testSlices(slices, 3, true);
          Util.randSleep(75);
        }
      }
    };
  }

  /**
   * Kelsey eats vegetarian pizza, 1 slice at a time
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable kelsey(Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        List<SliceType> slices;
        while ((slices = buffet.TakeVeg(1)) != null) {
          Util.printSlices("Kelsey", slices);
          Util.testSlices(slices, 1, true);
          Util.randSleep(25);
        }
      }
    };
  }

  /**
   * Daren eats any kind of pizza, 2 slices at a time
   * 
   * @param buffet buffet to use
   * 
   * @return runnable thread main
   */
  public static Runnable daren(Buffet buffet) {
    return new Runnable() {
      @Override
      public void run() {
        List<SliceType> slices;
        while ((slices = buffet.TakeAny(2)) != null) {
          Util.printSlices("Daren", slices);
          Util.testSlices(slices, 2, false);
          Util.randSleep(50);
        }
      }
    };
  }

  /**
   * Create, run, and join actor threads
   */
  public static void main(String[] args)
      throws NumberFormatException, NoSuchMethodException, SecurityException, ClassNotFoundException,
      InstantiationException, IllegalAccessException, IllegalArgumentException, InvocationTargetException {
    if (args.length != 1) {
      throw new IllegalArgumentException("Parameter(s): <Buffet Class>");
    }
    // Initialize the buffet our threads are using
    Buffet buffet = Util.getBuffet(args[0], 20);

    // Make a few server threads
    List<Thread> threads = List.of(new Thread(mary(buffet)), new Thread(nelle(buffet)), new Thread(john(buffet)),
        new Thread(genevive(buffet)), new Thread(laverne(buffet)), new Thread(michael(buffet)),
        new Thread(kelsey(buffet)), new Thread(daren(buffet)));
    threads.stream().forEach(Thread::start);

    Util.sleep(10);
    buffet.close();

    // When we're done, join with all the threads
    threads.stream().forEach(t -> {
      try {
        t.join();
      } catch (InterruptedException e) {}});
  }
}
