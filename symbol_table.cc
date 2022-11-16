#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <cstring>

#include "lexer.h"
#include "parser.h"
#include "symbol_table.h"

using namespace std;


struct symbol_table_entry {
            std::string name;
            std::string scope;
            int perms;
        };

        struct symbol_table {
            symbol_table_entry* entry;
            symbol_table* previous;
            symbol_table* next;
        };

        struct compare_lhs_rhs {
            string lhs;
            string rhs;
            string scope1;
            string scope2;
        };



struct symbol_table* symbolTable; 
struct symbol_table* tempNode;
struct symbol_table* newNode;

string current_scope = "::";
int public_private = 0;

Parser parser;
LexicalAnalyzer lexer;
Symbol_Table table;
Token token1;
Token token2;
Token token3;

vector< struct compare_lhs_rhs> memory;

 void Symbol_Table::s_assign(Token tok1, Token tok2){

    struct symbol_table* temp = newNode;
    struct symbol_table* temp1 = newNode;
    string scope1 = "";
    string scope2 = "";

    while(temp != NULL){
        if(temp->entry->name == tok1.lexeme) {
        if(temp->entry->perms == 2 && current_scope != temp->entry->scope) {
             temp = temp->previous;
             continue;
         } else {
             scope1 = temp->entry->scope;
             break;
         }
   }
    temp = temp->previous;
   
    }

        while(temp1 != NULL){
        if(temp1->entry->name == tok2.lexeme){
        if(temp1->entry->perms == 2 && current_scope != temp1->entry->scope){
            temp1 = temp1->previous;
            continue;

        } else {
            scope2 = temp1->entry->scope;
            break;
        }
       }
    
       temp1 = temp1->previous;
    
    }
    if(scope1.length() == 0) {
        scope1 = "?";
    }
    
    if(scope2.length() == 0){
    scope2 = '?';
    } 

    struct compare_lhs_rhs temp_node;
    temp_node.lhs = tok1.lexeme;
    temp_node.rhs = tok2.lexeme;
    temp_node.scope1 = scope1;
    temp_node.scope2 = scope2;
    memory.push_back(temp_node);
}

void Symbol_Table::s_print(){
    for(int i=0; i < memory.size(); i++){
        cout << memory[i].scope1;
    
        if(memory[i].scope1 != "::") 
            cout << "."; 
    
        cout<< memory[i].lhs << " = " << memory[i].scope2;
    
        if(memory[i].scope2 != "::") 
            cout<<".";
    
        cout<< memory[i].rhs << endl;
    
        }
}


void Symbol_Table::s_delete(string lexeme){ 
    if(symbolTable == NULL){ 
        symbolTable = (symbol_table*) malloc (sizeof(symbol_table));
        struct symbol_table_entry* newItem = (symbol_table_entry*) malloc (sizeof(symbol_table_entry));
        symbolTable->entry = newItem;
        symbolTable->next = NULL;   
        symbolTable->previous = NULL;



        symbolTable->entry->name = lexeme;
        symbolTable->entry->scope = current_scope;
        symbolTable->entry->perms = public_private; 

        newNode = symbolTable;
        tempNode = symbolTable; 
 } else { 
        tempNode = symbolTable; 
        while (tempNode->next != NULL){ 
            tempNode = tempNode-> next;
        }
        newNode = (symbol_table*) malloc (sizeof(symbol_table));
        struct symbol_table_entry* newEntry = (symbol_table_entry*) malloc(sizeof(symbol_table_entry));
        newNode->entry = newEntry;
        newNode->next = NULL;
        newNode->previous = tempNode;
        tempNode->next = newNode;

        newNode->entry->name = lexeme;
        newNode->entry->scope = current_scope;
        newNode->entry->perms = public_private;
 }
 
}
