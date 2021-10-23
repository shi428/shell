#include <shell.h>

using namespace std;
Node *make_subshell_node(Node* command_line) {
    Node *newNode = new Node();
    AST *tr = new AST;
    tr->root = command_line;
    newNode->obj = tr;
    newNode->type = SUBSHELL_NODE;
    return newNode;
}
