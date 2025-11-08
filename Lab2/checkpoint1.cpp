// caesar_solver.cpp
#include <bits/stdc++.h>
using namespace std;

string shift_text(const string &s, int sh){
    string out = s;
    for(size_t i=0;i<s.size();++i){
        char c = s[i];
        if(c>='a' && c<='z') out[i] = char('a' + (c - 'a' + 26 - sh) % 26);
        else if(c>='A' && c<='Z') out[i] = char('A' + (c - 'A' + 26 - sh) % 26);
        else out[i] = c;
    }
    return out;
}

int score_text(const string &t){
    // simple scoring by counting common words
    vector<string> words = {" the ", " and ", " to ", " of ", " that ", " is ", " in ", " it "};
    string T = " " + t + " ";
    int score = 0;
    for(auto &w: words){
        size_t pos = 0;
        while((pos = T.find(w, pos)) != string::npos){
            ++score;
            pos += w.size();
        }
    }
    return score;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string cipher;
    // Read entire ciphertext from stdin
    getline(cin, cipher);
    if(cipher.size()==0){ cerr<<"Provide ciphertext as single line input\n"; return 0; }
    vector<pair<int,string>> cand;
    for(int sh=0; sh<26; ++sh){
        string plain = shift_text(cipher, sh);
        int sc = score_text(plain);
        cand.push_back({sc, plain});
        cout<<"Shift "<<sh<<":\n"<<plain<<"\n\n";
    }
    // show best by score
    auto best = max_element(cand.begin(), cand.end(), [](auto &a, auto &b){ return a.first < b.first; });
    cout<<"====== Best candidate (simple score) ======\n";
    cout<<best->second<<"\n";
    return 0;
}
