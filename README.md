# alignx

**Smith–Waterman local sequence alignment** — C++17 engine, Java reference (same CLI/output contract), and Python tests that assert bit-for-bit agreement on fixtures.

[![CI](https://github.com/SK090347/alignx/actions/workflows/ci.yml/badge.svg)](https://github.com/SK090347/alignx/actions/workflows/ci.yml)
[![License: MIT OR Apache-2.0](https://img.shields.io/badge/license-MIT%20OR%20Apache--2.0-blue.svg)](LICENSE)

> Author: **Sumit Kumar Ta (SK090347)** · Dual license **MIT OR Apache-2.0**

---

## Mathematics / Formulation

Local alignment finds the highest-scoring contiguous pair of subsequences. With substitution score $s(a_i,b_j)$ and linear gap penalty $g > 0$:

$$
H_{i,j} = \max\begin{cases}
0 \\
H_{i-1,j-1} + s(a_i,b_j) \\
H_{i-1,j} - g \\
H_{i,j-1} - g
\end{cases}
$$

Affine gaps (Gotoh) with open $o$ and extend $e$:

$$
\begin{aligned}
E_{i,j} &= \max(H_{i,j-1} - o - e,\ E_{i,j-1} - e) \\
F_{i,j} &= \max(H_{i-1,j} - o - e,\ F_{i-1,j} - e) \\
H_{i,j} &= \max(0,\ H_{i-1,j-1} + s(a_i,b_j),\ E_{i,j},\ F_{i,j})
\end{aligned}
$$

Traceback from $\arg\max H$ until a zero cell. What is computed: **optimal local alignment score** plus the aligned substrings $(Q,S)$ and their endpoints.

### Complexity

| Mode | Time | Space | Notes |
|------|------|-------|-------|
| Linear gap | $O(mn)$ | $O(mn)$ | Single matrix $H$ + traceback |
| Affine gap | $O(mn)$ | $O(mn)$ | Matrices $H,E,F$ + pointers |
| Hirschberg (not shipped) | $O(mn)$ | $O(\min(m,n))$ | Linear-space upgrade path |

**Why this formula?** The $\max(0,\ldots)$ clamp is what makes Smith–Waterman *local* — global Needleman–Wunsch cannot stop early. Affine gaps match biology’s open-vs-extend cost asymmetry.

---

## Why Smith–Waterman?

Global alignment forces end-to-end paths. **Local** alignment is the right model for shared domains, motifs, and noisy flanks — classic dynamic programming for sequences of lengths $m$ and $n$.

```mermaid
flowchart LR
  A[Read FASTA / raw] --> B[Fill H / E / F DP]
  B --> C[Find max cell]
  C --> D[Traceback]
  D --> E[Aligned Q / S + score]
  E --> F{Python pytest}
  F -->|C++ == Java| G[Pass]
```

---

## Layout

| Path | Role |
|------|------|
| `cpp/` | C++17 library + CLI (`alignx`) — CMake |
| `java/` | Java reference (`com.alignx`) — `javac` or Maven |
| `python/` | CLI harness + **pytest** cross-checks |
| `fixtures/` | FASTA-ish sample pairs |
| `.github/workflows/ci.yml` | Build all three languages + tests |

---

## Quick start

```bash
# C++ + Java
make cpp
make java

# Cross-engine tests (needs pytest)
python3 -m venv .venv && source .venv/bin/activate
pip install -e "./python[dev]"
pytest python/tests -v

# Side-by-side demo
make demo
```

### CLI (identical flags on C++ and Java)

```bash
./cpp/build/alignx --affine --match 2 --mismatch -1 --gap-open 2 --gap-extend 1 \
  fixtures/query.fa fixtures/subject.fa

java -cp java/out com.alignx.Main --affine fixtures/query.fa fixtures/subject.fa

# Or via Python
python -m alignx.cli --engine both fixtures/query.fa fixtures/subject.fa
```

Machine-readable output (used by tests):

```
SCORE=...
Q=...
S=...
START_Q=...
START_S=...
END_Q=...
END_S=...
MODE=affine|linear
```

Raw sequences are accepted as arguments; paths are treated as **FASTA-ish** (headers `>` / `;` skipped, whitespace stripped).

---

## Use cases

- Teaching local alignment / Gotoh affine gaps alongside Needleman–Wunsch
- Validating a high-performance C++ core against a transparent Java reference
- Portfolio demonstration of DP, multi-language tooling, and CI hygiene
- Seed for motif search or domain-spotting prototypes (not a BLAST replacement)

---

## Build notes

- **C++**: CMake ≥ 3.16, C++17 (`cmake -S cpp -B cpp/build && cmake --build cpp/build`)
- **Java**: JDK 17+ (`make java` → `java/out`), optional `java/pom.xml`
- **Python**: 3.10+, pytest for the comparison suite

---

## License

Dual **MIT OR Apache-2.0** — see `LICENSE`, `LICENSE-MIT`, `LICENSE-APACHE`, and `NOTICE`.

## Topics

`bioinformatics` · `smith-waterman` · `cpp` · `java` · `python` · `dynamic-programming` · `portfolio`
