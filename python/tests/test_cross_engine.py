from __future__ import annotations

from pathlib import Path

import pytest

from alignx.runners import run_engine

ROOT = Path(__file__).resolve().parents[2]
FIXTURES = ROOT / "fixtures"


CASES = [
    # identical short motif
    ("ACGTACGT", "ACGTACGT", "affine"),
    ("ACGTACGT", "ACGTACGT", "linear"),
    # classic local: shared substring with flanks
    ("AAAAACGTACGTAAAA", "TTTTCGTACGTTTTT", "affine"),
    ("AAAAACGTACGTAAAA", "TTTTCGTACGTTTTT", "linear"),
    # mismatch / indel pressure
    ("GATTACA", "GCATGCU", "affine"),
    ("GATTACA", "GCATGCU", "linear"),
    # single base
    ("A", "A", "affine"),
    ("A", "T", "linear"),
]


def _assert_same(a, b):
    assert a.score == b.score
    assert a.aligned_query == b.aligned_query
    assert a.aligned_subject == b.aligned_subject
    assert a.start_query == b.start_query
    assert a.start_subject == b.start_subject
    assert a.end_query == b.end_query
    assert a.end_subject == b.end_subject
    assert a.mode == b.mode


@pytest.mark.parametrize("query,subject,mode", CASES)
def test_cpp_matches_java_raw(query: str, subject: str, mode: str):
    cpp = run_engine("cpp", query, subject, mode=mode)
    java = run_engine("java", query, subject, mode=mode)
    _assert_same(cpp, java)


def test_fasta_fixtures_affine():
    q = str(FIXTURES / "query.fa")
    s = str(FIXTURES / "subject.fa")
    cpp = run_engine("cpp", q, s, mode="affine")
    java = run_engine("java", q, s, mode="affine")
    _assert_same(cpp, java)
    assert cpp.score > 0
    assert len(cpp.aligned_query) == len(cpp.aligned_subject)


def test_fasta_fixtures_linear():
    q = str(FIXTURES / "query.fa")
    s = str(FIXTURES / "subject.fa")
    cpp = run_engine("cpp", q, s, mode="linear")
    java = run_engine("java", q, s, mode="linear")
    _assert_same(cpp, java)


def test_perfect_match_score():
    # match=2, length 8 → score 16 with no gaps
    r = run_engine("cpp", "ACGTACGT", "ACGTACGT", mode="linear")
    assert r.score == 16
    assert r.aligned_query == "ACGTACGT"
    assert r.aligned_subject == "ACGTACGT"


def test_no_similarity_zero():
    r = run_engine("java", "AAAA", "TTTT", match=2, mismatch=-5, gap_open=10, mode="linear")
    assert r.score == 0
