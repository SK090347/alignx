#include "smith_waterman.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

static void usage(const char* argv0) {
  std::cerr
      << "Usage: " << argv0
      << " [--match N] [--mismatch N] [--gap-open N] [--gap-extend N]\n"
      << "       [--linear | --affine] <query.fa|SEQ> <subject.fa|SEQ>\n"
      << "Prints machine-readable alignment for cross-language checks.\n";
}

int main(int argc, char** argv) {
  alignx::Scoring scoring;
  std::string query_arg, subject_arg;
  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    auto need = [&](const char* name) -> int {
      if (i + 1 >= argc) {
        std::cerr << "Missing value for " << name << "\n";
        std::exit(2);
      }
      return std::atoi(argv[++i]);
    };
    if (a == "--match") {
      scoring.match = need("--match");
    } else if (a == "--mismatch") {
      scoring.mismatch = need("--mismatch");
    } else if (a == "--gap-open") {
      scoring.gap_open = need("--gap-open");
    } else if (a == "--gap-extend") {
      scoring.gap_extend = need("--gap-extend");
    } else if (a == "--linear") {
      scoring.mode = alignx::GapMode::Linear;
    } else if (a == "--affine") {
      scoring.mode = alignx::GapMode::Affine;
    } else if (a == "-h" || a == "--help") {
      usage(argv[0]);
      return 0;
    } else if (a[0] == '-') {
      std::cerr << "Unknown flag: " << a << "\n";
      usage(argv[0]);
      return 2;
    } else if (query_arg.empty()) {
      query_arg = a;
    } else if (subject_arg.empty()) {
      subject_arg = a;
    } else {
      std::cerr << "Unexpected argument: " << a << "\n";
      return 2;
    }
  }

  if (query_arg.empty() || subject_arg.empty()) {
    usage(argv[0]);
    return 2;
  }

  auto [q, s] = alignx::load_two_sequences(query_arg, subject_arg);
  if (q.empty() || s.empty()) {
    std::cerr << "Empty sequence after load\n";
    return 1;
  }

  alignx::SmithWaterman sw(scoring);
  auto r = sw.align(q, s);

  std::cout << "SCORE=" << r.score << "\n"
            << "Q=" << r.aligned_query << "\n"
            << "S=" << r.aligned_subject << "\n"
            << "START_Q=" << r.start_query << "\n"
            << "START_S=" << r.start_subject << "\n"
            << "END_Q=" << r.end_query << "\n"
            << "END_S=" << r.end_subject << "\n"
            << "MODE=" << (scoring.mode == alignx::GapMode::Affine ? "affine" : "linear")
            << "\n";
  return 0;
}
