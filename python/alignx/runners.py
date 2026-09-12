from __future__ import annotations

import os
import subprocess
from pathlib import Path

from alignx.parse import AlignmentOutput, parse_engine_output

REPO_ROOT = Path(__file__).resolve().parents[2]


def _cpp_bin() -> Path:
    env = os.environ.get("ALIGNX_CPP")
    if env:
        return Path(env)
    candidates = [
        REPO_ROOT / "cpp" / "build" / "alignx",
        REPO_ROOT / "build" / "alignx",
    ]
    for c in candidates:
        if c.is_file():
            return c
    return candidates[0]


def _java_cp() -> str:
    env = os.environ.get("ALIGNX_JAVA_CP")
    if env:
        return env
    return str(REPO_ROOT / "java" / "out")


def run_engine(
    engine: str,
    query: str,
    subject: str,
    *,
    match: int = 2,
    mismatch: int = -1,
    gap_open: int = 2,
    gap_extend: int = 1,
    mode: str = "affine",
) -> AlignmentOutput:
    common = [
        "--match",
        str(match),
        "--mismatch",
        str(mismatch),
        "--gap-open",
        str(gap_open),
        "--gap-extend",
        str(gap_extend),
        f"--{mode}",
        query,
        subject,
    ]
    if engine == "cpp":
        cmd = [str(_cpp_bin()), *common]
    elif engine == "java":
        cmd = ["java", "-cp", _java_cp(), "com.alignx.Main", *common]
    else:
        raise ValueError(f"Unknown engine: {engine}")

    proc = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        cwd=str(REPO_ROOT),
        check=False,
    )
    if proc.returncode != 0:
        raise RuntimeError(
            f"{engine} failed ({proc.returncode}):\n"
            f"cmd={cmd}\nstdout={proc.stdout}\nstderr={proc.stderr}"
        )
    return parse_engine_output(proc.stdout)
