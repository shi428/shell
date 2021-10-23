#include <shell.h>

using namespace std;
Node *make_arg_node(string &arg) {
    Node *newNode = new Node();
    newNode->obj = new string(arg);
    newNode->type = ARG_NODE;
    return newNode;
}
