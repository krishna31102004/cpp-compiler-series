
// Name: Krishna Balaji
// ASU ID: 1226254343

#include <iostream>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include <cstring>

#include "execute.h"
#include "lexer.h"

using namespace std;

LexicalAnalyzer lexer;
Token currToken;
map<string,int> locationTable;

void syntaxError(const char *msg) {
    cerr << "Syntax error: " << msg
         << " at line " << currToken.line_no << "\n";
    exit(1);
}

void advance() {
    currToken = lexer.GetToken();
}
void match(TokenType expected) {
    if(currToken.token_type==expected){
        advance();
    } else {
        syntaxError("unexpected token");
    }
}

int getLocation(const string &lexeme,bool isNumber) {
    map<string,int>::iterator it=locationTable.find(lexeme);
    if(it!=locationTable.end()){
        return it->second;
    }
    int loc=next_available;
    if(isNumber){
        mem[loc]=atoi(lexeme.c_str());
    } else {
        mem[loc]=0;
    }
    next_available++;
    locationTable[lexeme]=loc;
    return loc;
}

InstructionNode* parseStmtList();
InstructionNode* parseStmt();
InstructionNode* parseBody();
void parseVarSection();
void parseInputs();
InstructionNode* parseSwitchStmt();
InstructionNode* parseForStmt();

void parseVarSection() {
    if(currToken.token_type!=ID)
        syntaxError("expected identifier in var section");
    while(true){
        string name=currToken.lexeme;
        getLocation(name,false);
        match(ID);
        if(currToken.token_type==COMMA){
            match(COMMA);
            if(currToken.token_type!=ID)
                syntaxError("expected identifier after comma");
        } else {
            break;
        }
    }
    match(SEMICOLON);
}

int parsePrimary() {
    if(currToken.token_type==ID){
        string name=currToken.lexeme;
        int loc=getLocation(name,false);
        match(ID);
        return loc;
    }
    if(currToken.token_type==NUM){
        string lit=currToken.lexeme;
        int loc=getLocation(lit,true);
        match(NUM);
        return loc;
    }
    syntaxError("expected identifier or number");
    return -1;
}

void parseCondition(ConditionalOperatorType &condOp,
                    int &lhs,int &rhs) {
    lhs=parsePrimary();
    TokenType rel=currToken.token_type;
    if(rel==GREATER)      condOp=CONDITION_GREATER;
    else if(rel==LESS)    condOp=CONDITION_LESS;
    else if(rel==NOTEQUAL)condOp=CONDITION_NOTEQUAL;
    else syntaxError("expected relational operator");
    match(rel);
    rhs=parsePrimary();
}

InstructionNode* parseAssignStmt() {
    string var=currToken.lexeme;
    int lhs_loc=getLocation(var,false);
    match(ID);match(EQUAL);
    int op1=parsePrimary();
    ArithmeticOperatorType opType=OPERATOR_NONE;
    int op2=0;
    if(currToken.token_type==PLUS||
       currToken.token_type==MINUS||
       currToken.token_type==MULT||
       currToken.token_type==DIV) {
        switch(currToken.token_type){
            case PLUS:  opType=OPERATOR_PLUS;  break;
            case MINUS: opType=OPERATOR_MINUS; break;
            case MULT:  opType=OPERATOR_MULT;  break;
            case DIV:   opType=OPERATOR_DIV;   break;
            default:    break;
        }
        TokenType opTok=currToken.token_type;
        match(opTok);
        op2=parsePrimary();
    }
    match(SEMICOLON);

    InstructionNode *n=new InstructionNode;
    n->type=ASSIGN;
    n->assign_inst.lhs_loc=lhs_loc;
    n->assign_inst.op1_loc=op1;
    n->assign_inst.op2_loc=op2;
    n->assign_inst.op=opType;
    n->next=NULL;
    return n;
}

InstructionNode* parseInputStmt() {
    match(INPUT);
    if(currToken.token_type!=ID)
        syntaxError("expected identifier after input");
    int loc=getLocation(currToken.lexeme,false);
    match(ID);match(SEMICOLON);

    InstructionNode *n=new InstructionNode;
    n->type=IN;
    n->input_inst.var_loc=loc;
    n->next=NULL;
    return n;
}

InstructionNode* parseOutputStmt() {
    match(OUTPUT);
    if(currToken.token_type!=ID)
        syntaxError("expected identifier after output");
    int loc=getLocation(currToken.lexeme,false);
    match(ID);match(SEMICOLON);

    InstructionNode *n=new InstructionNode;
    n->type=OUT;
    n->output_inst.var_loc=loc;
    n->next=NULL;
    return n;
}

InstructionNode* parseIfStmt() {
    match(IF);
    ConditionalOperatorType cond;int a,b;
    parseCondition(cond,a,b);

    InstructionNode *cj=new InstructionNode;
    cj->type=CJMP;
    cj->cjmp_inst.condition_op=cond;
    cj->cjmp_inst.op1_loc=a;
    cj->cjmp_inst.op2_loc=b;

    InstructionNode *body=parseBody();
    cj->next=body;

    InstructionNode *noop=new InstructionNode;
    noop->type=NOOP;noop->next=NULL;
    InstructionNode *t=body;
    while(t->next)t=t->next;
    t->next=noop;
    cj->cjmp_inst.target=noop;
    return cj;
}

