#include <shell_util.h>

using namespace std;
Node *make_cmd_subst_node(Node* command_line) {
    Node *newNode = new Node();
    AST *tr = new AST;
    tr->root = command_line;
    newNode->obj = tr;
    newNode->type = CMD_SUBST_NODE;
    return newNode;
}
