
#include <algorithm>
#include <array>
#include <chrono>
#include <cctype>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

using std::array;
using std::pair;
using std::size_t;
using std::string;
using std::vector;

static inline string normalize_text(const string& s) {
    string t;
    t.reserve(s.size());
    for (char c : s) {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            t.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        } else if (c == ' ') {
            t.push_back(' ');
        }
        // else: drop other characters (keeps scoring simpler)
    }
    return t;
}

static inline array<int, 26> count_letters(const string& s) {
    array<int, 26> f{};
    f.fill(0);
    for (char c : s) {
        if (c >= 'a' && c <= 'z') f[c - 'a']++;
    }
    return f;
}

// Apply mapping: for each lowercase letter, replace with mapc[letter_index], pass other chars through.
static inline string apply_mapping(const string& ct, const array<char, 26>& mapc) {
    string out = ct;
    for (size_t i = 0; i < ct.size(); ++i) {
        char c = ct[i];
        if (c >= 'a' && c <= 'z') out[i] = mapc[c - 'a'];
        else out[i] = c;
    }
    return out;
}

static inline int word_hit_score(const string& s) {
    // Pad with spaces so we can match " the " at boundaries.
    static const vector<string> common = {
        " the ", " and ", " to ", " of ", " that ", " is ", " in ", " it ", " a "
    };
    string padded = " " + s + " ";
    int sc = 0;
    for (const auto& w : common) {
        size_t pos = 0;
        while ((pos = padded.find(w, pos)) != string::npos) {
            ++sc;
            pos += w.size();
        }
    }
    return sc;
}

static inline int total_score(const string& plaintext) {
    // Currently just word hits; can be extended with chi-square or n-grams.
    return word_hit_score(plaintext);
}

// Build a seed mapping by aligning ciphertext frequency rank to typical English frequency rank.
static array<char, 26> seed_mapping_by_frequency(const string& ct) {
    const string english_rank = "etaoinshrdlcumwfgypbvkjxqz";
    array<int, 26> f = count_letters(ct);

    vector<pair<int,int>> ranked; // (count, index)
    ranked.reserve(26);
    for (int i = 0; i < 26; ++i) ranked.emplace_back(f[i], i);
    std::sort(ranked.begin(), ranked.end(), [](const auto& a, const auto& b){
        if (a.first != b.first) return a.first > b.first; // descending by count
        return a.second < b.second; // tie-break by letter index
    });

    array<char, 26> mapc{};
    // Initialize with placeholders to be safe
    for (int i = 0; i < 26; ++i) mapc[i] = 'x';

    for (int i = 0; i < 26; ++i) {
        int cipher_idx = ranked[i].second;     // which cipher letter
        mapc[cipher_idx] = english_rank[i];    // map it to its English counterpart
    }
    return mapc;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    // Read whole stdin (multi-line allowed)
    string all, line;
    while (std::getline(std::cin, line)) {
        if (!all.empty()) all.push_back('\n');
        all += line;
    }
    if (all.empty()) {
        std::cerr << "Provide ciphertext via stdin\n";
        return 0;
    }

    // Normalize for easier scoring/mapping
    string cipher = normalize_text(all);

    // Seed key by frequency
    array<char, 26> best_key = seed_mapping_by_frequency(cipher);
    string best_plain = apply_mapping(cipher, best_key);
    int best_sc = total_score(best_plain);

    // Hill-climb with random swaps of key outputs
    std::mt19937 rng(static_cast<unsigned>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()
    ));

    const int MAX_ITERS = 30000;
    for (int it = 0; it < MAX_ITERS; ++it) {
        int a = rng() % 26;
        int b = rng() % 26;
        if (a == b) continue;

        array<char, 26> trial_key = best_key;
        std::swap(trial_key[a], trial_key[b]);

        string trial_plain = apply_mapping(cipher, trial_key);
        int sc = total_score(trial_plain);

        if (sc > best_sc) {
            best_sc = sc;
            best_key = trial_key;
            best_plain = std::move(trial_plain);
            // (Optional) You could also try a few extra greedy swaps here.
        }
    }

    // Output
    std::cout << "===== Decrypted guess (may be imperfect) =====\n";
    std::cout << best_plain << "\n\n";

    std::cout << "===== Mapping (cipher letter -> plain letter) =====\n";
    for (int i = 0; i < 26; ++i) {
        std::cout << static_cast<char>('a' + i) << " -> " << best_key[i] << "\n";
    }

    return 0;
}
