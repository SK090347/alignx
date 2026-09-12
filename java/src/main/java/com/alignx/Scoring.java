package com.alignx;

public final class Scoring {
  public enum GapMode { LINEAR, AFFINE }

  public int match = 2;
  public int mismatch = -1;
  public int gapOpen = 2;
  public int gapExtend = 1;
  public GapMode mode = GapMode.AFFINE;

  public Scoring copy() {
    Scoring s = new Scoring();
    s.match = match;
    s.mismatch = mismatch;
    s.gapOpen = gapOpen;
    s.gapExtend = gapExtend;
    s.mode = mode;
    return s;
  }
}
