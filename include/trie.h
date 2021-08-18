#ifndef TRIE_H
#define TRIE_H
#define ALPHABET_LEN 255 - 32 + 1

#include <bits/stdc++.h>
using namespace std;
struct TrieNode {
    TrieNode();
    int n_children;
    TrieNode *children[ALPHABET_LEN];
    bool is_word;
};

struct Trie {
    Trie();
    ~Trie();
    TrieNode *root;
    void insert(string &key);
    void traverse(TrieNode *root, string &prefix);
    TrieNode* search(string &key);
    string try_complete(string &key);
};
#endif
