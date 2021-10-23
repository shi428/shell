#ifndef TRIE_H
#define TRIE_H
#define ALPHABET_LEN 255 - 32 + 1

#include <string>
#include <iostream>
#include <cstring>
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
    void insert(std::string &key);
    void traverse(TrieNode *root, std::string &prefix);
    TrieNode* search(std::string &key);
    std::string try_complete(std::string &key);
};
#endif
