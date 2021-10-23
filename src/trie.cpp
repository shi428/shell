#include <shell.h>

using namespace std;
TrieNode::TrieNode() {
    n_children = 0;
    is_word = false;
    for (int i = 0; i < ALPHABET_LEN; i++) {
        children[i] = NULL;
    }
}

Trie::Trie() {
    root = new TrieNode();
}

void deleteHelper(TrieNode *root) {
    if (!root) return;
  /*  if (!root->n_children) {
        delete root;
        return ;
    }*/
    for (int i = 0; i < ALPHABET_LEN; i++) {
      //  if (root->children[i]) {
        deleteHelper(root->children[i]);
       // }
    }
    delete root;
}

Trie::~Trie() {
    deleteHelper(root);
}

void Trie::insert(std::string &key) {
    TrieNode *current = root;
    for (unsigned int i = 0; i < key.length(); i++) {
        int index = (unsigned char)key[i] - 0x20;
        if (index >= 0) {
        if (!current->children[index]) {
            current->children[index] = new TrieNode();
            current->n_children++;
        }
        current = current->children[index];
        }
    }
    current->is_word = true;
}

void Trie::traverse(TrieNode *root, std::string &prefix) {
    if (!root) return;
    if (!root->n_children && root->is_word) {
        std::cout << prefix << endl;
        return ;
    }
    if (root->is_word) {
        std::cout << prefix << endl;
    }
    for (int i = 0; i < ALPHABET_LEN; i++) {
        if (root->children[i]) {
            unsigned char c = i + 0x20;
            prefix += c;
            traverse(root->children[i], prefix);
            prefix.pop_back();
        }
    }
}

TrieNode* Trie::search(std::string &key) {
    TrieNode *current = root;
    for (unsigned int i = 0; i < key.length(); i++) {
        int index = (unsigned char)key[i] - 0x20;
        if (index >= 0) {
        if (!current->children[index]) {
            return NULL;
        }
        else {
            current = current->children[index];
        }
        }
    }
    return current;
}

std::string Trie::try_complete(std::string &input) {
    string ret = "";
    TrieNode *current = search(input);
    while (current && current->n_children == 1 && !current->is_word) {
        for (unsigned int i = 0; i < ALPHABET_LEN; i++) {
            if (current->children[i]) {
                current = current->children[i];
                ret += (char)(i + 0x20);
                break;
            }
        }
    }
    return ret;
}
