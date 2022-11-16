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

string global_scope = "::";
int sPermission = 0;

vector< struct compare_lhs_rhs> memory;

Parser parser;
 LexicalAnalyzer lexer;
 Token token1;
 Token token2;
 Token token3;

 void Parser::printAssignment(Token tok1, Token tok2){

    struct symbol_table* temp = newNode;
    struct symbol_table* temp1 = newNode;
    string scope1 = "";
    string scope2 = "";

    while(temp != NULL){
        if(temp->entry->name == tok1.lexeme) {
        if(temp->entry->perms == 2 && global_scope != temp->entry->scope) {
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
        if(temp1->entry->perms == 2 && global_scope != temp1->entry->scope){
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

void Parser::printlist(){
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

void Parser::deleteList(string lexeme){
    if(symbolTable == NULL){
        return;
    }
   
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
   global_scope = newNode->entry->scope;
   
}

void Parser::addList(string lexeme){ 
    if(symbolTable == NULL){ 
        symbolTable = (symbol_table*) malloc (sizeof(symbol_table));
        struct symbol_table_entry* newItem = (symbol_table_entry*) malloc (sizeof(symbol_table_entry));
        symbolTable->entry = newItem;
        symbolTable->next = NULL;   
        symbolTable->previous = NULL;

        int len = lexeme.length(); 
        symbolTable->entry->name = new char[len+1];
        symbolTable->entry->name = lexeme;
        // pass by reference?
        //strcpy(symbolTable->entry->name,lexeme);
        symbolTable->entry->name[len] = '\0'; 
        symbolTable->entry->scope = global_scope;
        symbolTable->entry->perms = sPermission; 

        newNode = symbolTable;
        tempNode = symbolTable; 
 } else { 
        tempNode = symbolTable; // create another pointer to struct temp1 which will point to top of the symbolTable
    while (tempNode->next != NULL){ 
        tempNode = tempNode-> next;
    }
    //add newElement at top
    newNode = (symbol_table*) malloc (sizeof(symbol_table));
    struct symbol_table_entry* newItem = (symbol_table_entry*) malloc(sizeof(symbol_table_entry));
    newNode->entry = newItem;
    newNode->next = NULL;
    newNode->previous = tempNode;
    tempNode->next = newNode; // now newNode is at top of the stack; 
    int len = lexeme.length();
    
    newNode->entry->name = new char[len+1];
    //strcpy(newNode->entry->name, lexeme);
    newNode->entry->name = lexeme;
    newNode->entry->name[len] = '\0';
    newNode->entry->scope = global_scope;
    newNode->entry->perms = sPermission;
 }
 
}

void Parser::parse_program(){
    //cout<< "Inside parse_program " << endl;
    token1 = lexer.GetToken();
    if(token1.token_type == ID) {
        token2 = lexer.GetToken();
    if(token2.token_type == COMMA || token2.token_type == SEMICOLON){
        lexer.UngetToken(token2);
        lexer.UngetToken(token1);
        parser.parse_global_vars();
        parser.parse_scope();
        //cout << " program -> global_vars scope" << endl;
    } else if(token2.token_type == LBRACE) {
        lexer.UngetToken(token2);
        lexer.UngetToken(token1);
        parser.parse_scope();
        //cout << "global_vars -> epsilon" << endl;
    } else {
        cout << "Syntax Error"<<endl;
        exit(1);
    }
    } else {
        cout <<"Syntax Error"<<endl; 
        exit(1);
    }
}

void Parser::parse_global_vars(){
    //cout<< "Inside parse_global_vars " << endl;
    token1 = lexer.GetToken();
    if(token1.token_type == ID){
        lexer.UngetToken(token1);
        parser.parse_varlist();
        token1 = lexer.GetToken();
        if(token1.token_type == SEMICOLON) {
           //cout << " global_vars -> var_list SEMICOLON " << endl; 
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

void Parser::parse_scope(){
    //cout<< "Inside parse_scope" << endl;
    token1 = lexer.GetToken();
    if(token1.token_type == ID){
//        cout<<"lexeme value inside scope " << token1.lexeme << endl; 
        string slexeme = token1.lexeme; 
        global_scope = const_cast<char *>(slexeme.c_str());  
        token1= lexer.GetToken();
    if(token1.token_type == LBRACE){   
        parser.parse_public_vars();
        parser.parse_private_vars();
        parser.parse_stmt_list();
    token1 = lexer.GetToken();
    if(token1.token_type == RBRACE){
//        cout << "call deleteList"<<endl;
        deleteList(global_scope);
        token1=lexer.GetToken();
//        cout<< "return from deleteList: token1.lexeme = " << token1.lexeme << endl;
    if(token1.token_type ==END_OF_FILE){
        deleteList(global_scope);
    }
    else{
        lexer.UngetToken(token1);
    } 
    }
    else {
        cout<< "Syntax Error"<<endl;
        exit(1);
    }
    }
    else{
        cout<< "Syntax Error"<<endl;
        exit(1);
    }
    } else {
        cout << "Syntax Error"<<endl;
        exit(1);
    }
}

void Parser::parse_public_vars(){
    //cout<< "Inside parse_public_vars " << endl;
    token1 = lexer.GetToken();
//    cout << "token1.lexeme inside public_var " << token1.lexeme<< endl;
    if(token1.token_type == PUBLIC){
        sPermission = 1;
        token1 = lexer.GetToken();
        if(token1.token_type == COLON){
            token1 = lexer.GetToken(); 
            //Check FIRSTSET of varlist;
        if(token1.token_type == ID){
            lexer.UngetToken(token1);
            parser.parse_varlist();
            token1 = lexer.GetToken();
        if(token1.token_type == SEMICOLON){
//            cout<< "public_vars -> PUBLIC COLON var_list SEMICOLON" << endl;
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
//   cout << "public_vars -> epsilon"<< endl;
   }
  else{
  cout << "Syntax Error"<< endl;
  exit(1);
  }
}

void Parser::parse_private_vars(){
    //cout<< "Inside parse_private_vars" << endl;
    token1=lexer.GetToken();
    if(token1.token_type == PRIVATE){
        sPermission = 2;
        token1 = lexer.GetToken();
    if(token1.token_type == COLON){
        token1 =lexer.GetToken();
        if(token1.token_type == ID){
            lexer.UngetToken(token1);
            parser.parse_varlist();
            token1 = lexer.GetToken();
            if(token1.token_type == SEMICOLON){
//                cout<< "private_vars -> PRIVATE COLON var_list SEMICOLON" <<endl;
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
            //    cout<< "private_vars -> epsilon" << endl;
        } else {
            return;
        }
}

void Parser::parse_varlist(){
    //cout<< "Inside parse_varlist" << endl;
    token1 = lexer.GetToken();

    char* lexeme = (char*) malloc (sizeof (token1.lexeme)); 
    memcpy(lexeme,(token1.lexeme).c_str(),sizeof(token1)); 
    addList(lexeme); 
  
    symbol_table* temp1 = symbolTable; 
  
//    cout << "\n" << endl;
    while(temp1 != NULL){
//        cout << " symbolTable -> name: " << temp1->entry->name <<" "<< "scope: " << temp1->entry->scope << " "<< "permission: " << temp1->entry->perms << endl;
        temp1 = temp1 ->next; 
  }

  if(token1.token_type == ID){
    token1 = lexer.GetToken();
     if(token1.token_type == COMMA){    
       parser.parse_varlist();
//       cout << "var_list -> ID COMMA var_list" << endl;
       return;
       }
     //FOLLOWSET of varlist
        else if(token1.token_type == SEMICOLON){
            lexer.UngetToken(token1);
//            cout << " var_list -> ID" << endl;
            return;
       }
     else{
      cout << "Syntax Error" << endl;   // How to handle the case: var_list -> ID
      exit(1);
     }
  }
  else {
   cout << "Syntax Error" << endl;
   exit(1);
  }
}

void Parser::parse_stmt(){
//cout<< "Inside parse_stmt" << endl; 
  token1 = lexer.GetToken();
  if(token1.token_type == ID){
    token2 = lexer.GetToken();
    if(token2.token_type == EQUAL){     
      token3 = lexer.GetToken();
       if(token3.token_type == ID) {

        printAssignment(token1, token3);
        
        token1 = lexer.GetToken();
          if(token1.token_type == SEMICOLON){
          //cout << "stmt -> ID EQUAL ID SEMICOLON "<< endl;
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

        global_scope = const_cast<char*>((token1.lexeme).c_str());  
        //cout << "scope in parse_stmt() "<< global_scope << endl;
        lexer.UngetToken(token2);
        lexer.UngetToken(token1);
        parser.parse_scope();     
     } else {
        cout << "Syntax Error" << endl;
        exit(1);
     }
  }
  
  else{
    cout << "Syntax Error" << endl;
    exit(1);
  }
}

void Parser::parse_stmt_list(){
    //cout<< "Inside parse_stmt_list" << endl; 
 //check FIRST SET of scope()
    token1 = lexer.GetToken();
   if(token1.token_type == ID){
     lexer.UngetToken(token1);
      parser.parse_stmt();
       token2 = lexer.GetToken();
       if(token2.token_type == ID){
            lexer.UngetToken(token2);
            parser.parse_stmt_list();
       //cout<< " stmt_list -> stmt stmt_list" << endl;
        return;
       } else if(token2.token_type == RBRACE){       
      //cout<< "stmt_list -> stmt"<< endl;
            lexer.UngetToken(token2);       
            return;
       }  else {
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
    parser.parse_program(); 
    parser.printlist();
//    LexicalAnalyzer lexer;
//    Token token;
//
//    token = lexer.GetToken();
//    token.Print();
//    while (token.token_type != END_OF_FILE)
//    {
//        token = lexer.GetToken();
//        token.Print();x
//    }
}



