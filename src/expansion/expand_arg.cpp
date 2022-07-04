#include <shell.h>

string expand_arg(Node *argNode, string &command) {
    string expandedArg;
    size_t index = 0;
    vector <char> tokens = argNode->expansionFlags;
    while (index < tokens.size()) {
        if (tokens[index] == 1) {
            int delimIndex = find(tokens.begin() + index + 1, tokens.end(), 1) - tokens.begin();
            string subshell_command = ((string *)argNode->obj)->substr(index + 2, delimIndex - (index + 2));
            expandedArg += expand_command_subst(subshell_command);
            index = delimIndex;
        }
        else if (tokens[index] == 2) {
            int delimIndex = find(tokens.begin() + index + 1, tokens.end(), 2) - tokens.begin();
            string subshell_command = ((string *)argNode->obj)->substr(index + 2, delimIndex - (index + 2));
            expandedArg += expand_process_subst(subshell_command, true);
            index = delimIndex;
        }
        else if (tokens[index] == 3) {
            int delimIndex = find(tokens.begin() + index + 1, tokens.end(), 3) - tokens.begin();
            string subshell_command = ((string *)argNode->obj)->substr(index + 2, delimIndex - (index + 2));
            expandedArg += expand_process_subst(subshell_command, false);
            index = delimIndex;
        }
        else {
            expandedArg += (*(string *)argNode->obj)[index];
        }
        index++;
    }
    command += *(string *)argNode->obj;
    command += " ";
    expandedArg += " ";
    return expandedArg;
}
