#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

string normalize(const string &s) {
    string t;
    for (char c : s) {
        if (isalpha(c)) 
            t += tolower(c);  // Convert all letters to lowercase
        else if (c == ' ') 
            t += ' ';  // Preserve spaces
    }
    return t;
}

vector<double> eng_freq = {
    8.05, 1.67, 2.23, 5.10, 12.22, 2.14, 2.30, 6.62, 6.28, 0.19, 0.95, 4.08,
    2.33, 6.95, 7.63, 1.66, 0.09, 5.29, 6.02, 9.67, 2.92, 0.82, 2.60, 0.11, 2.04, 0.06
};

// Function to apply a letter mapping to the ciphertext
string apply_map(const string &ct, const vector<char> &mapc) {
    string res = ct;
    for (size_t i = 0; i < ct.size(); ++i) {
        char c = ct[i];
        if (c >= 'a' && c <= 'z') 
            res[i] = mapc[c - 'a'];  // Map the character using the mapping
        else 
            res[i] = c;  // Non-alphabet characters remain unchanged
    }
    return res;
}

// Function to count letter frequency in the ciphertext
vector<int> freq_count(const string &s) {
    vector<int> f(26, 0);  // Initialize a vector to store the frequency of each letter
    for (char c : s) 
        if (c >= 'a' && c <= 'z') 
            f[c - 'a']++;  // Increment frequency for the letter
    return f;
}

// Function to score the text based on common English words
int word_score(const string &s) {
    vector<string> words = {" the ", " and ", " to ", " of ", " that ", " is ", " in ", " it ", " a "};  // Common words to search for
    string T = " " + s + " ";  // Add spaces at the start and end of the text
    int sc = 0;
    for (auto &w : words) {
        size_t pos = 0;
        while ((pos = T.find(w, pos)) != string::npos) {  // Find the common word in the text
            sc++;
            pos += w.size();  // Move past the word that was found
        }
    }
    return sc;
}

// Function to score the entire text based on word occurrence
int score_text(const string &plain) {
    return word_score(plain);  // Score the text based on common word frequency
}

// Function to generate an initial mapping based on letter frequency analysis
vector<char> initial_map_by_freq(const string &ct) {
    auto f = freq_count(ct);  // Get the letter frequency in the ciphertext
    vector<pair<int, int>> v;
    for (int i = 0; i < 26; ++i) 
        v.push_back({f[i], i});  // Create pairs of frequency and letter index
    sort(v.begin(), v.end(), greater<>());  // Sort by frequency, highest first

    // English letters by frequency order
    string eng_order = "etaoinshrdlcumwfgypbvkjxqz";
    vector<char> mapc(26);
    for (int i = 0; i < 26; ++i) {
        int cidx = v[i].second;
        mapc[cidx] = eng_order[i];  // Map the most frequent letters to common English letters
    }
    return mapc;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string line, ciphertext;
    // Read entire input ciphertext
    while (getline(cin, line)) {
        if (!ciphertext.empty()) ciphertext += '\n';
        ciphertext += line;
    }

    if (ciphertext.empty()) { 
        cerr << "Provide ciphertext via stdin\n"; 
        return 0; 
    }

    ciphertext = normalize(ciphertext);  // Normalize the ciphertext to lowercase and remove non-alphabet characters

    // Generate an initial mapping based on frequency analysis
    vector<char> best_map = initial_map_by_freq(ciphertext);
    string best_plain = apply_map(ciphertext, best_map);
    int best_score = score_text(best_plain);  // Score the initial guess

    // Hill-climbing random swaps to improve the decryption
    mt19937 rng((unsigned)chrono::high_resolution_clock::now().time_since_epoch().count());  // Initialize random number generator
    for (int iter = 0; iter < 30000; ++iter) {
        // Propose a swap between two letters in the map
        int a = rng() % 26, b = rng() % 26;
        if (a == b) continue;  // Skip if both are the same letter
        vector<char> trial = best_map;
        swap(trial[a], trial[b]);  // Swap the letters in the trial map
        string trial_plain = apply_map(ciphertext, trial);  // Apply the new mapping
        int sc = score_text(trial_plain);  // Score the trial plaintext
        if (sc > best_score) {
            best_score = sc;
            best_map = trial;
            best_plain = trial_plain;  // If the score is better, accept the trial mapping
        }
    }

    // Output the best decrypted guess
    cout << "===== Decrypted guess (may be imperfect) =====\n";
    cout << best_plain << "\n\n";

    // Output the mapping from cipher text to plain text
    cout << "===== Mapping (cipher letter -> plain letter) =====\n";
    for (int i = 0; i < 26; ++i) {
        cout << char('a' + i) << " -> " << best_map[i] << "\n";  // Show the mapping for each letter
    }

    return 0;
}
