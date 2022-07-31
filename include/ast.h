#ifndef AST_H
#define AST_H
#include <vector>
#include <string>
using namespace std;
typedef enum type {
        COMMAND_NODE= 0,
        PIPE_NODE = 1,
        AND_NODE = 2,
        OR_NODE = 3,
        ARG_NODE = 4,
        CMD_SUBST_NODE = 5,
}Type;

struct Node {
    int background;
    Node();
    ~Node();
    string get_command(Node *root);
    void *obj;
    Type type;
    vector <Node *> children;
    vector <char> expansionFlags;
};

struct AST {
    AST();
    ~AST();
    void free_nodes(Node  *root);
    Node  *root;
};

Node *make_arg_node(string &arg, vector <char> &expansionFlags);
#endif
