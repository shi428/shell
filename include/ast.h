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
        CMD_SUBST_NODE = 5,
}Type;

struct Node {
    int background;
    Node();
    ~Node();
    string getCommand(Node *root);
    void *obj;
    Type type;
    vector <Node *> children;
    vector <char> expansionType;
};

struct AST {
    AST();
    ~AST();
    string getCommand();
    void freeNodes(Node  *node);
    Node  *root;
};

Node *make_arg_node(string &arg, vector <char> &expansionTypes);
Node *make_cmd_subst_node(Node *command_line);
#endif
