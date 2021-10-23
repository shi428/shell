#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include <string>
using namespace std;
typedef enum type {
        COMMAND_NODE= 0,
        PIPE_NODE = 1,
        AND_NODE = 2,
        OR_NODE = 3,
        ARG_NODE = 4,
}Type;

struct Node {
    int background;
    Node();
    ~Node();
    void *obj;
    Type type;
    vector <Node *> children;
};
struct AST {
    AST();
    ~AST();
    void freeNodes(Node *node);
    Node *root;
};

Node *make_arg_node(string &arg);
#endif
