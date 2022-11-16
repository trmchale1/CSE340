#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"
#include <vector>
#include <string>

using namespace std;


class Parser {
    public:
        void parse_program();
        void parse_global_vars();
        void parse_scope();
        void parse_varlist();
        void parse_public_vars();
        void parse_private_vars();
        void parse_stmt_list();
        void parse_stmt();
        void printAssignment(Token,Token);
        void addList(string);
        void deleteList(string);
        void printlist();

};

#endif  //__PARSER__H__
