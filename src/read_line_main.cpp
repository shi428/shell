#include <bits/stdc++.h>

using namespace std;
extern string read_line();
vector <string> history;
int ind;
int main() {
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
        //if (!getline(cin, a)) break;
        cout << a;
    }
}
