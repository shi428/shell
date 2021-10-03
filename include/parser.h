#ifndef PARSER_H
#define PARSER_H

    typedef enum type {
        COMMAND_NODE= 0,
        PIPE_NODE = 1,
        AND_NODE = 2,
        OR_NODE = 3,
    }Type;
typedef struct node {
    int background;
    node();
    ~node();
    node *left;
    node *right;
    void *obj;
    Type type;

    void traverse(int spaces);
    int spaces;
}Node;

typedef struct tree {
    tree();
    ~tree();
    void freeNodes(Node *node);
    Node *root;
}Tree;

/*Tree *newTree(vector <Token> &t);
Node *parseCommand(vector <Token> &tokens, unsigned int *index);
Node *parsePipe(vector <Token> &tokens, unsigned int *index);*/
#endif
