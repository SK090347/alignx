# alignx

Smith–Waterman local alignment with a C++17 engine, a Java reference that shares the same CLI/output contract, and Python tests that assert they agree on fixtures.

[![CI](https://github.com/SK090347/alignx/actions/workflows/ci.yml/badge.svg)](https://github.com/SK090347/alignx/actions/workflows/ci.yml)
[![License: MIT OR Apache-2.0](https://img.shields.io/badge/license-MIT%20OR%20Apache--2.0-blue.svg)](LICENSE)

Sumit Kumar Ta · SK090347

## Notes

Local alignment score with substitution \(s(a_i,b_j)\) and linear gap \(g > 0\):

\[
H_{i,j} = \max\begin{cases}
0 \\
H_{i-1,j-1} + s(a_i,b_j) \\
H_{i-1,j} - g \\
H_{i,j-1} - g
\end{cases}
\]

Affine gaps (Gotoh) add open \(o\) / extend \(e\) via matrices \(E,F\). Traceback from \(\arg\max H\) until a zero cell. Time/space \(O(mn)\) for both modes.

The \(\max(0,\ldots)\) clamp is what makes this *local* — Needleman–Wunsch can’t stop early. Affine gaps match open-vs-extend cost asymmetry in biology.

## Layout

| Path | Role |
|------|------|
| `cpp/` | C++17 library + CLI — CMake |
| `java/` | Java reference — javac / Maven |
| `python/` | harness + pytest cross-checks |
| `fixtures/` | sample pairs |

## Quick start

```bash
make cpp && make java

python3 -m venv .venv && source .venv/bin/activate
pip install -e "./python[dev]"
pytest python/tests -v
make demo
```

```bash
./cpp/build/alignx --affine --match 2 --mismatch -1 --gap-open 2 --gap-extend 1 \
  fixtures/query.fa fixtures/subject.fa

java -cp java/out com.alignx.Main --affine fixtures/query.fa fixtures/subject.fa
python -m alignx.cli --engine both fixtures/query.fa fixtures/subject.fa
```

Machine-readable output (`SCORE=`, `Q=`, `S=`, endpoints, `MODE=`) is what the tests compare.

## License

**MIT** OR **Apache-2.0**.
