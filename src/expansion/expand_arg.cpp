#include <shell.h>

string expand_arg_it1(Node *argNode, string &command) {
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
        else if (tokens[index] == 4) {
            int delimIndex = find(tokens.begin() + index + 1, tokens.end(), 4) - tokens.begin();
            string env_var = ((string *)argNode->obj)->substr(index + 2, delimIndex - (index + 2));
            expandedArg += expand_env(env_var);
            index = delimIndex;
        }
        /*else if (tokens[index] == 6) {
          int delimIndex = find(tokens.begin() + index + 1, tokens.end(), 6) - tokens.begin();
          string wildcard = ((string *)argNode->obj)->substr(index + 2, delimIndex - (index + 2));
          bool pwd = !(wildcard.find('/') == 0);
          string full_wildcard = !pwd? wildcard : string(getenv("PWD")) +"/"+ wildcard;
          vector <string> args = expand_wildcard(full_wildcard, pwd);
          cout << wildcard << endl;
          for (auto i: args) {
          expandedArg += i; 
          expandedArg += " ";
          }
          if (args.size() == 0) {
          expandedArg += "\"" + wildcard + "\"";
          }
          index = delimIndex;
          }*/
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

string expand_arg_it2(Node *argNode, string &command) {
    string expandedArg;
    size_t index = 0;
    string original = *(string *)argNode->obj;
    vector <char> tokens = argNode->expansionFlags;
    // vector <char> tokens = get_tokens(*(string*)argNode->obj);
    //cout <<  << endl;
    //cout << *(string*)argNode->obj << endl;
    while (index < tokens.size()) {
        if (tokens[index] == 5) {
            //if just one char tilde
            string tilde;
            if (find(tokens.begin() + index + 1, tokens.end(), 5) == tokens.end()) {
                //index++;
            }
            else {
                int delimIndex = find(tokens.begin() + index + 1, tokens.end(), 5) - tokens.begin();
                vector <char> tokens = get_tokens(original);
                int quoteDelimIndex = find(tokens.begin() + index + 1, tokens.end(), 5) - tokens.begin();
                string evaluatedString = get_evaluated_string(original);
                tilde = evaluatedString.substr(index + 1, quoteDelimIndex - index + 1);
                tilde.pop_back();
                char c = tilde[0];
                if (c == '\'' || c == '\"') {
                    tilde.erase(tilde.begin());
                    tilde.erase(find(tilde.begin() + 1, tilde.end(), c));
                }
                index = delimIndex;
            }
            expandedArg += expand_tilde(tilde);
        }
        else {
            //expandedArg += (*(string *)argNode->obj)[index];
            expandedArg += original[index];
        }
        index++;
    }
    command += *(string *)argNode->obj;
    command += " ";
    expandedArg += " ";
    //cout << expandedArg << endl;
    return expandedArg;
}

//wildcards
string expand_arg_it3(Node *argNode, string &command) {
    string expandedArg;
    size_t index = 0;
    string original = *(string *)argNode->obj;
    vector <char> tokens = argNode->expansionFlags;
    //cout << *(string*)argNode->obj << endl;

    while (index < tokens.size()) {
        if (tokens[index] == QUOTE) {
            int delimIndex = find(tokens.begin() + index + 1, tokens.end(), QUOTE) - tokens.begin();
            expandedArg += original.substr(index, delimIndex - index + 1);
            index = delimIndex;
        }
        else {
            if (original[index] == '?' || original[index] == '*') {
                expandedArg = "";
                string evaluatedString = get_evaluated_string(original);
                evaluatedString.pop_back();
                string wildcard = evaluatedString;
                bool pwd = !(wildcard.find('/') == 0);
                string full_wildcard = !pwd? wildcard : string(getenv("PWD")) +"/"+ wildcard;
                vector <string> args = expand_wildcard(full_wildcard, pwd);
                for (auto i: args) {
                    expandedArg += i; 
                    expandedArg += " ";
                }
                if (args.size() == 0) {
                    expandedArg += "\"" + wildcard + "\"";
                }
                break;
            }
            else {
                expandedArg += original[index];
            }
        }
        index++;
    }
    command += *(string *)argNode->obj;
    command += " ";
    expandedArg += " ";
    //cout << expandedArg << endl;
    return expandedArg;
}
