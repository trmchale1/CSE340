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
    "PUBLIC", "PRIVATE","EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRACE", "RBRACE",
    "ID", "ERROR"
};

#define KEYWORDS_COUNT 2
string keyword[] = {"public","private"};


/////////////////////////////////////////

struct symbol_table_entry {
    char* name;
    char* scope;
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

char* global_scope = "::";
int sPermission = 0;

vector< struct compare_lhs_rhs> memory;

 LexicalAnalyzer lexer;
 Token token1;
 Token token2;
 Token token3;

void printAssignment(Token tok1, Token tok2){

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

void printlist(){
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

void deleteList(char* lexeme){
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


 void addList(char* lexeme){ 
    if(symbolTable == NULL){ 
        symbolTable = (symbol_table*) malloc (sizeof(symbol_table));
        struct symbol_table_entry* newItem = (symbol_table_entry*) malloc (sizeof(symbol_table_entry));
        symbolTable->entry = newItem;
        symbolTable->next = NULL;   
        symbolTable->previous = NULL;

        int len = strlen(lexeme) ; 
        symbolTable->entry->name = new char[len+1];
        strcpy(symbolTable->entry->name,lexeme);
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
    int len = strlen(lexeme) ;
    
    newNode->entry->name = new char[len+1];
    strcpy(newNode->entry->name, lexeme);
    newNode->entry->name[len] = '\0';
    newNode->entry->scope = global_scope;
    newNode->entry->perms = sPermission;
 }
 
}


void LexicalAnalyzer::parse_program(){
//    cout<< "Inside parse_program " << endl;
    token1 = lexer.GetToken();
    if(token1.token_type == ID) {
        token2 = lexer.GetToken();
    if(token2.token_type == COMMA || token2.token_type == SEMICOLON){
        lexer.UngetToken(token2);
        lexer.UngetToken(token1);
        lexer.parse_global_vars();
        lexer.parse_scope();
//        cout << " program -> global_vars scope" << endl;
    } else if(token2.token_type == LBRACE) {
        lexer.UngetToken(token2);
        lexer.UngetToken(token1);
        lexer.parse_scope();
//        cout << "global_vars -> epsilon" << endl;
    } else {
        cout << "Syntax Error"<<endl;
        return;
    }
    } else {
        cout <<"Syntax Error"<<endl; 
        return;
    }
}

void LexicalAnalyzer::parse_global_vars(){
//    cout<< "Inside parse_global_vars " << endl;
    token1 = lexer.GetToken();
    if(token1.token_type == ID){
        lexer.UngetToken(token1);
        lexer.parse_varlist();
        token1 = lexer.GetToken();
        if(token1.token_type == SEMICOLON) {
//            cout << " global_vars -> var_list SEMICOLON " << endl; 
        } else {
            cout << "Syntax Error" <<endl;
            return;
        } 
        } else {
            cout << "Syntax Error"<< endl;
            return;
        }
    return;
}


void LexicalAnalyzer::parse_scope(){
//    cout<< "Inside parse_scope" << endl;
    token1 = lexer.GetToken();
    if(token1.token_type == ID){
//        cout<<"lexeme value inside scope " << token1.lexeme << endl; 
        string slexeme = token1.lexeme; 
        global_scope = const_cast<char *>(slexeme.c_str());  
        token1= lexer.GetToken();
    if(token1.token_type == LBRACE){   
    lexer.parse_public_vars();
    lexer.parse_private_vars();
    lexer.parse_stmt_list();
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
        UngetToken(token1);
    } 
    }
    else {
        cout << "Syntax Error"<< endl;
    }
    }
    else{
        cout<< "Syntax Error"<<endl;
    }
    } else {
        cout << "Syntax Error"<<endl;
    }
}

void LexicalAnalyzer::parse_public_vars(){
//    cout<< "Inside parse_public_vars " << endl;
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
            lexer.parse_varlist();
            token1 = lexer.GetToken();
        if(token1.token_type == SEMICOLON){
//            cout<< "public_vars -> PUBLIC COLON var_list SEMICOLON" << endl;
        } else {
            cout<< "Syntax Error" << endl;
        }
    } else {
        cout<< "Syntax Error" << endl;
    }
    } else {
    cout<< "Syntax Error" << endl;
    }
    } else if (token1.token_type == PRIVATE || token1.token_type == ID){
  lexer.UngetToken(token1);
//   cout << "public_vars -> epsilon"<< endl;
   }
  else{
  cout << "Syntax Error"<< endl;
  }
}

