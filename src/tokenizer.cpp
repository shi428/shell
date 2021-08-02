#include <tokenizer.h>

string createFileTokenEntry(StringIterator &it) {
    string ret;
    ret += it.advance();
    ret+= consumeSpaces(it);
    ret+= consumeChars(it, true, false);
    ret+= consumeSpaces(it);
    return ret;
}

bool detectError(StringIterator &it) {
    return (it.peek() == '2') && (it.lookahead(1) == '>');
}

Token next(StringIterator &it) {
    Token ret;
    ret.lexeme = string();
    if (it.peek() == '&') {
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
    else if (it.peek() == '\"') {
        ret.type = QUOTES;
        it.advance();
        consumeSpaces(it);
        while (it.pos < it.len) {
            ret.lexeme += consumeChars(it, true, true);
            ret.lexeme += consumeSpaces(it);
            if (it.peek() == '\"') {
                it.advance();
                consumeSpaces(it);
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
            ret.lexeme += consumeChars(it, true, false);
            //cout << it.peek() << endl;
            ret.lexeme += consumeSpaces(it);
            if (it.peek() == '\'') {
                it.advance();
                consumeSpaces(it);
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
        while (it.pos < it.len && it.peek() != '>' && it.peek() != '<' && it.peek() != '|' && it.peek() != '&' && it.peek() != '$') {
            ret.lexeme += consumeChars(it, false, false);
            if (it.pos == it.len || it.peek() == '>' || it.peek() == '<' || it.peek() == '|' || it.peek() == '&' || it.peek() == '\'' || it.peek() == '\"' || it.peek() == '$')// reached end
                break;
            string spaces = consumeSpaces(it);
            if (spaces == "") {
                ret.type = ERROR;
                ret.lexeme = "no spaces";
                break;
            }
            if (detectError(it)) {
                break;
            }
            ret.lexeme += spaces;
        }
    }
    return ret;
}

void Token::printToken() {
    string commandType;
    switch (this->type) {
        case COMMAND: 
            commandType = "COMMAND";
            break;
        case FILE_OUT:
            commandType = "FILE_OUT";
            break;
        case FILE_IN: 
            commandType = "FILE_IN";
            break;
        case FILE_ERR:
            commandType = "FILE_ERR";
            break;
        case FILE_OUT_ERR:
            commandType = "FILE_OUT_ERR";
            break;
        case FILE_APPEND:
            commandType = "FILE_APPEND";
            break;
        case FILE_APPEND_ERR:
            commandType = "FILE_APPEND_ERR";
            break;
        case PIPE:
            commandType = "PIPE";
            break;
        case AMPERSAND:
            commandType = "AMPERSAND";
            break;
        case QUOTES:
            commandType = "QUOTES";
            break;
        case SUBSHELL:
            commandType = "SUBSHELL";
            break;
        case ERROR:
            commandType = "ERROR";
            break;
        default: break;
    }
    cout << commandType << "(" << this->lexeme << ")" << endl;
}

vector <Token> genTokens(string &line) {
    vector <Token> ret;
    removeWhiteSpace(line);
    stringIterator it(line);
    while (it.pos < it.len) {
        ret.push_back(next(it));
    }
    //cout << ret.size() << endl;
    return ret;
}
