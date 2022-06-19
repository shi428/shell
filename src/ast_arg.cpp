#include <shell_util.h>

using namespace std;
Node *make_arg_node(string &arg, vector <char> &expansionTypes) {
    Node *newNode = new Node();
    newNode->obj = new string(arg);
    newNode->type = ARG_NODE;
    for (auto i: expansionTypes) {
        newNode->expansionType.push_back(i);
    }
    return newNode;
}
