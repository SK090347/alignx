from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class AlignmentOutput:
    score: int
    aligned_query: str
    aligned_subject: str
    start_query: int
    start_subject: int
    end_query: int
    end_subject: int
    mode: str


def parse_engine_output(text: str) -> AlignmentOutput:
    fields: dict[str, str] = {}
    for line in text.strip().splitlines():
        line = line.strip()
        if not line or "=" not in line:
            continue
        k, v = line.split("=", 1)
        fields[k] = v
    required = [
        "SCORE",
        "Q",
        "S",
        "START_Q",
        "START_S",
        "END_Q",
        "END_S",
        "MODE",
    ]
    missing = [k for k in required if k not in fields]
    if missing:
        raise ValueError(f"Missing fields: {missing} in output:\n{text}")
    return AlignmentOutput(
        score=int(fields["SCORE"]),
        aligned_query=fields["Q"],
        aligned_subject=fields["S"],
        start_query=int(fields["START_Q"]),
        start_subject=int(fields["START_S"]),
        end_query=int(fields["END_Q"]),
        end_subject=int(fields["END_S"]),
        mode=fields["MODE"],
    )


def read_fasta_or_raw(path_or_seq: str) -> str:
    p = Path(path_or_seq)
    if p.is_file():
        seq: list[str] = []
        for line in p.read_text(encoding="utf-8").splitlines():
            if not line or line[0] in ">;":
                continue
            seq.append(line.strip())
        return "".join(seq)
    return "".join(path_or_seq.split())
