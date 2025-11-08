#!/usr/bin/env python3
# substitution_cracker.py
# Heuristic solver for monoalphabetic substitution ciphers.
# - Normalizes input (lowercase letters + spaces).
# - Seeds key via frequency alignment.
# - Hill-climbs by swapping letter assignments if score improves.
# - Scores by counting frequent function words.

import sys
import time
import random
from typing import List, Tuple

def normalize_text(s: str) -> str:
    t_chars: List[str] = []
    append = t_chars.append
    for ch in s:
        if ch.isalpha():
            append(ch.lower())
        elif ch == ' ':
            append(' ')
        # else: drop character
    return ''.join(t_chars)

def count_letters(s: str) -> List[int]:
    f = [0] * 26
    for ch in s:
        if 'a' <= ch <= 'z':
            f[ord(ch) - 97] += 1
    return f

def apply_mapping(ct: str, mapc: List[str]) -> str:
    out = []
    o = out.append
    for ch in ct:
        if 'a' <= ch <= 'z':
            o(mapc[ord(ch) - 97])
        else:
            o(ch)
    return ''.join(out)

def word_hit_score(s: str) -> int:
    common = [" the ", " and ", " to ", " of ", " that ", " is ", " in ", " it ", " a "]
    padded = " " + s + " "
    sc = 0
    for w in common:
        pos = 0
        while True:
            pos = padded.find(w, pos)
            if pos == -1:
                break
            sc += 1
            pos += len(w)
    return sc

def total_score(plaintext: str) -> int:
    return word_hit_score(plaintext)

def seed_mapping_by_frequency(ct: str) -> List[str]:
    english_rank = "etaoinshrdlcumwfgypbvkjxqz"
    f = count_letters(ct)
    ranked = sorted(((f[i], i) for i in range(26)), key=lambda x: (-x[0], x[1]))
    mapc = ['x'] * 26
    for i in range(26):
        cipher_idx = ranked[i][1]
        mapc[cipher_idx] = english_rank[i]
    return mapc

def main() -> int:
    # Read entire stdin (multi-line allowed)
    data = sys.stdin.read()
    if not data:
        print("Provide ciphertext via stdin", file=sys.stderr)
        return 0

    cipher = normalize_text(data)

    # Seed key by frequency
    best_key = seed_mapping_by_frequency(cipher)
    best_plain = apply_mapping(cipher, best_key)
    best_sc = total_score(best_plain)

    # Hill-climb with random swaps
    random.seed(int(time.time() * 1e9) & 0xffffffff)
    MAX_ITERS = 30000

    for _ in range(MAX_ITERS):
        a = random.randrange(26)
        b = random.randrange(26)
        if a == b:
            continue

        trial_key = best_key[:]
        trial_key[a], trial_key[b] = trial_key[b], trial_key[a]

        trial_plain = apply_mapping(cipher, trial_key)
        sc = total_score(trial_plain)

        if sc > best_sc:
            best_sc = sc
            best_key = trial_key
            best_plain = trial_plain

    # Output
    print("===== Decrypted guess (may be imperfect) =====")
    print(best_plain)
    print()
    print("===== Mapping (cipher letter -> plain letter) =====")
    for i in range(26):
        print(f"{chr(97+i)} -> {best_key[i]}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
