#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"
#include <vector>
#include <string>

using namespace std;


class Parser {
    public:
        void s_program();
        void s_global();
        void s_scope();
        void s_varlist();
        void s_public();
        void s_private();
        void s_stmt_list();
        void s_stmt();
        void s_assign(Token,Token);
        void s_add(string);
        void s_delete(string);
        void s_print();

};

#endif  //__PARSER__H__
