// substitution_solver.cpp
#include <bits/stdc++.h>
using namespace std;

string normalize(const string &s){
    string t;
    for(char c: s){
        if(isalpha(c)) t += tolower(c);
        else if(c==' ') t += ' ';
    }
    return t;
}

vector<double> eng_freq = {
    8.05, 1.67, 2.23, 5.10,12.22,2.14,2.30,6.62,6.28,0.19,0.95,4.08,
    2.33,6.95,7.63,1.66,0.09,5.29,6.02,9.67,2.92,0.82,2.60,0.11,2.04,0.06
};

string apply_map(const string &ct, const vector<char> &mapc){
    string res = ct;
    for(size_t i=0;i<ct.size();++i){
        char c = ct[i];
        if(c>='a' && c<='z') res[i] = mapc[c-'a'];
        else res[i] = c;
    }
    return res;
}

vector<int> freq_count(const string &s){
    vector<int> f(26,0);
    for(char c: s) if(c>='a' && c<='z') f[c-'a']++;
    return f;
}

int word_score(const string &s){
    // count common words occurrences
    vector<string> words = {" the "," and "," to "," of "," that "," is "," in "," it "," a "};
    string T = " " + s + " ";
    int sc=0;
    for(auto &w: words){
        size_t pos=0;
        while((pos=T.find(w,pos))!=string::npos){ sc++; pos+=w.size(); }
    }
    return sc;
}

int score_text(const string &plain){
    int sc = word_score(plain);
    return sc;
}

vector<char> initial_map_by_freq(const string &ct){
    auto f = freq_count(ct);
    vector<pair<int,int>> v;
    for(int i=0;i<26;++i) v.push_back({f[i], i});
    sort(v.begin(), v.end(), greater<>());
    // english letters by frequency order:
    string eng_order = "etaoinshrdlcumwfgypbvkjxqz";
    vector<char> mapc(26);
    for(int i=0;i<26;++i){
        int cidx = v[i].second;
        mapc[cidx] = eng_order[i];
    }
    for(char &c: mapc) if(!isalpha(c)) c = 'x';
    return mapc;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string line, ciphertext;
    // Read entire input
    while(getline(cin, line)){
        if(!ciphertext.empty()) ciphertext += '\n';
        ciphertext += line;
    }
    if(ciphertext.empty()){ cerr<<"Provide ciphertext via stdin\n"; return 0; }
    ciphertext = normalize(ciphertext);

    // initial mapping
    vector<char> best_map = initial_map_by_freq(ciphertext);
    string best_plain = apply_map(ciphertext, best_map);
    int best_score = score_text(best_plain);

    // hill-climbing random swaps
    std::mt19937 rng((unsigned)chrono::high_resolution_clock::now().time_since_epoch().count());
    for(int iter=0; iter<30000; ++iter){
        // propose swap two letters in map
        int a = rng()%26, b = rng()%26;
        if(a==b) continue;
        vector<char> trial = best_map;
        // swap mapped outputs for letters a and b
        swap(trial[a], trial[b]);
        string trial_plain = apply_map(ciphertext, trial);
        int sc = score_text(trial_plain);
        if(sc > best_score){
            best_score = sc;
            best_map = trial;
            best_plain = trial_plain;
            // optional: improve faster by additional random changes
        }
    }

    cout<<"===== Decrypted guess (may be imperfect) =====\n";
    cout<<best_plain<<"\n\n";
    cout<<"===== Mapping (cipher letter -> plain letter) =====\n";
    for(int i=0;i<26;++i){
        cout<<char('a'+i)<<" -> "<<best_map[i]<<"\n";
    }
    return 0;
}
