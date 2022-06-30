#include <shell_util.h>
Node::Node() {
    obj = NULL;
    background = 0;
}

Node::~Node() {
    if (obj) 
        switch (type) {
            case ARG_NODE: 
                delete (string*)obj; 
                break;
            case COMMAND_NODE:
                delete (Command*)obj;
                break;
            default: break;
        }
}

AST::AST() {
    root = NULL;
}

string Node::get_command(Node *root) {
    string ret;
    if (root->type == COMMAND_NODE) {
        for (auto i: root->children) {
            ret += *(string *)i->obj;
            ret += " ";
        }
        for (int i = 0; i < 6; i++) {
            string redirectSymbol;
            switch(i) {
            case 0:
                redirectSymbol = " < ";
                break;
            case 1:
                redirectSymbol = " > ";
                break;
            case 2:
                redirectSymbol = " 2> ";
                break;
            case 3:
                redirectSymbol = " >& ";
                break;
            case 4:
                redirectSymbol = " >> ";
                break;
            case 5:
                redirectSymbol = " >>& ";
                break;
            }
            vector <Node*> files = ((Command *)(root->obj))->files[i];
            if (files.size()) {
            ret += redirectSymbol;
            for (unsigned int j = 0; j < files.size() - 1; j++) {
                ret += *(string*)files[j]->obj;
                ret += redirectSymbol;
            }
            ret += *(string*)files[files.size() - 1]->obj;
            }

        }
        return ret;
    }
    ret += get_command(root->children[0]);
    ret += "|";
    ret += get_command(root->children[1]);
    return ret;
}
void AST::free_nodes(Node *root) {
    if (!root) return;
    for (auto i: root->children) {
    free_nodes(i);
    }
    delete root;
}

AST::~AST() {
    free_nodes(root); 
}

