#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <cstring>

#include "lexer.h"
#include "parser.h"

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

vector< struct compare_lhs_rhs> memory;

Parser parser;
LexicalAnalyzer lexer;
Token token1;
Token token2;
Token token3;

 // refactor assign, add, delete, print

// no good ideas for assign
 void Parser::s_assign(Token tok1, Token tok2){

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

void Parser::s_print(){
    // rewrite for loop, google search vector loops c++
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

void Parser::s_delete(string lexeme){
    if(symbolTable == NULL){
        return;
    }
   // can we deallocate memory w/o covenrting to NULL ?
   while(newNode->entry->scope == lexeme){   
        tempNode->next = NULL;
        newNode->previous = NULL;
        if(tempNode->previous != NULL){
            tempNode=tempNode->previous;
            newNode=tempNode->next;
            newNode->next=NULL;
            return;
        } else if (tempNode == newNode) {
           tempNode = NULL;
            newNode = NULL;
            return;
        } else {
            tempNode->next = NULL;
            newNode->previous = NULL;
            newNode=tempNode;
            tempNode->next =NULL;
   }

   }
   current_scope = newNode->entry->scope;
   
}

// try deleting NULL assignments
void Parser::s_add(string lexeme){ 
    if(symbolTable == NULL){ 
        symbolTable = (symbol_table*) malloc (sizeof(symbol_table));
        struct symbol_table_entry* newItem = (symbol_table_entry*) malloc (sizeof(symbol_table_entry));
        
        symbolTable->entry = newItem;
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


void Parser::s_program(){
    token1 = lexer.GetToken();
    
    if(token1.token_type == ID) {
        token2 = lexer.GetToken();
        
        if(token2.token_type == COMMA || token2.token_type == SEMICOLON){
            lexer.UngetToken(token2);
            lexer.UngetToken(token1);
            parser.s_global();
            parser.s_scope();
        } else if(token2.token_type == LBRACE) {
            lexer.UngetToken(token2);
            lexer.UngetToken(token1);
            parser.s_scope();
        } else {
            cout << "Syntax Error"<<endl;
            exit(1);
        }
    } else {
        cout <<"Syntax Error"<<endl; 
        exit(1);
    }
}

void Parser::s_global(){
    token1 = lexer.GetToken();
    
    if(token1.token_type == ID){
        lexer.UngetToken(token1);
        parser.s_varlist();
        token1 = lexer.GetToken();
        
        if(token1.token_type == SEMICOLON) {
    
        } else {
            cout << "Syntax Error" <<endl;
            exit(1);
        } 
    } else {
        cout << "Syntax Error"<< endl;
        exit(1);
    }
    return;
}

void Parser::s_scope(){
    token1 = lexer.GetToken();
        
        if(token1.token_type == ID){
            current_scope = token1.lexeme;
            token1 = lexer.GetToken();
            
            if(token1.token_type == LBRACE){   
                parser.s_public();
                parser.s_private();
                parser.s_stmt_list();
                token1 = lexer.GetToken();
                
                if(token1.token_type == RBRACE){
                    s_delete(current_scope);
                    token1=lexer.GetToken();
                    
                    if(token1.token_type == END_OF_FILE){
                        s_delete(current_scope);
                    } else {
                        lexer.UngetToken(token1);
                    } 
                } else {
                    cout<< "Syntax Error"<<endl;
                    exit(1);
                }
            } else {
                cout<< "Syntax Error"<<endl;
                exit(1);
            }
        } else {
            cout << "Syntax Error"<<endl;
            exit(1);
        }
}

void Parser::s_public(){
    token1 = lexer.GetToken();
    
    if(token1.token_type == PUBLIC){
        public_private = 1;
        token1 = lexer.GetToken();
        
        if(token1.token_type == COLON){
            token1 = lexer.GetToken(); 
            
            if(token1.token_type == ID){
                lexer.UngetToken(token1);
                parser.s_varlist();
                token1 = lexer.GetToken();
                
                if(token1.token_type == SEMICOLON){
                
                } else {
                    cout<< "Syntax Error" << endl;
                    exit(1);
                }
            } else {
                cout<< "Syntax Error" << endl;
                exit(1);
            }
        } else {
            cout<< "Syntax Error" << endl;
            exit(1);
        }
    } else if (token1.token_type == PRIVATE || token1.token_type == ID){
        lexer.UngetToken(token1);
    } else {
        cout << "Syntax Error"<< endl;
        exit(1);
    }
}

void Parser::s_private(){
    token1=lexer.GetToken();
    
    if(token1.token_type == PRIVATE){
        public_private = 2;
        token1 = lexer.GetToken();
        
        if(token1.token_type == COLON){
            token1 =lexer.GetToken();
            
            if(token1.token_type == ID){
                lexer.UngetToken(token1);
                parser.s_varlist();
                token1 = lexer.GetToken();
                
                if(token1.token_type == SEMICOLON){
        
                } else {
                    cout<<"Syntax Error"<<endl;
                    exit(1);
                }
            } else {
                cout<<"Syntax Error"<<endl;
                exit(1);
            }
        } else {
            cout<<"Syntax Error"<<endl;
            exit(1);
        }
    } else if(token1.token_type == ID) {
        lexer.UngetToken(token1);
        } else {
            return;
        }
}

void Parser::s_varlist(){
    token1 = lexer.GetToken();
    string lexeme = token1.lexeme; 
    
    s_add(lexeme); 
    
    symbol_table* temp1 = symbolTable; 
  
    while(temp1 != NULL){
        temp1 = temp1 ->next; 
    }

    if(token1.token_type == ID){
        token1 = lexer.GetToken();
        
        if(token1.token_type == COMMA){    
            parser.s_varlist();
            return;
        } else if(token1.token_type == SEMICOLON){
            lexer.UngetToken(token1);
            return;
        } else {
            cout << "Syntax Error" << endl;
            exit(1);
        }
        } else {
            cout << "Syntax Error" << endl;
            exit(1);  
        }
}

void Parser::s_stmt(){
    token1 = lexer.GetToken();
    
    if(token1.token_type == ID){
        token2 = lexer.GetToken();
        
        if(token2.token_type == EQUAL){     
            token3 = lexer.GetToken();
            
            if(token3.token_type == ID) {
                s_assign(token1, token3);
                token1 = lexer.GetToken();
                if(token1.token_type == SEMICOLON){
                    return;
                } else {
                    cout << "Syntax Error" << endl;
                    exit(1);
                }
            } else {
                cout << "Syntax Error" << endl;
                exit(1);
            }
        }
    else if(token2.token_type == LBRACE){
        current_scope = token1.lexeme;
        lexer.UngetToken(token2);
        lexer.UngetToken(token1);
        parser.s_scope();     
    } else {
        cout << "Syntax Error" << endl;
        exit(1);
    }
    } else {
        cout << "Syntax Error" << endl;
        exit(1);
    }
}

void Parser::s_stmt_list(){
    token1 = lexer.GetToken();
    
    if(token1.token_type == ID){
        lexer.UngetToken(token1);
        parser.s_stmt();
        token2 = lexer.GetToken();
        
        if(token2.token_type == ID){
            lexer.UngetToken(token2);
            parser.s_stmt_list();
            return;
        } else if(token2.token_type == RBRACE){       
            lexer.UngetToken(token2);       
            return;
        } else {
            cout<< "Syntax Error"<<endl;
            exit(1);
        }
    } else {
        cout<< "Syntax Error"<<endl;
        exit(1);
    }
}


int main()
{   
    parser.s_program(); 
    parser.s_print();
}


