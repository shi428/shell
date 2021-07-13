#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include <bits/stdc++.h>
typedef struct node {
    node(Token &t, vector <Token> &tokens);
    ~node();
    Token token;
    node *left;
    node *right;
    void *obj;

    void traverse(int spaces);
    int spaces;
}Node;

typedef struct tree {
    tree();
    ~tree();
    void freeNodes(Node *node);
    Node *root;
}Tree;

Tree *newTree(vector <Token> &t);
Node *parseCommand(vector <Token> &tokens, unsigned int *index);
Node *parsePipe(vector <Token> &tokens, unsigned int *index);
#endif
