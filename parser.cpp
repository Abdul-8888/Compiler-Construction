#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <map>

using namespace std;

enum TokenType {
    T_INT, // 0: int
    T_ID, // 1: temp
    T_NUM, // 2: 123
    T_IF, // 3: if
    T_ELSE, // 4: else
    T_RETURN, // 5: return
    T_ASSIGN, // 6: =
    T_PLUS, // 7: +
    T_MINUS, // 8: -
    T_MUL, // 9: *
    T_DIV, // 10: /
    T_LPAREN, // 11: (
    T_RPAREN, // 12: )
    T_LBRACE, // 13: {
    T_RBRACE, // 14: }
    T_SEMICOLON, // 15: ;
    T_GT, // 16: >
    T_LT, // 17: <
    T_WHILE, // 18: while
    T_EOF, // 19: end of file
};


struct Token {
    TokenType type;
    string value;
};

class Lexer {

    private:
        string src;
        size_t pos;
        /*
        It hold positive values. 
        In C++, size_t is an unsigned integer data type used to represent the 
        size of objects in bytes or indices, especially when working with memory-related 
        functions, arrays, and containers like vector or string. You can also use the int data type but size_t is recommended one
        */

    public:
        Lexer(const string &src) {
            this->src = src;  
            this->pos = 0;    
        }

        vector<Token> tokenize() {
            vector<Token> tokens;
            while (pos < src.size()) {
                char current = src[pos];
                
                if (isspace(current)) {
                    pos++;
                    continue;
                }
                if (isdigit(current)) {
                    tokens.push_back(Token{T_NUM, consumeNumber()});
                    continue;
                }
                if (isalpha(current)) {
                    string word = consumeWord();
                    if (word == "int") tokens.push_back(Token{T_INT, word});
                    else if (word == "if") tokens.push_back(Token{T_IF, word});
                    else if (word == "else") tokens.push_back(Token{T_ELSE, word});
                    else if (word == "return") tokens.push_back(Token{T_RETURN, word});
                    else if (word == "while") tokens.push_back(Token{T_WHILE, word});
                    else tokens.push_back(Token{T_ID, word});
                    continue;
                }
                
                switch (current) {
                        case '=': tokens.push_back(Token{T_ASSIGN, "="}); break;
                        case '+': tokens.push_back(Token{T_PLUS, "+"}); break;
                        case '-': tokens.push_back(Token{T_MINUS, "-"}); break;
                        case '*': tokens.push_back(Token{T_MUL, "*"}); break;
                        case '/': tokens.push_back(Token{T_DIV, "/"}); break;
                        case '(': tokens.push_back(Token{T_LPAREN, "("}); break;
                        case ')': tokens.push_back(Token{T_RPAREN, ")"}); break;
                        case '{': tokens.push_back(Token{T_LBRACE, "{"}); break;  
                        case '}': tokens.push_back(Token{T_RBRACE, "}"}); break;  
                        case ';': tokens.push_back(Token{T_SEMICOLON, ";"}); break;
                        case '>': tokens.push_back(Token{T_GT, ">"}); break;
                        case '<': tokens.push_back(Token{T_LT, "<"}); break;
                        default: cout << "Unexpected character: " << current << endl; exit(1);
                }
                pos++;
            }
            tokens.push_back(Token{T_EOF, ""});
            return tokens;
        }


        string consumeNumber() {
            size_t start = pos;
            while (pos < src.size() && isdigit(src[pos])) pos++;
            return src.substr(start, pos - start);
        }

        string consumeWord() {
            size_t start = pos;
            while (pos < src.size() && isalnum(src[pos])) pos++;
            return src.substr(start, pos - start);
        }
};


class Parser {
 

public:
    Parser(const vector<Token> &tokens) {
        this->tokens = tokens;  
        this->pos = 0;          
    }

    void parseProgram() {
        while (tokens[pos].type != T_EOF) {
            parseStatement();
        }
        cout << "Parsing completed successfully! No Syntax Error" << endl;
    }

private:
    vector<Token> tokens;
    size_t pos;

    void parseStatement() {
        if (tokens[pos].type == T_INT) {
            parseDeclaration();
        } else if (tokens[pos].type == T_ID) {
            parseAssignment();
        } else if (tokens[pos].type == T_IF) {
            parseIfStatement();
        } else if (tokens[pos].type == T_WHILE) {
            parseWhileStatement();
        } else if (tokens[pos].type == T_RETURN) {
            parseReturnStatement();
        } else if (tokens[pos].type == T_LBRACE) {  
            parseBlock();
        } else {
            cout << "Syntax error: unexpected token " << tokens[pos].value << endl;
            exit(1);
        }
    }

    void parseBlock() {
        expect(T_LBRACE);  
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF) {
            parseStatement();
        }
        expect(T_RBRACE);
    }

    void parseDeclaration() {
        expect(T_INT);
        expect(T_ID);
        if (tokens[pos].type == T_ASSIGN)
        {
            expect(T_ASSIGN);
            parseExpression();
        }
        expect(T_SEMICOLON);
    }

    void parseAssignment() {
        expect(T_ID);
        expect(T_ASSIGN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseIfStatement() {
        expect(T_IF);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        parseStatement();  
        if (tokens[pos].type == T_ELSE) {
            expect(T_ELSE);
            parseStatement();  
        }
    }

    void parseWhileStatement() {
        expect(T_WHILE);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        parseStatement();
    }

    void parseReturnStatement() {
        expect(T_RETURN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseExpression() {
        parseTerm();
        while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS) {
            pos++;
            parseTerm();
        }
        if (tokens[pos].type == T_GT || tokens[pos].type == T_LT) {
            pos++;
            parseExpression();  // After relational operator, parse the next expression
        }
    }

    void parseTerm() {
        parseFactor();
        while (tokens[pos].type == T_MUL || tokens[pos].type == T_DIV) {
            pos++;
            parseFactor();
        }
    }

    void parseFactor() {
        if (tokens[pos].type == T_NUM || tokens[pos].type == T_ID) {
            pos++;
        } else if (tokens[pos].type == T_LPAREN) {
            expect(T_LPAREN);
            parseExpression();
            expect(T_RPAREN);
        } else {
            cout << "Syntax error: unexpected token " << tokens[pos].value << endl;
            exit(1);
        }
    }

    void expect(TokenType type) {
        if (tokens[pos].type == type) {
            pos++;
        } else {
            cout << "Syntax error: expected " << type << " but found " << tokens[pos].value << endl;
            exit(1);
        }
    }
};

int main() {
    string input = R"(
        int i = 0;
        int a;
        a = 5;
        int b;
        b = a + 10;
        while (a < b) {
            return a;
        }
        if (b > 10) {
            return b;
        } else {
            return 0;
        }
    )";

    Lexer lexer(input);
    vector<Token> tokens = lexer.tokenize();
    
    Parser parser(tokens);
    parser.parseProgram();

    return 0;
}
