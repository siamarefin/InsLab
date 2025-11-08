#!/usr/bin/env python3
# caesar_cracker.py
# Attempts all Caesar shifts, scores candidates by common words,
# prints every candidate and the best-scoring one at the end.

import sys
from typing import List, Tuple

def rotate_alpha(ch: str, shift_back: int) -> str:
    """Reverse the Caesar shift by 'shift_back' positions, preserving case."""
    if 'A' <= ch <= 'Z':
        base = ord('A')
        idx = ord(ch) - base
        dec = (idx - shift_back) % 26
        return chr(base + dec)
    if 'a' <= ch <= 'z':
        base = ord('a')
        idx = ord(ch) - base
        dec = (idx - shift_back) % 26
        return chr(base + dec)
    return ch  # non-letters unchanged

def decode_with_shift(s: str, shift: int) -> str:
    return ''.join(rotate_alpha(ch, shift) for ch in s)

def heuristic_score(text: str) -> int:
    """
    Count occurrences of frequent function words to approximate English-likeness.
    Matches with spaces on both sides by padding.
    """
    probes: List[str] = [" the ", " and ", " to ", " of ", " that ", " is ", " in ", " it "]
    padded = " " + text + " "
    score = 0
    for w in probes:
        pos = 0
        while True:
            pos = padded.find(w, pos)
            if pos == -1:
                break
            score += 1
            pos += len(w)
    return score

def main() -> int:
    # Read a single line of ciphertext from stdin
    line = sys.stdin.readline()
    if not line or line.strip() == "":
        print("Provide ciphertext as a single line on stdin.", file=sys.stderr)
        return 0

    candidates: List[Tuple[int, str]] = []
    for s in range(26):
        guess = decode_with_shift(line.rstrip("\n"), s)
        sc = heuristic_score(guess)
        candidates.append((sc, guess))
        print(f"Shift {s}:\n{guess}\n")

    # Choose best by score
    best_score, best_guess = max(candidates, key=lambda x: x[0])
    print("====== Best candidate (simple score) ======")
    print(best_guess)
    return 0

if __name__ == "__main__":
    sys.exit(main())
