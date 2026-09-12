package com.alignx;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

public final class Fasta {
  private Fasta() {}

  public static String readFastaOrRaw(String pathOrSeq) {
    Path p = Path.of(pathOrSeq);
    if (Files.isRegularFile(p)) {
      try {
        StringBuilder seq = new StringBuilder();
        for (String line : Files.readAllLines(p)) {
          if (line.isEmpty()) continue;
          char c = line.charAt(0);
          if (c == '>' || c == ';') continue;
          seq.append(line);
        }
        return stripWs(seq.toString());
      } catch (IOException e) {
        throw new RuntimeException("Failed to read " + pathOrSeq, e);
      }
    }
    return stripWs(pathOrSeq);
  }

  private static String stripWs(String s) {
    return s.replaceAll("\\s+", "");
  }
}
