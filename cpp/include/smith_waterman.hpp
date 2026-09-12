#pragma once

#include <string>
#include <vector>
#include <utility>

namespace alignx {

enum class GapMode { Linear, Affine };

struct Scoring {
  int match = 2;
  int mismatch = -1;
  int gap_open = 2;    // positive cost; applied as -gap_open
  int gap_extend = 1;  // positive cost; applied as -gap_extend
  GapMode mode = GapMode::Affine;
};

struct AlignmentResult {
  int score = 0;
  std::string aligned_query;
  std::string aligned_subject;
  int start_query = 0;   // 0-based inclusive on original sequences
  int start_subject = 0;
  int end_query = 0;     // 0-based exclusive
  int end_subject = 0;
};

class SmithWaterman {
 public:
  explicit SmithWaterman(Scoring scoring = {});

  AlignmentResult align(const std::string& query,
                        const std::string& subject) const;

 private:
  Scoring scoring_;
  int sub(char a, char b) const;
};

std::string read_fasta_or_raw(const std::string& path_or_seq);
std::pair<std::string, std::string> load_two_sequences(
    const std::string& a, const std::string& b);

}  // namespace alignx
