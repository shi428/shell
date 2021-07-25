#include <parser.h>
#include <command.h>

Node::node(Token &t, vector <Token> &files) {
    background = 0;
    token = t;
    if (t.type == COMMAND) {
        obj = new Command();
        ((Command *)obj)->parseCommand(token);
        ((Command *)obj)->parseFileList(files);
    }
    else {
        obj = new char[t.lexeme.length() + 1];
        strcpy((char *)obj, t.lexeme.c_str());
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
    if (!tokens.size()) return NULL;
    Token token = tokens[*index];
    (*index)++;
    while (*index < tokens.size() && tokens[*index].type != COMMAND && tokens[*index].type != PIPE && tokens[*index].type != AMPERSAND) {
        files.push_back(tokens[*index]);
        (*index)++;
    }
    return new Node (token, files);
}

Node *parsePipe(vector <Token> &tokens, unsigned int *index) {
    Token token = tokens[*index];
    if (token.type != PIPE) return NULL;
    (*index)++;
    return new Node(token, tokens);
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
