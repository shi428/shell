#include <bits/stdc++.h>
#include <trie.h>

using namespace std;
extern string read_line();
vector <string> history;
int ind;
void set_tty_raw_mode();
void tty_reset();
Trie *trie = NULL;
extern string current;
int main() {
    trie = new Trie;
    string line;
    ifstream fin("/usr/share/dict/words");
    while (getline(fin, line)) {
        //        cout << line << endl;
        trie->insert(line);
    }
    fin.close();
    while (1) {
        cout << "line editor>";
        fflush(stdout);
        //string a;
        string a = read_line();
        if (!a.compare("")) break;
        if (a.compare("\n")) {
            history.push_back(a);
            ind = history.size();
        }
        a.pop_back();
        //if (!getline(cin, a)) break;
        //TrieNode *node = trie->search(a);
        //trie->traverse(node, a);
    }
    delete trie;
}
