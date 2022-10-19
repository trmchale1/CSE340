/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "IF", "WHILE", "DO", "THEN", "PRINT",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN",
    "NOTEQUAL", "GREATER", "LESS", "LTEQ", "GTEQ",
    "DOT", "NUM", "ID", "ERROR","REALNUM", "BASE08NUM", "BASE16NUM" // TODO: Add labels for new token types here (as string)
};

#define KEYWORDS_COUNT 5
string keyword[] = { "IF", "WHILE", "DO", "THEN", "PRINT" };

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

bool LexicalAnalyzer::isdigit8(char c){
    switch (c) {
        case '0':
            return true;
        case '1':
            return true;
        case '2':
            return true;
        case '3':
            return true;
        case '4':
            return true;
        case '5':
            return true;
        case '6':
            return true;
        case '7':
            return true;
        default:
            return false;
    }
}

bool LexicalAnalyzer::ispdigit8(char c)
{
        switch (c) {
        case '1':
            return true;
        case '2':
            return true;
        case '3':
            return true;
        case '4':
            return true;
        case '5':
            return true;
        case '6':
            return true;
        case '7':
            return true;
        default:
            return false;
    }
}

bool LexicalAnalyzer::ispdigit16(char c)
{
        switch (c) {
        case '1':
            return true;
        case '2':
            return true;
        case '3':
            return true;
        case '4':
            return true;
        case '5':
            return true;
        case '6':
            return true;
        case '7':
            return true;
        case '8':
            return true;
        case '9':
            return true;
        case 'A':
            return true;
        case 'B':
            return true;
        case 'C':
            return true;
        case 'D':
            return true;
        case 'E':
            return true;
        case 'F':
            return true;
        default:
            return false;
    }
}

bool LexicalAnalyzer::isdigit16(char c)
{
        switch (c) {
        case '0':
            return true;
        case '1':
            return true;
        case '2':
            return true;
        case '3':
            return true;
        case '4':
            return true;
        case '5':
            return true;
        case '6':
            return true;
        case '7':
            return true;
        case '8':
            return true;
        case '9':
            return true;
        case 'A':
            return true;
        case 'B':
            return true;
        case 'C':
            return true;
        case 'D':
            return true;
        case 'E':
            return true;
        case 'F':
            return true;
        default:
            return false;
    }
}

// The case 0x08 will fail if preceded by a nondigit8 
string LexicalAnalyzer::isBase8()
{
    char c;
    string lexeme;
    input.GetChar(c);
    lexeme += c;
    //cout << "First char into input base8 " << c << endl;
    if(isdigit8(c)){
        input.GetChar(c);
        while (!input.EndOfInput() && isdigit8(c)) {
            lexeme += c;
            input.GetChar(c);
            //cout << "Each additional char into input base8 " << c << endl;
        }
        lexeme += c;
        if(c == 'x'){
            //cout << "Did we catch x " << c << endl;
            input.GetChar(c);
            lexeme += c;
            if (c == '0') {
                //cout << "Did we catch 0 " << c << endl;
                input.GetChar(c);
                lexeme += c;
            }
            if (c == '8'){
                return lexeme;
            }
        } 
    } 

    //cout << "We failed somewhere and we are ungetting this string " << lexeme << endl;
    input.UngetString(lexeme);
    lexeme = "";
    return lexeme;
}

// The case 0x16 will fail if preceded by a nondigit16
string LexicalAnalyzer::isBase16()
{
    char c;
    string lexeme;
    input.GetChar(c);
    //cout << "First char from isBase16 " << c << endl;
    if(ispdigit16(c)){
        lexeme = "";
        while (!input.EndOfInput() && isdigit16(c)) {
            lexeme += c;
            //cout << "lexeme has been modified " << lexeme << endl;
            input.GetChar(c);
            //cout << "Each additional char from isBase16 " << c << endl;
        }
        lexeme += c;
        if(c == 'x'){
            //cout << "Did we find x " << c << endl;
            input.GetChar(c);
            lexeme += c;
            if (c == '1') {
                //cout << "Did we find 1 " << c << endl;

                input.GetChar(c);
                lexeme += c;
            }
            if (c == '6'){
                //cout << "Did we find 6 " << c << endl;
                return lexeme;
            }
        } 
    } 
    //cout << "We failed somewhere and we are ungetting this string " << lexeme << endl;
    input.UngetString(lexeme);
    lexeme = "";
    return lexeme;
}

