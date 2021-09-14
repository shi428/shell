#include <parser.h>
#include <command.h>
Node::node() {
    obj = NULL;
    left = NULL;
    right = NULL;
    background = 0;
}
void Node::traverse(int spaces) {
    //indent(spaces);
    /*if (this->token.type == PIPE) {
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
    }*/
}

Node::~node() {
    if (obj) delete (Command *)obj;
}

/*Node *parseCommand(vector <Token> &tokens, unsigned int *index) {
    vector <Token> files;
    vector <Token> command;
    bool flag = 0;
    if (!tokens.size()) return NULL;
    // Token token = tokens[*index];
    // (*index)++;
    if (tokens[*index].type == QUOTES) {
        flag = 1;
    }
    command.push_back(tokens[*index]);
    (*index)++;
    while (*index < tokens.size() && tokens[*index].type != PIPE && tokens[*index].type != AMPERSAND) {
        if (tokens[*index].type > 0 && tokens[*index].type < 7) {
            if (*index + 1 < tokens.size() && tokens[*index + 1].type == QUOTES) {
                tokens[*index + 1].type = tokens[*index].type;
                if (tokens[*index].type < 3) {
                tokens[*index + 1].lexeme.insert(tokens[*index + 1].lexeme.begin(), tokens[*index].lexeme[0]);
                }
                else if(tokens[*index].type > 2 && tokens[*index].type < 5) {
                tokens[*index + 1].lexeme.insert(tokens[*index + 1].lexeme.begin(), tokens[*index].lexeme.begin(), tokens[*index].lexeme.begin() + 2);
                }
                else {
                tokens[*index + 1].lexeme.insert(tokens[*index + 1].lexeme.begin(), tokens[*index].lexeme.begin(), tokens[*index].lexeme.begin() + 3);
                }
                files.push_back(tokens[++(*index)]);
                (*index)++;
            }
            else {
            files.push_back(tokens[*index]);
            (*index)++;
            }
        }
        else {
            if (tokens[*index].type == QUOTES) {
                command.push_back(tokens[*index]);
                (*index)++;
                flag = 1;
            } 
            else if (tokens[*index].type == COMMAND) {
                if (flag) {
                    command.push_back(tokens[*index]);
                    (*index)++;
                }
                else {
                    break;
                }
            }
        }
    }
    return new Node (command, files);
}

Node *parsePipe(vector <Token> &tokens, unsigned int *index) {
    vector <Token> ts(tokens.begin() + *index, tokens.end());
    Token token = tokens[*index];
    if (token.type != PIPE) return NULL;
    (*index)++;
    return new Node(ts, tokens);
}*/

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

/*Tree* newTree(vector <Token> &t) {
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
}*/
