#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"
#include <vector>


struct symbol_table {
    symbol_table_entry* entry;
    symbol_table* previous;
    symbol_table* next;
};


struct symbol_table_entry {
    std::string name;
    std::string scope;
    int perms;
};