void LexicalAnalyzer::parse_private_vars(){
//    cout<< "Inside parse_private_vars" << endl;
    token1=lexer.GetToken();
    if(token1.token_type == PRIVATE){
        sPermission = 2;
        token1 = lexer.GetToken();
    if(token1.token_type == COLON){
        token1 =lexer.GetToken();
        if(token1.token_type == ID){
            lexer.UngetToken(token1);
            lexer.parse_varlist();
            token1 = lexer.GetToken();
            if(token1.token_type == SEMICOLON){
//                cout<< "private_vars -> PRIVATE COLON var_list SEMICOLON" <<endl;
            } else {
                cout<<"Syntax Error"<<endl;
                return;
            }
            } else {
                cout<<"Syntax Error"<<endl;
                return;
                }
            } else {
                cout<<"Syntax Error"<<endl;
                return;
            }
 } else if(token1.token_type == ID) {
    lexer.UngetToken(token1);
//    cout<< "private_vars -> epsilon" << endl;
 } else {
    cout<< "Syntax Error"<< endl;
    return;
    }
}

void LexicalAnalyzer::parse_varlist(){
//    cout<< "Inside parse_varlist" << endl;
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
       lexer.parse_varlist();
//       cout << "var_list -> ID COMMA var_list" << endl;
       return;
       }
     //FOLLOWSET of varlist
        else if(token1.token_type == SEMICOLON){
            UngetToken(token1);
//            cout << " var_list -> ID" << endl;
            return;
       }
     else{
      cout << "Syntax Error" << endl;   // How to handle the case: var_list -> ID
      return;
     }
  }
  else {
   cout << "Syntax Error" << endl;
   return;
  }
}


void LexicalAnalyzer::parse_stmt(){
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
          }
          else{
          cout << "Syntax Error" << endl;
            return;
          }
       }
       else {
       cout << "Syntax Error" << endl;
       return;
       }
      }
     else if(token2.token_type == LBRACE){

     global_scope = const_cast<char*>((token1.lexeme).c_str());  
    //cout << "scope in parse_stmt() "<< global_scope << endl;
     lexer.UngetToken(token2);
     lexer.UngetToken(token1);
     lexer.parse_scope();     
     }
     
     else {
     cout << "Syntax Error" << endl;
     return;
     }
  }
  
  else{
  cout << "Syntax Error" << endl;
  return;
  }
  return;
}



void LexicalAnalyzer::parse_stmt_list(){
//    cout<< "Inside parse_stmt_list" << endl; 
 //check FIRST SET of scope()
    token1 = lexer.GetToken();

   if(token1.token_type == ID){
     lexer.UngetToken(token1);
      lexer.parse_stmt();
       token2 = lexer.GetToken();
       if(token2.token_type == ID){
       lexer.UngetToken(token2);
       lexer.parse_stmt_list();
//       cout<< " stmt_list -> stmt stmt_list" << endl;
       return;
       }

    else if(token2.token_type == RBRACE){       
//      cout<< "stmt_list -> stmt"<< endl;
       lexer.UngetToken(token2);       
       return;
       }  
       
       else {
       cout<< "Syntax Error"<<endl;
       return;
       }
    }
    
    else {
    cout<< "Syntax Error"<<endl;
    return;
    }
}



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


bool LexicalAnalyzer::SkipComment(){
 
    char c;
    bool comment = false;
  
    input.GetChar(c);
  
    if(input.EndOfInput()){
        input.UngetChar(c);
        return comment;
    }
  
     if(c=='/'){
        input.GetChar(c);
            if(c=='/'){
                comment = true;
                while (c != '\n'){
                    comment = true;
                    input.GetChar(c);
                }
                line_no +=1;
                SkipComment();      
         } else {
            comment = false;
            cout << "Comment not found" << endl;
            return comment;
         }
      } 

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    } 
    
    return comment;

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
        if (IsKeyword(tmp.lexeme)){
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        }else
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
    SkipComment();
    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
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
        case '{':
            tmp.token_type = LBRACE;
            return tmp;
        case '}':
            tmp.token_type = RBRACE;
            return tmp;
        default:
            if (isdigit(c)) {
                input.UngetChar(c);
                cout << "This is a number, not calling ScanNumber()" << endl;
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
    lexer.parse_program(); 
    printlist();
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