InstructionNode* parseWhileStmt() {
    match(WHILE);
    ConditionalOperatorType cond;int a,b;
    parseCondition(cond,a,b);

    InstructionNode *cj=new InstructionNode;
    cj->type=CJMP;
    cj->cjmp_inst.condition_op=cond;
    cj->cjmp_inst.op1_loc=a;
    cj->cjmp_inst.op2_loc=b;

    InstructionNode *body=parseBody();
    cj->next=body;

    InstructionNode *jmp=new InstructionNode;
    jmp->type=JMP;
    jmp->jmp_inst.target=cj;
    jmp->next=NULL;

    InstructionNode *t=body;
    while(t->next)t=t->next;
    t->next=jmp;

    InstructionNode *noop=new InstructionNode;
    noop->type=NOOP;noop->next=NULL;
    jmp->next=noop;
    cj->cjmp_inst.target=noop;
    return cj;
}

InstructionNode* parseSwitchStmt() {
    match(SWITCH);

    int varLoc=getLocation(currToken.lexeme,false);
    match(ID);
    match(LBRACE);

    InstructionNode* exitNoop=new InstructionNode;
    exitNoop->type=NOOP;
    exitNoop->next=NULL;

    InstructionNode *head=NULL;
    InstructionNode *tail=NULL;

    while(currToken.token_type==CASE) {
        match(CASE);

        if(currToken.token_type!=NUM)
            syntaxError("number expected after CASE");
        int litLoc=next_available;
        mem[next_available++]=atoi(currToken.lexeme.c_str());
        match(NUM);
        match(COLON);

        InstructionNode* test=new InstructionNode;
        test->type=CJMP;
        test->cjmp_inst.condition_op=CONDITION_NOTEQUAL;
        test->cjmp_inst.op1_loc=varLoc;
        test->cjmp_inst.op2_loc=litLoc;

        InstructionNode* skip=new InstructionNode;
        skip->type=NOOP;
        skip->next=NULL;

        InstructionNode* body=parseBody();

        InstructionNode* leave=new InstructionNode;
        leave->type=JMP;
        leave->jmp_inst.target=exitNoop;
        leave->next=NULL;

        if(body){
            InstructionNode* t=body;
            while(t->next)t=t->next;
            t->next=leave;
        }else{
            body=leave;
        }

        test->next=skip;
        test->cjmp_inst.target=body;

        if(head==NULL)head=test;
        if(tail)tail->next=test;
        tail=skip;
    }

    if(currToken.token_type==DEFAULT){
        match(DEFAULT);
        match(COLON);
        InstructionNode* defBody=parseBody();

        InstructionNode* leave=new InstructionNode;
        leave->type=JMP;
        leave->jmp_inst.target=exitNoop;
        leave->next=NULL;

        if(defBody){
            InstructionNode* t=defBody;
            while(t->next)t=t->next;
            t->next=leave;
        }else{
            defBody=leave;
        }

        if(tail)tail->next=defBody;
        else head=defBody;
        tail=leave;
    }else{
        if(tail)tail->next=exitNoop;
        else head=exitNoop;
    }

    if(tail&&tail!=exitNoop)
        tail->next=exitNoop;

    match(RBRACE);
    return head;
}

InstructionNode* parseForStmt() {
    match(FOR);match(LPAREN);

    InstructionNode* init=parseAssignStmt();

    ConditionalOperatorType condOp;int a,b;
    parseCondition(condOp,a,b);
    match(SEMICOLON);

    InstructionNode* incr=parseAssignStmt();
    match(RPAREN);

    InstructionNode* body=parseBody();

    InstructionNode* cj=new InstructionNode;
    cj->type=CJMP;
    cj->cjmp_inst.condition_op=condOp;
    cj->cjmp_inst.op1_loc=a;
    cj->cjmp_inst.op2_loc=b;

    init->next=cj;
    cj->next=body;

    InstructionNode* t=body;
    while(t->next)t=t->next;
    t->next=incr;

    InstructionNode* jm=new InstructionNode;
    jm->type=JMP;
    jm->jmp_inst.target=cj;
    jm->next=NULL;
    incr->next=jm;

    InstructionNode* noop=new InstructionNode;
    noop->type=NOOP;noop->next=NULL;
    jm->next=noop;

    cj->cjmp_inst.target=noop;
    return init;
}

InstructionNode* parseStmt() {
    switch(currToken.token_type) {
      case ID:     return parseAssignStmt();
      case INPUT:  return parseInputStmt();
      case OUTPUT: return parseOutputStmt();
      case IF:     return parseIfStmt();
      case WHILE:  return parseWhileStmt();
      case SWITCH: return parseSwitchStmt();
      case FOR:    return parseForStmt();
      default:     syntaxError("invalid start of statement");
    }
    return NULL;
}

InstructionNode* parseStmtList() {
    InstructionNode *head=NULL,*tail=NULL;
    while(currToken.token_type==ID||
          currToken.token_type==INPUT||
          currToken.token_type==OUTPUT||
          currToken.token_type==IF||
          currToken.token_type==WHILE||
          currToken.token_type==SWITCH||
          currToken.token_type==FOR){
        InstructionNode* st=parseStmt();
        InstructionNode* last=st;
        while(last->next)last=last->next;
        if(!head)head=st;
        else tail->next=st;
        tail=last;
    }
    return head;
}

InstructionNode* parseBody() {
    match(LBRACE);
    InstructionNode* blk=parseStmtList();
    match(RBRACE);
    return blk;
}

void parseInputs() {
    while(currToken.token_type==NUM) {
        inputs.push_back(atoi(currToken.lexeme.c_str()));
        match(NUM);
    }
}

InstructionNode* parse_Generate_Intermediate_Representation() {
    advance();
    parseVarSection();
    InstructionNode* program=parseBody();
    parseInputs();
    return program;
}