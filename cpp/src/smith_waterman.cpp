#include "smith_waterman.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace alignx {

SmithWaterman::SmithWaterman(Scoring scoring) : scoring_(std::move(scoring)) {}

int SmithWaterman::sub(char a, char b) const {
  char ua = static_cast<char>(std::toupper(static_cast<unsigned char>(a)));
  char ub = static_cast<char>(std::toupper(static_cast<unsigned char>(b)));
  return ua == ub ? scoring_.match : scoring_.mismatch;
}

AlignmentResult SmithWaterman::align(const std::string& query,
                                     const std::string& subject) const {
  const int m = static_cast<int>(query.size());
  const int n = static_cast<int>(subject.size());
  AlignmentResult out;
  if (m == 0 || n == 0) {
    return out;
  }

  const int go = scoring_.gap_open;
  const int ge = scoring_.gap_extend;
  const bool affine = scoring_.mode == GapMode::Affine;

  // H: best score ending at (i,j). E: gap in query (horiz). F: gap in subject (vert).
  std::vector<std::vector<int>> H(m + 1, std::vector<int>(n + 1, 0));
  std::vector<std::vector<int>> E(m + 1, std::vector<int>(n + 1, 0));
  std::vector<std::vector<int>> F(m + 1, std::vector<int>(n + 1, 0));
  // Trace: 0=stop, 1=diag, 2=up (gap in subject), 3=left (gap in query)
  std::vector<std::vector<char>> ptr(m + 1, std::vector<char>(n + 1, 0));

  int best = 0;
  int bi = 0, bj = 0;

  for (int i = 1; i <= m; ++i) {
    for (int j = 1; j <= n; ++j) {
      const int diag = H[i - 1][j - 1] + sub(query[i - 1], subject[j - 1]);

      int up, left;
      if (affine) {
        // Opening a gap from H costs go+ge (standard Gotoh); extend costs ge.
        F[i][j] = std::max(H[i - 1][j] - go - ge, F[i - 1][j] - ge);
        E[i][j] = std::max(H[i][j - 1] - go - ge, E[i][j - 1] - ge);
        up = F[i][j];
        left = E[i][j];
      } else {
        up = H[i - 1][j] - go;
        left = H[i][j - 1] - go;
      }

      int score = diag;
      char p = 1;
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

  std::string aq, as;
  int i = bi, j = bj;
  while (i > 0 && j > 0 && ptr[i][j] != 0) {
    const char p = ptr[i][j];
    if (p == 1) {
      aq.push_back(query[i - 1]);
      as.push_back(subject[j - 1]);
      --i;
      --j;
    } else if (p == 2) {
      aq.push_back(query[i - 1]);
      as.push_back('-');
      --i;
    } else {  // p == 3
      aq.push_back('-');
      as.push_back(subject[j - 1]);
      --j;
    }
  }

  std::reverse(aq.begin(), aq.end());
  std::reverse(as.begin(), as.end());
  out.aligned_query = std::move(aq);
  out.aligned_subject = std::move(as);
  out.start_query = i;
  out.start_subject = j;
  out.end_query = bi;
  out.end_subject = bj;
  return out;
}

static std::string strip_ws(std::string s) {
  s.erase(std::remove_if(s.begin(), s.end(),
                         [](unsigned char c) { return std::isspace(c) != 0; }),
          s.end());
  return s;
}

std::string read_fasta_or_raw(const std::string& path_or_seq) {
  // If it looks like a path that exists, read FASTA/raw file; else treat as sequence.
  std::ifstream in(path_or_seq);
  if (!in) {
    return strip_ws(path_or_seq);
  }
  std::ostringstream seq;
  std::string line;
  bool fasta = false;
  while (std::getline(in, line)) {
    if (line.empty()) continue;
    if (line[0] == '>' || line[0] == ';') {
      fasta = true;
      continue;
    }
    seq << line;
  }
  (void)fasta;
  return strip_ws(seq.str());
}

std::pair<std::string, std::string> load_two_sequences(const std::string& a,
                                                       const std::string& b) {
  return {read_fasta_or_raw(a), read_fasta_or_raw(b)};
}

}  // namespace alignx
