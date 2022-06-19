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

string Node::getCommand(Node *root) {
    string ret;
    if (root->type == COMMAND_NODE) {
        for (auto i: root->children) {
            ret += *(string *)i->obj;
            ret += " ";
        }
        for (int i = 0; i < 6; i++) {
            string redirect_symbol;
            switch(i) {
            case 0:
                redirect_symbol = " < ";
                break;
            case 1:
                redirect_symbol = " > ";
                break;
            case 2:
                redirect_symbol = " 2> ";
                break;
            case 3:
                redirect_symbol = " >& ";
                break;
            case 4:
                redirect_symbol = " >> ";
                break;
            case 5:
                redirect_symbol = " >>& ";
                break;
            }
            vector <string> files = ((Command *)(root->obj))->files[i];
            if (files.size()) {
            ret += redirect_symbol;
            for (unsigned int j = 0; j < files.size() - 1; j++) {
                ret += files[j];
                ret += redirect_symbol;
            }
            ret += files[files.size() - 1];
            }

        }
        return ret;
    }
    ret += getCommand(root->children[0]);
    ret += "|";
    ret += getCommand(root->children[1]);
    return ret;
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

