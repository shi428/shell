#include <shell_util.h>
Node::Node() {
    obj = NULL;
    background = 0;
}

Node::~Node() {
    if (obj) 
        switch (type) {
            case ARG_NODE: 
                delete obj; 
                break;
            case COMMAND_NODE:
                delete obj;
                break;
            default: break;
        }
}

AST::AST() {
    root = NULL;
}

void AST::freeNodes(Node *node) {
    if (!node) return;
    for (auto i: node->children) {
    freeNodes(i);
    }
    delete node;
}

AST::~AST() {
    freeNodes(root); 
}

