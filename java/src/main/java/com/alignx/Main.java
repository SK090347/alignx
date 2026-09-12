package com.alignx;

public final class Main {
  private Main() {}

  private static void usage() {
    System.err.println(
        "Usage: java com.alignx.Main [--match N] [--mismatch N] [--gap-open N] [--gap-extend N]\n"
            + "       [--linear | --affine] <query.fa|SEQ> <subject.fa|SEQ>");
  }

  public static void main(String[] args) {
    Scoring scoring = new Scoring();
    String queryArg = null;
    String subjectArg = null;

    for (int i = 0; i < args.length; i++) {
      String a = args[i];
      if (a.equals("--match")) {
        scoring.match = Integer.parseInt(args[++i]);
      } else if (a.equals("--mismatch")) {
        scoring.mismatch = Integer.parseInt(args[++i]);
      } else if (a.equals("--gap-open")) {
        scoring.gapOpen = Integer.parseInt(args[++i]);
      } else if (a.equals("--gap-extend")) {
        scoring.gapExtend = Integer.parseInt(args[++i]);
      } else if (a.equals("--linear")) {
        scoring.mode = Scoring.GapMode.LINEAR;
      } else if (a.equals("--affine")) {
        scoring.mode = Scoring.GapMode.AFFINE;
      } else if (a.equals("-h") || a.equals("--help")) {
        usage();
        return;
      } else if (a.startsWith("-")) {
        System.err.println("Unknown flag: " + a);
        usage();
        System.exit(2);
      } else if (queryArg == null) {
        queryArg = a;
      } else if (subjectArg == null) {
        subjectArg = a;
      } else {
        System.err.println("Unexpected argument: " + a);
        System.exit(2);
      }
    }

    if (queryArg == null || subjectArg == null) {
      usage();
      System.exit(2);
    }

    String q = Fasta.readFastaOrRaw(queryArg);
    String s = Fasta.readFastaOrRaw(subjectArg);
    if (q.isEmpty() || s.isEmpty()) {
      System.err.println("Empty sequence after load");
      System.exit(1);
    }

    AlignmentResult r = new SmithWaterman(scoring).align(q, s);
    System.out.println("SCORE=" + r.score);
    System.out.println("Q=" + r.alignedQuery);
    System.out.println("S=" + r.alignedSubject);
    System.out.println("START_Q=" + r.startQuery);
    System.out.println("START_S=" + r.startSubject);
    System.out.println("END_Q=" + r.endQuery);
    System.out.println("END_S=" + r.endSubject);
    System.out.println(
        "MODE=" + (scoring.mode == Scoring.GapMode.AFFINE ? "affine" : "linear"));
  }
}
