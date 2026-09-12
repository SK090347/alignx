package com.alignx;

public final class SmithWaterman {
  private final Scoring scoring;

  public SmithWaterman() {
    this(new Scoring());
  }

  public SmithWaterman(Scoring scoring) {
    this.scoring = scoring.copy();
  }

  private int sub(char a, char b) {
    char ua = Character.toUpperCase(a);
    char ub = Character.toUpperCase(b);
    return ua == ub ? scoring.match : scoring.mismatch;
  }

  public AlignmentResult align(String query, String subject) {
    int m = query.length();
    int n = subject.length();
    AlignmentResult out = new AlignmentResult();
    if (m == 0 || n == 0) {
      return out;
    }

    int go = scoring.gapOpen;
    int ge = scoring.gapExtend;
    boolean affine = scoring.mode == Scoring.GapMode.AFFINE;

    int[][] H = new int[m + 1][n + 1];
    int[][] E = new int[m + 1][n + 1];
    int[][] F = new int[m + 1][n + 1];
    byte[][] ptr = new byte[m + 1][n + 1];

    int best = 0;
    int bi = 0, bj = 0;

    for (int i = 1; i <= m; i++) {
      for (int j = 1; j <= n; j++) {
        int diag = H[i - 1][j - 1] + sub(query.charAt(i - 1), subject.charAt(j - 1));

        int up, left;
        if (affine) {
          F[i][j] = Math.max(H[i - 1][j] - go - ge, F[i - 1][j] - ge);
          E[i][j] = Math.max(H[i][j - 1] - go - ge, E[i][j - 1] - ge);
          up = F[i][j];
          left = E[i][j];
        } else {
          up = H[i - 1][j] - go;
          left = H[i][j - 1] - go;
        }

        int score = diag;
        byte p = 1;
        if (up > score) {
          score = up;
          p = 2;
        }
        if (left > score) {
          score = left;
          p = 3;
        }
        if (score < 0) {
          score = 0;
          p = 0;
        }

        H[i][j] = score;
        ptr[i][j] = p;
        if (score > best) {
          best = score;
          bi = i;
          bj = j;
        }
      }
    }

    out.score = best;
    if (best == 0) {
      return out;
    }

    StringBuilder aq = new StringBuilder();
    StringBuilder as = new StringBuilder();
    int i = bi, j = bj;
    while (i > 0 && j > 0 && ptr[i][j] != 0) {
      byte p = ptr[i][j];
      if (p == 1) {
        aq.append(query.charAt(i - 1));
        as.append(subject.charAt(j - 1));
        i--;
        j--;
      } else if (p == 2) {
        aq.append(query.charAt(i - 1));
        as.append('-');
        i--;
      } else {
        aq.append('-');
        as.append(subject.charAt(j - 1));
        j--;
      }
    }

    out.alignedQuery = aq.reverse().toString();
    out.alignedSubject = as.reverse().toString();
    out.startQuery = i;
    out.startSubject = j;
    out.endQuery = bi;
    out.endSubject = bj;
    return out;
  }
}
