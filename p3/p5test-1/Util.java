import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.List;
import java.util.Random;
import java.util.stream.Collectors;

/**
 * Test utility class
 */
public final class Util {
  /**
   * Start time (really class load time)
   */
  public static long start = System.nanoTime();

  /**
   * Sleep for given seconds
   * 
   * @param seconds time to sleep
   */
  public static void sleep(int seconds) {
    try {
      Thread.sleep(seconds * 1000);
    } catch (InterruptedException e) {
      // Ignoring
    }
  }
  
  /**
   * Random sleep generator
   */
  public static Random r = new Random();

  /**
   * Sleep for random time with given bound
   * 
   * @param maxMS maximum milliseconds to sleep
   */
  public static void randSleep(final int maxMS) {
    try {
      Thread.sleep(r.nextInt(maxMS));
    } catch (InterruptedException e) {
      // Ignoring
    }
  }
  
  /**
   * Test if the number of slices on a plate matches expectations
   * 
   * @param plate plate to test
   * @param slices expected number of slices
   * @param veg true if slices are vegetarian
   */
  public static void testSlices(List<SliceType> plate, int slices, boolean veg) {
    if (plate.size() != slices) {
      System.err.println("Exected any %d slices but got %d slices".formatted(slices, plate.size()));
      System.exit(1);
    }
    if (veg && !plate.stream().allMatch(SliceType::isVeg)) {
        System.err.println("Exected %d veg slices but got %d slices".formatted(slices, plate.size()));
        System.exit(1);
    }
  }
  
  /**
   * Create timestamp
   * 
   * @return timestamp
   */
  public static String ts() {
    return String.format("%.5f", (System.nanoTime() - start) / 1000000000.0);
  }

  /**
   * Print slices of pizza
   * 
   * @param name name of person getting pizza
   * @param slices list of slices
   */
  public static void printSlices(final String name, final List<SliceType> slices) {
    StringBuilder b = new StringBuilder();
    b.append("%s: %s gets: ".formatted(ts(), name));
    if (slices != null) {
        b.append(slices.stream().map(Enum::toString).collect(Collectors.joining(",")));
    } else {
      b.append("nothing");
    }
    System.out.println(b);
  }
  
  /**
   * Print addition of slices
   * 
   * @param name name of person adding pizza
   * @param success true if slices added successfully
   */
  public static void printAdd(final String name, int slices, SliceType type, boolean success) {
    if (!success) {
      System.err.printf("%d: %s failed to add %d slices of %s\n", ts(), name, slices, type);
    } else {
      System.out.printf("%s: %s added %d slices of %s\n", ts(), name, slices, type);
    }
  }
  
  public static void run(List<Thread> threads) {
    // Make a few actor threads
    threads.stream().forEach(Thread::start);
    threads.stream().forEach(t -> {
      try {
        t.join();
      } catch (InterruptedException e) {}});
  }
  
  /**
   * Construct buffet based on class name with given max slices
   * 
   * @param className name of class to instantiate
   * @param maxSlices maximum number of pizza slices 
   * 
   * @return new Buffet
   * 
   * @throws NoSuchMethodException if problems
   * @throws SecurityException
   * @throws ClassNotFoundException
   * @throws InstantiationException
   * @throws IllegalAccessException
   * @throws IllegalArgumentException
   * @throws InvocationTargetException
   */
  protected static Buffet getBuffet(String className, int maxSlices) throws NoSuchMethodException, SecurityException, ClassNotFoundException, InstantiationException, IllegalAccessException, IllegalArgumentException, InvocationTargetException {
    Constructor<?> constructor = Class.forName(className).getConstructor(Integer.TYPE);
    return (Buffet) constructor.newInstance(maxSlices);
  }
}
