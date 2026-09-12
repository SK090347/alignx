from __future__ import annotations

import argparse
import sys

from alignx.runners import run_engine


def main(argv: list[str] | None = None) -> int:
    p = argparse.ArgumentParser(
        description="Run alignx C++ or Java Smith-Waterman and print the alignment."
    )
    p.add_argument("query", help="FASTA path or raw sequence")
    p.add_argument("subject", help="FASTA path or raw sequence")
    p.add_argument("--engine", choices=("cpp", "java", "both"), default="both")
    p.add_argument("--match", type=int, default=2)
    p.add_argument("--mismatch", type=int, default=-1)
    p.add_argument("--gap-open", type=int, default=2)
    p.add_argument("--gap-extend", type=int, default=1)
    p.add_argument("--mode", choices=("affine", "linear"), default="affine")
    args = p.parse_args(argv)

    engines = ["cpp", "java"] if args.engine == "both" else [args.engine]
    for eng in engines:
        r = run_engine(
            eng,
            args.query,
            args.subject,
            match=args.match,
            mismatch=args.mismatch,
            gap_open=args.gap_open,
            gap_extend=args.gap_extend,
            mode=args.mode,
        )
        print(f"=== {eng} ===")
        print(f"SCORE={r.score}")
        print(f"Q={r.aligned_query}")
        print(f"S={r.aligned_subject}")
        print(f"START_Q={r.start_query}")
        print(f"START_S={r.start_subject}")
        print(f"END_Q={r.end_query}")
        print(f"END_S={r.end_subject}")
        print(f"MODE={r.mode}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
