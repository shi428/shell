#include <parser.h>
#include <command.h>

Node::node(vector <Token> &t, vector <Token> &files) {
    background = 0;
    if (t[0].type == COMMAND || t[0].type == QUOTES) {
        token.type = COMMAND;
        obj = new Command();
        ((Command *)obj)->parseCommand(t);
        ((Command *)obj)->parseFileList(files);
    }
    else {
        token.type = PIPE;
        obj = new char[t[0].lexeme.length() + 1];
        strcpy((char *)obj, t[0].lexeme.c_str());
    }
    left = NULL;
    right = NULL;
}

void Node::traverse(int spaces) {
    indent(spaces);
    if (this->token.type == PIPE) {
        cout << "PIPE(" << endl;
        this->left->traverse(spaces + 2);
        this->right->traverse(spaces + 2);
        indent(spaces);
        cout << ")" << endl;
    }
    else {
        cout << "COMMAND(" << endl;
        ((Command *)obj)->printCommand(spaces + 2);
        indent(spaces);
        cout << ")" << endl;
    }
}

Node::~node() {
    if (token.type == COMMAND)
        delete (Command *)obj; 
    else delete [] (char *)obj;
}

Node *parseCommand(vector <Token> &tokens, unsigned int *index) {
    vector <Token> files;
    vector <Token> command;
    if (!tokens.size()) return NULL;
    Token token = tokens[*index];
    (*index)++;
    while (*index < tokens.size() && tokens[*index].type != COMMAND && tokens[*index].type != PIPE && tokens[*index].type != AMPERSAND) {
        if (tokens[*index].type > 0 && tokens[*index].type < 7) {
            files.push_back(tokens[*index]);
            (*index)++;
        }
        else {
            command.push_back(tokens[*index]);
            (*index)++;
        }
    }
    return new Node (tokens, files);
}

Node *parsePipe(vector <Token> &tokens, unsigned int *index) {
    Token token = tokens[*index];
    if (token.type != PIPE) return NULL;
    (*index)++;
    return new Node(tokens, tokens);
}

Tree::tree() {
    root = NULL;
}

void Tree::freeNodes(Node *node) {
    if (!node) return;
    freeNodes(node->left);
    freeNodes(node->right);
    delete node;
}

Tree::~tree() {
    freeNodes(root); 
}

Tree* newTree(vector <Token> &t) {
    Tree *ret = new Tree;
    Node *left = NULL;
    unsigned int index = 0;
    ret->root = parseCommand(t, &index);
    while (index < t.size()) {
        left = ret->root;
        if (t[index].type == PIPE) {
            ret->root = parsePipe(t, &index);
            ret->root->left = left;
            ret->root->right = parseCommand(t, &index);
            //    ret->root->right;
        }
        if (index == t.size() - 1 && t[index].type == AMPERSAND) {
            index++;
            ret->root->background = 1;
        }
    }
    return ret;
}
