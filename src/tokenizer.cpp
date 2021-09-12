#include <tokenizer.h>
#include <regex>

string createFileTokenEntry(StringIterator &it) {
    string ret;
    ret += it.advance();
    ret+= consumeSpaces(it);
    ret+= consumeChars(it, true, false, false, true);
    ret+= consumeSpaces(it);
    return ret;
}

bool detectError(StringIterator &it) {
    return (it.peek() == '2') && (it.lookahead(1) == '>');
}

Token next(StringIterator &it, bool escape) {
    Token ret;
    regex ch_reg("[^ \\{\\(\n\t]");
    regex newline("\n");
    string it_str = string(*it.it);
    //last char 
    if (it_str.length() == 1) {
        if (regex_match(it_str, newline)) {
            ret.type = NEWLINE;
            ret.lexeme = it.advance();
        }
    }
    else if (it.peek() == '\\') {
    if (regex_match(it_str.begin() + 1, it_str.begin()+2, ch_reg)) {
        ret.type = ESCAPE_CHAR; 
        ret.lexeme += it.advance();
        ret.lexeme += it.advance();
    }
    else {
        ret.type = CHAR; 
        ret.lexeme = it.advance();
    }
    }
    else if (regex_match(it_str.begin(), it_str.begin()+1, ch_reg)) {
        ret.type = CHAR; 
        ret.lexeme = it.advance();
    }
    else if (regex_match(it_str.begin(), it_str.begin()+1, newline)) {
            ret.type = NEWLINE; 
            ret.lexeme = it.advance();
    }
    else {
    it.advance();
    }
    /* if (it.peek() == '~') {
       ret.type = TILDE;
       ret.lexeme = it.advance();
       bool lookahead = it.lookahead(1) == '{' || it.lookahead(1) == '(';
       while (it.pos < it.len && it.peek() != '>' && it.peek() != '<' && it.peek() != '|' && it.peek() != '&' && (it.peek() != '$' || !lookahead) && it.peek() != '\'' && it.peek() != '\"') {
       ret.lexeme += consumeChar(it, true, NULL, true);
       if (it.peek() == '~') {
       ret.type = COMMAND;
       }
       lookahead = it.pos < it.len - 1 ? it.lookahead(1) == '{' || it.lookahead(1) == '(' : false;
       }
       }
       else if (it.peek() == '&') {
       ret.lexeme = it.advance();
       ret.type = AMPERSAND;
       consumeSpaces(it);
       }
       else if (it.peek() == '$' && it.lookahead(1) == '(') {
       stack <string> paren;
       paren.push("$(");
       it.advance();
       it.advance();
       ret.type = SUBSHELL;
    //string subshelltoken(*it.it);
    //unsigned int index = subshelltoken.find_last_of(')') + it.pos;
    while (it.pos < it.len) {
    if (it.peek() == '$' && it.lookahead(1) == '(') {
    paren.push("$(");
    }
    if (it.peek() == ')') {
    paren.pop();
    }
    if (paren.empty()) {
    it.advance();
    break;
    }
    ret.lexeme += it.advance();
    }
    }
    else if (it.peek() == '$' && it.lookahead(1) == '{') {
    stack <string> bracket;
    bracket.push("${");
    it.advance();
    it.advance();
    ret.type = ENV;
    //string subshelltoken(*it.it);
    //unsigned int index = subshelltoken.find_last_of(')') + it.pos;
    while (it.pos < it.len) {
    if (it.peek() == '$' && it.lookahead(1) == '{') {
    bracket.push("${");
    }
    if (it.peek() == '}') {
    bracket.pop();
    }
    if (bracket.empty()) {
    it.advance();
    break;
    }
    ret.lexeme += it.advance();
    }
    }
    else if (it.peek() == '\"') {
    ret.type = QUOTES;
    it.advance();
    consumeSpaces(it);
    while (it.pos < it.len) {
    ret.lexeme += consumeChars(it, true, false, true, escape);
    ret.lexeme += consumeSpaces(it);
    if (it.peek() == '\"') {
    it.advance();
    if (it.peek() == '\'' || it.peek() == '\"') {
    ret.lexeme += next(it, escape).lexeme;
}
string spaces = consumeSpaces(it);
if (it.pos < it.len) {//not at the end
    ret.lexeme += spaces;
}
return ret;
}
}
ret.type = ERROR;
ret.lexeme = "Missing \"";
}
else if (it.peek() == '\'') {
    ret.type = QUOTES;
    it.advance();
    consumeSpaces(it);
    while (it.pos < it.len) {
        ret.lexeme += consumeChars(it, true, true, false, escape);
        //cout << it.peek() << endl;
        ret.lexeme += consumeSpaces(it);
        if (it.peek() == '\'') {
            it.advance();
            if (it.peek() == '\'' || it.peek() == '\"') {
                ret.lexeme += next(it, escape).lexeme;
            }
            string spaces = consumeSpaces(it);
            if (it.pos < it.len) {//not at the end
                ret.lexeme += spaces;
            }
            return ret;
        }
    }
    ret.type = ERROR;
    ret.lexeme = "Missing \'";
}
else if (it.peek() == '|') {
    ret.lexeme = it.advance();
    ret.type = PIPE;
    consumeSpaces(it);
}
else if (detectError(it)) {
    ret.type = FILE_ERR;
    ret.lexeme += it.advance();
    ret.lexeme += createFileTokenEntry(it);
}
else if (it.peek() == '<') {
    ret.type = FILE_IN;
    ret.lexeme += createFileTokenEntry(it);
}
else if (it.peek() == '>') {
    if (it.lookahead(1)== '&')  {
        ret.type = FILE_OUT_ERR;
        ret.lexeme += it.advance();
        ret.lexeme += createFileTokenEntry(it);
    }
    else if (it.lookahead(1) == '>') {
        ret.lexeme += it.advance();
        if (it.lookahead(1) == '&') {
            ret.type = FILE_APPEND_ERR;
            ret.lexeme += it.advance();
            ret.lexeme += createFileTokenEntry(it);
        }
        else {
            ret.type = FILE_APPEND;
            ret.lexeme += createFileTokenEntry(it);
        }
    }
    else {
        ret.type = FILE_OUT;
        ret.lexeme += createFileTokenEntry(it);
    }
}
else {
    ret.type = COMMAND;
    //consumeSpaces(it);
    bool lookahead = it.lookahead(1) == '{' || it.lookahead(1) == '(';
    while (it.pos < it.len && it.peek() != '>' && it.peek() != '<' && it.peek() != '|' && it.peek() != '&' && (it.peek() != '$' || !lookahead) && it.peek() != '\'' && it.peek() != '\"') {
        ret.lexeme += consumeChars(it, false, false, false, escape);
        lookahead = it.pos < it.len - 1 ? it.lookahead(1) == '{' || it.lookahead(1) == '(' : false;
        if (it.pos == it.len || it.peek() == '>' || it.peek() == '<' || it.peek() == '|' || it.peek() == '&' || it.peek() == '\'' || it.peek() == '\"' || (it.peek() == '$' && lookahead))// reached end
            break;
        string spaces = consumeSpaces(it);
        if (spaces == "") {
            ret.type = ERROR;
            ret.lexeme = "no spaces";
            break;
        }
        else {
            if (it.peek() == '~') {
                ret.lexeme += spaces;
                break;
            }
        }
        if (detectError(it)) {
            break;
        }
        lookahead = it.lookahead(1) == '{' || it.lookahead(1) == '(';
        ret.lexeme += spaces;
    }
}*/
return ret;
}

void Token::printToken() {
    string commandType;
    switch (this->type) {
        case CHAR: 
            commandType = "CHAR";
            break;
        case ESCAPE_CHAR:
            commandType = "ESCAPE_CHAR";
            break;
        case NEWLINE:
            commandType = "NEWLINE";
            break;
        default: break;
    }
    cout << commandType << "(" << this->lexeme << ")" << endl;
}

vector <Token> genTokens(string &line, bool escape) {
    vector <Token> ret;
    removeWhiteSpace(line);
    stringIterator it(line);
    while (it.pos < it.len) {
        ret.push_back(next(it, escape));
    }
    //cout << ret.size() << endl;
    return ret;
}
