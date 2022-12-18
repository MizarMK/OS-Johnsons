public enum SliceType {
  Veggie,
  Cheese,
  Meat,
  Works;
  
  public boolean isVeg() {
    return this == Veggie || this == Cheese;
  }
}
