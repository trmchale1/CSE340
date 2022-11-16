#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include <string>
#include "lexer.h"
#include <vector>
#include <string>

using namespace std;

class Symbol_Table {
    public:
        void s_assign(Token,Token);
        void s_add(string);
        void s_delete(string);
        void s_print();
};


#endif  //__PARSER__H__