string LexicalAnalyzer::isNumber()
{
    char c;
    string lexeme;
    input.GetChar(c);
    //cout << "This is the first char isNumber() " << c << endl;
    if(isdigit(c)){
        lexeme = "";
        while (!input.EndOfInput() && isdigit(c)) {
            lexeme += c;
            //cout << "lexeme has been modified" << lexeme << endl;
            input.GetChar(c);
            //cout << "Additional charchter from isNumber " << c << endl;
        }
        if(!isdigit(c)){
            input.UngetChar(c);
            return lexeme;
        }
    }
    //cout << "We screwed up and now we are ungetting lexeme " << lexeme << endl;
    input.UngetString(lexeme);
    lexeme = "";
    return lexeme;
}

string LexicalAnalyzer::isRealNum() 
{
      char c;
    string lexeme;
    input.GetChar(c);
    //cout << "This is the first char isRealNum() " << c << endl;
    if(isdigit(c)){
        lexeme = "";
        while (!input.EndOfInput() && isdigit(c)) {
            lexeme += c;
            //cout << "lexeme has been modified " << lexeme << endl;
            input.GetChar(c);
            //cout << "Additional charchter from isRealNum " << c << endl;
        }
        lexeme += c;
        if(c == '.'){
            //cout << "Did we find . " << c << endl;
            input.GetChar(c);
            if(isdigit(c)){
            while (!input.EndOfInput() && isdigit(c)) {
            lexeme += c;
            //cout << "lexeme has been modified" << lexeme << endl;
            input.GetChar(c);
            //cout << "Additional charchter from isRealNum " << c << endl;
            }
        if(!isdigit(c)){
            
            input.UngetChar(c);
        }
        } else {
            lexeme += c;
            //cout << "isrealnum() ungetting " << lexeme << endl;
            input.UngetString(lexeme);
            lexeme = "";
            return lexeme;
        }

        return lexeme;
    }
    //cout << "We screwed up and now we are ungetting lexeme " << lexeme << endl;
    input.UngetString(lexeme);
    lexeme = "";
    return lexeme;
    }
}

Token LexicalAnalyzer::ScanNumber()
{
    string tmp_lexeme;

    tmp_lexeme = isBase8();
    //cout << tmp_lexeme << " output from isBase8() " << endl;
    if(!tmp_lexeme.empty()){
        tmp.lexeme = tmp_lexeme;
        tmp.token_type = BASE08NUM;
        tmp.line_no = line_no;
        return tmp;
    } 
    tmp_lexeme = isBase16();
    //cout << tmp_lexeme << " output from isBase16() " << endl;
    if(!tmp_lexeme.empty()){
        tmp.lexeme = tmp_lexeme;
        tmp.token_type = BASE16NUM;
        tmp.line_no = line_no;
        return tmp;
    } 
    tmp_lexeme = isRealNum();
    //cout << tmp_lexeme << " output from isRealNum " << endl;
    if(!tmp_lexeme.empty()) {
        tmp.lexeme = tmp_lexeme;
        tmp.token_type = REALNUM;
        tmp.line_no = line_no;
        return tmp;
    }
    tmp_lexeme = isNumber();
    if(!tmp_lexeme.empty()) {
        tmp.lexeme = tmp_lexeme;
        tmp.token_type = NUM;
        tmp.line_no = line_no;
        return tmp;
    } 
        
    tmp.lexeme = tmp_lexeme;
    tmp.token_type = ERROR;
    tmp.line_no = line_no;
    return tmp;
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
        case '.':
            tmp.token_type = DOT;
            return tmp;
        case '+':
            tmp.token_type = PLUS;
            return tmp;
        case '-':
            tmp.token_type = MINUS;
            return tmp;
        case '/':
            tmp.token_type = DIV;
            return tmp;
        case '*':
            tmp.token_type = MULT;
            return tmp;
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '[':
            tmp.token_type = LBRAC;
            return tmp;
        case ']':
            tmp.token_type = RBRAC;
            return tmp;
        case '(':
            tmp.token_type = LPAREN;
            return tmp;
        case ')':
            tmp.token_type = RPAREN;
            return tmp;
        case '<':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = LTEQ;
            } else if (c == '>') {
                tmp.token_type = NOTEQUAL;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = LESS;
            }
            return tmp;
        case '>':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = GTEQ;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = GREATER;
            }
            return tmp;
        default:
            if (isdigit(c)) {
                input.UngetChar(c);
                return ScanNumber();
            } else if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

            return tmp;
    }
}

int main()
{
    LexicalAnalyzer lexer;
    Token token;

    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }
}
