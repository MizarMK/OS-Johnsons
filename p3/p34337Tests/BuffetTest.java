import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.lang.reflect.InvocationTargetException;
import java.util.List;

import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;

public class BuffetTest {
  private static final int MAXBUFFETSIZE = 5;
  private static String buffetClassName;
  private Buffet buffet;
  
  @BeforeAll
  protected static void getBuffetClassName() {
    buffetClassName = System.getProperty("buffet");
    if (buffetClassName == null) {
      throw new RuntimeException("Buffet class name property (buffet) not set");
    }
  }
  
  @BeforeEach
  protected void makeBuffet() throws NoSuchMethodException, SecurityException, ClassNotFoundException, InstantiationException, IllegalAccessException, IllegalArgumentException, InvocationTargetException {
    buffet = Util.getBuffet(buffetClassName, MAXBUFFETSIZE);
  }
  
  /*************************
   * Test TakeAny()
   *************************/
  @DisplayName("Take Any Too Many")
  @Test
  public void testTakeAnyTooMany() {
    assertThrows(IllegalArgumentException.class, () -> buffet.TakeAny(MAXBUFFETSIZE+1));
  }
  
  @DisplayName("Take Any Negative")
  @Test
  public void testTakeAnyTooFew() {
    assertThrows(IllegalArgumentException.class, () -> buffet.TakeAny(-1));
  }
  
  @DisplayName("Take Any Multiple")
  @Test
  public void testTakeAnyOrder() {
    assertTrue(buffet.AddPizza(1, SliceType.Cheese));
    assertTrue(buffet.AddPizza(1, SliceType.Meat));
    assertTrue(buffet.AddPizza(1, SliceType.Cheese));
    List<SliceType> slices = buffet.TakeAny(2);
    assertArrayEquals(
        new SliceType[] {SliceType.Cheese, SliceType.Meat}, 
        slices.toArray());
    slices = buffet.TakeAny(1);
    assertArrayEquals(
        new SliceType[] {SliceType.Cheese}, 
        slices.toArray());
  }
  
  @DisplayName("Take Any Close")
  @Test
  public void testTakeAnyClose() {
    assertTrue(buffet.AddPizza(1, SliceType.Cheese));
    assertTrue(buffet.AddPizza(1, SliceType.Meat));
    assertTrue(buffet.AddPizza(1, SliceType.Cheese));
    buffet.close();
    assertNull(buffet.TakeAny(2));
  }
  
  /*************************
   * Test TakeVeg()
   *************************/
  @DisplayName("Take Veg Too Many")
  @Test
  public void testTakeVegTooMany() {
    assertThrows(IllegalArgumentException.class, () -> buffet.TakeVeg(MAXBUFFETSIZE+1));
  }
  
  @DisplayName("Take Veg Negative")
  @Test
  public void testTakeVegTooFew() {
    assertThrows(IllegalArgumentException.class, () -> buffet.TakeVeg(-1));
  }
  
  @DisplayName("Take Veg Multiple")
  @Test
  public void testTakeVegOrder() {
    assertTrue(buffet.AddPizza(1, SliceType.Cheese));
    assertTrue(buffet.AddPizza(1, SliceType.Meat));
    assertTrue(buffet.AddPizza(1, SliceType.Veggie));
    List<SliceType> slices = buffet.TakeVeg(2);
    assertArrayEquals(
        new SliceType[] {SliceType.Cheese, SliceType.Veggie}, 
        slices.toArray());
    slices = buffet.TakeAny(1);
    assertArrayEquals(
        new SliceType[] {SliceType.Meat}, 
        slices.toArray());
  }
  
  @DisplayName("Take Veg Close")
  @Test
  public void testTakeVegClose() {
    assertTrue(buffet.AddPizza(1, SliceType.Cheese));
    assertTrue(buffet.AddPizza(1, SliceType.Meat));
    assertTrue(buffet.AddPizza(1, SliceType.Cheese));
    buffet.close();
    assertNull(buffet.TakeVeg(2));
  }
  
  /*************************
   * Test AddPizza()
   *************************/
  @DisplayName("Add Pizza Negative")
  @Test
  public void testAddPizzaTooFew() {
    assertThrows(IllegalArgumentException.class, () -> buffet.AddPizza(-1, SliceType.Cheese));
  }
  
  @DisplayName("Add Pizza Null")
  @Test
  public void testAddPizzaNull() {
    assertTrue(buffet.AddPizza(1, SliceType.Cheese));
    assertTrue(buffet.AddPizza(2, SliceType.Meat));
    assertThrows(NullPointerException.class, () -> buffet.AddPizza(1, null));
  }

  @DisplayName("Add Pizza Multiple")
  @Test
  public void testAddPizzaOrder() {
    assertTrue(buffet.AddPizza(1, SliceType.Cheese));
    assertTrue(buffet.AddPizza(2, SliceType.Meat));
    assertTrue(buffet.AddPizza(1, SliceType.Cheese));
    List<SliceType> slices = buffet.TakeAny(4);
    assertArrayEquals(
        new SliceType[] {SliceType.Cheese, SliceType.Meat, SliceType.Meat, SliceType.Cheese}, 
        slices.toArray());
  }
  
  @DisplayName("Add Pizza Close")
  @Test
  public void testAddPizzaClose() {
    assertTrue(buffet.AddPizza(1, SliceType.Cheese));
    buffet.close();
    assertFalse(buffet.AddPizza(2, SliceType.Meat));
  }
}
