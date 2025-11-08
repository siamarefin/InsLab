// caesar_cracker.cpp (paraphrased version)
// Attempts all Caesar shifts, scores candidates by common words,
// prints every candidate and the best-scoring one at the end.

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

using std::string;

static inline char rotate_alpha(char ch, int shift_back) {
    // Reverse the Caesar shift by 'shift_back' positions, preserving case.
    if (std::isalpha(static_cast<unsigned char>(ch))) {
        char base = std::isupper(static_cast<unsigned char>(ch)) ? 'A' : 'a';
        int idx = ch - base;                     // 0..25
        int dec = (idx - shift_back) % 26;       // may be negative
        if (dec < 0) dec += 26;
        return static_cast<char>(base + dec);
    }
    return ch; // non-letters unchanged
}

string decode_with_shift(const string& s, int shift) {
    string out;
    out.reserve(s.size());
    for (char ch : s) {
        out.push_back(rotate_alpha(ch, shift));
    }
    return out;
}

int heuristic_score(const string& text) {
    // Count occurrences of frequent function words to approximate English-likeness.
    static const std::vector<string> probes = {
        " the ", " and ", " to ", " of ", " that ", " is ", " in ", " it "
    };
    // Surround with spaces so matches at boundaries can be found as " word ".
    string padded = " " + text + " ";
    int score = 0;
    for (const auto& w : probes) {
        std::size_t pos = 0;
        while ((pos = padded.find(w, pos)) != string::npos) {
            ++score;
            pos += w.size();
        }
    }
    return score;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    string cipher;
    if (!std::getline(std::cin, cipher) || cipher.empty()) {
        std::cerr << "Provide ciphertext as a single line on stdin.\n";
        return 0;
    }

    // Store (score, plaintext) pairs to select a best candidate later.
    std::vector<std::pair<int, string>> candidates;
    candidates.reserve(26);

    for (int s = 0; s < 26; ++s) {
        string guess = decode_with_shift(cipher, s);
        int sc = heuristic_score(guess);
        candidates.emplace_back(sc, guess);

        std::cout << "Shift " << s << ":\n" << guess << "\n\n";
    }

    // Choose the candidate with the largest score
    auto best_it = std::max_element(
        candidates.begin(), candidates.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; }
    );

    std::cout << "====== Best candidate (simple score) ======\n";
    std::cout << best_it->second << "\n";

    return 0;
}
