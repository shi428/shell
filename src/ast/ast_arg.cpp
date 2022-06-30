#include <shell_util.h>

using namespace std;
Node *make_arg_node(string &arg, vector <char> &expansionFlags) {
    Node *newNode = new Node();
    newNode->obj = new string(arg);
    newNode->type = ARG_NODE;
    for (auto i: expansionFlags) {
        newNode->expansionFlags.push_back(i);
    }
    return newNode;
}
