#include <shell.h>

string try_expand_command_it1(Node *node, string &command) {
    string expandedCommand;
    if (node->type == COMMAND_NODE) {
        for (auto i: node->children) {
            expandedCommand += expand_arg_it1(i, command);
        }

        //expand redirect file args
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
            vector <Node*> files = ((Command *)(node->obj))->files[i];
            if (files.size()) {
                expandedCommand += redirect_symbol;
                for (unsigned int j = 0; j < files.size() - 1; j++) {
                    expandedCommand += expand_arg_it1(files[j], command);
                    expandedCommand += redirect_symbol;
                }
                expandedCommand += expand_arg_it1(files[files.size() - 1], command);
            }

        }
        return expandedCommand;
    }
    expandedCommand += try_expand_command_it1(node->children[0], command);
    command += " | ";
    expandedCommand += " | ";
    expandedCommand += try_expand_command_it1(node->children[1], command);
    return expandedCommand;
}

string try_expand_command_it2(Node *node, string &command) {
    string expandedCommand;
    if (node->type == COMMAND_NODE) {
        for (auto i: node->children) {
            expandedCommand += expand_arg_it2(i, command);
        }

        //expand redirect file args
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
            vector <Node*> files = ((Command *)(node->obj))->files[i];
            if (files.size()) {
                expandedCommand += redirect_symbol;
                for (unsigned int j = 0; j < files.size() - 1; j++) {
                    expandedCommand += expand_arg_it2(files[j], command);
                    expandedCommand += redirect_symbol;
                }
                expandedCommand += expand_arg_it2(files[files.size() - 1], command);
            }

        }
        return expandedCommand;
    }
    expandedCommand += try_expand_command_it2(node->children[0], command);
    command += " | ";
    expandedCommand += " | ";
    expandedCommand += try_expand_command_it2(node->children[1], command);
    return expandedCommand;
}

string try_expand_command_it3(Node *node, string &command) {
    string expandedCommand;
    if (node->type == COMMAND_NODE) {
        for (auto i: node->children) {
            expandedCommand += expand_arg_it3(i, command);
        }

        //expand redirect file args
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
            vector <Node*> files = ((Command *)(node->obj))->files[i];
            if (files.size()) {
                expandedCommand += redirect_symbol;
                for (unsigned int j = 0; j < files.size() - 1; j++) {
                    expandedCommand += expand_arg_it3(files[j], command);
                    expandedCommand += redirect_symbol;
                }
                expandedCommand += expand_arg_it3(files[files.size() - 1], command);
            }

        }
        return expandedCommand;
    }
    expandedCommand += try_expand_command_it3(node->children[0], command);
    command += " | ";
    expandedCommand += " | ";
    expandedCommand += try_expand_command_it3(node->children[1], command);
    return expandedCommand;
}
