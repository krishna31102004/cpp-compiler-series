//Name:Krishna Balaji
//ASU ID:1226254343
//Class:CSE 340
//Section:20660



#include "parser.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <set>
#include <vector>
#include <map>
using namespace std;

Parser::Parser(){
    currentPolyIndex=-1;
    currentPolyHasParams=false;
    inputIndex=0;
}

int Parser::run(){
    parseInput();
    reportSemanticErrorsAndExitIfAny();
    bool doTask2=(std::find(tasks.begin(),tasks.end(),2)!=tasks.end());
    if(doTask2){
        executeProgram();
    }
    bool doWarn3=(std::find(tasks.begin(),tasks.end(),3)!=tasks.end());
    if(doWarn3){
        checkUninitializedVariables();
        if(!uninitializedWarnings.empty()){
            sort(uninitializedWarnings.begin(),uninitializedWarnings.end());
            cout<<"Warning Code 1: ";
            for(size_t i=0;i<uninitializedWarnings.size();i++){
                cout<<uninitializedWarnings[i];
                if(i+1<uninitializedWarnings.size())
                    cout<<" ";
            }
            cout<<endl;
        }
    }
    bool doWarn4=(std::find(tasks.begin(),tasks.end(),4)!=tasks.end());
    if(doWarn4){
        checkUselessAssignmentsBackward();
        if(!uselessWarnings.empty()){
            sort(uselessWarnings.begin(),uselessWarnings.end());
            cout<<"Warning Code 2: ";
            for(size_t i=0;i<uselessWarnings.size();i++){
                cout<<uselessWarnings[i];
                if(i+1<uselessWarnings.size())
                    cout<<" ";
            }
            cout<<endl;
        }
    }
    bool doDegree=(std::find(tasks.begin(),tasks.end(),5)!=tasks.end());
    if(doDegree){
        printDegrees();
    }
    
    freeStatements();
    return 0;
}

int main(){
    Parser parser;
    return parser.run();
}

void Parser::parseInput(){
    parse_program();
    Token t=lexer.GetToken();
    if(t.token_type!=END_OF_FILE){
        syntax_error();
    }
}

void Parser::parse_program(){
    parse_tasks_section();
    parse_poly_section();
    parse_execute_section();
    parse_inputs_section();
}

void Parser::parse_tasks_section(){
    expect(TASKS);
    while(true){
        Token peekT=lexer.peek(1);
        if(peekT.token_type==NUM){
            Token actual=lexer.GetToken();
            int val=std::stoi(actual.lexeme);
            if(find(tasks.begin(),tasks.end(),val)==tasks.end()){
                tasks.push_back(val);
            }
        }

        else{
            break;
        }
    }
}

void Parser::parse_num_list(){
    expect(NUM);
    Token t2=lexer.peek(1);
    if(t2.token_type==NUM){
        parse_num_list();
    }
}

void Parser::parse_poly_section(){
    expect(POLY);
    parse_poly_decl_list();
}

void Parser::parse_poly_decl_list(){
    parse_poly_decl();
    Token t=lexer.peek(1);
    if(t.token_type==ID){
        parse_poly_decl_list();
    }
    else if(t.token_type==EXECUTE){
        return;
    }
    else{
        syntax_error();
    }
}

void Parser::parse_poly_decl(){
    Polynomial p=parse_poly_header();
    addPolynomial(p.name,p.line_declared,p.params);
    currentPolyIndex=getPolynomialIndex(p.name);
    currentPolyHasParams=!(polynomials[currentPolyIndex].params.size()==1 && polynomials[currentPolyIndex].params[0]=="x");
    expect(EQUAL);
    PolyExpr* expr=parse_poly_expr();
    expect(SEMICOLON);
    polynomials[currentPolyIndex].body=expr;
    int deg=computeDegree(expr);
    polynomials[currentPolyIndex].degree=deg;
    currentPolyIndex=-1;
    currentPolyHasParams=false;
}

int Parser::computeDegree(PolyExpr* expr){
    if(expr==nullptr)
        return 0;
    switch(expr->type){
        case PolyExpr::LITERAL:
            return 0;
        case PolyExpr::VAR:
            return 1;
        case PolyExpr::ADD:
        case PolyExpr::SUB:
            return max(computeDegree(expr->left),computeDegree(expr->right));
        case PolyExpr::MUL:
            return computeDegree(expr->left)+computeDegree(expr->right);
        case PolyExpr::POW:
            return computeDegree(expr->left)*expr->exponent;
        default:
            return 0;
    }
}

Polynomial Parser::parse_poly_header(){
    Token t=expect(ID);
    Polynomial p;
    p.name=t.lexeme;
    p.line_declared=t.line_no;
    p.params.clear();
    Token t2=lexer.peek(1);
    if(t2.token_type==LPAREN){
        expect(LPAREN);
        vector<string> ids;
        parse_id_list(ids);
        expect(RPAREN);
        p.params=ids;
    }
    else{
        p.params.push_back("x");
    }
    return p;
}

void Parser::parse_id_list(vector<string> &idlist){
    Token t=expect(ID);
    idlist.push_back(t.lexeme);
    Token t2=lexer.peek(1);
    if(t2.token_type==COMMA){
        expect(COMMA);
        parse_id_list(idlist);
    }
}

int Parser::parse_poly_body(){
    return parse_term_list();
}

int Parser::parse_term_list(){
    int d1=parse_term();
    Token t=lexer.peek(1);
    if(t.token_type==PLUS || t.token_type==MINUS){
        parse_add_operator();
        int d2=parse_term_list();
        return max(d1,d2);
    }
    else{
        return d1;
    }
}

int Parser::parse_term(){
    Token t=lexer.peek(1);
    if(t.token_type==NUM){
        parse_coefficient();
        Token t2=lexer.peek(1);
        if(t2.token_type==ID || t2.token_type==LPAREN){
            int dm=parse_monomial_list();
            return dm;
        }
        return 0;
    }
    else if(t.token_type==ID || t.token_type==LPAREN){
        int dm=parse_monomial_list();
        return dm;
    }
    else{
        syntax_error();
        return 0;
    }
}

int Parser::parse_monomial_list(){
    int d1=parse_monomial();
    Token t=lexer.peek(1);
    if(t.token_type==ID || t.token_type==LPAREN){
        int d2=parse_monomial_list();
        return d1+d2;
    }
    else{
        return d1;
    }
}

int Parser::parse_monomial(){
    int dp=parse_primary();
    Token t=lexer.peek(1);
    if(t.token_type==POWER){
        int e=parse_exponent();
        return dp*e;
    }
    else{
        return dp;
    }
}

int Parser::parse_primary(){
    Token t=lexer.peek(1);
    if(t.token_type==ID){
        Token idToken=expect(ID);
        if(currentPolyIndex>=0 && !isValidVariableInCurrentPoly(idToken.lexeme)){
            invalidMonomial.push_back(idToken.line_no);
        }
        return 1;
    }
    else if(t.token_type==LPAREN){
        expect(LPAREN);
        int d=parse_term_list();
        expect(RPAREN);
        return d;
    }
    else{
        syntax_error();
        return 0;
    }
}

int Parser::parse_exponent(){
    expect(POWER);
    Token numTok=expect(NUM);
    return std::stoi(numTok.lexeme);
}

int Parser::parse_coefficient(){
    expect(NUM);
    return 0;
}

void Parser::parse_add_operator(){
    Token t=lexer.GetToken();
    if(t.token_type!=PLUS && t.token_type!=MINUS){
        syntax_error();
    }
}

void Parser::parse_execute_section(){
    expect(EXECUTE);
    parse_statement_list();
}

void Parser::parse_statement_list(){
    parse_statement();
    Token t=lexer.peek(1);
    if(t.token_type==INPUT || t.token_type==OUTPUT || t.token_type==ID){
        parse_statement_list();
    }
}

void Parser::parse_statement(){
    Token peekTok=lexer.peek(1);
    if(peekTok.token_type==INPUT){
        parse_input_statement();
    }
    else if(peekTok.token_type==OUTPUT){
        parse_output_statement();
    }
    else if(peekTok.token_type==ID){
        parse_assign_statement();
    }
    else{
        syntax_error();
    }
}

void Parser::parse_input_statement(){
    Token inTok=expect(INPUT);
    Token varTok=expect(ID);
    expect(SEMICOLON);
    Statement* st=new Statement;
    st->type=STMT_INPUT;
    st->var=varTok.lexeme;
    st->rhs=nullptr;
    st->lineNo=inTok.line_no;
    execStmts.push_back(st);
}

void Parser::parse_output_statement(){
    Token outTok=expect(OUTPUT);
    Token varTok=expect(ID);
    expect(SEMICOLON);
    Statement* st=new Statement;
    st->type=STMT_OUTPUT;
    st->var=varTok.lexeme;
    st->rhs=nullptr;
    st->lineNo=outTok.line_no;
    execStmts.push_back(st);
}

void Parser::parse_assign_statement(){
    Token lhsTok=expect(ID);
    expect(EQUAL);
    PolyEvalAST* rhsAst=parse_poly_evaluation();
    expect(SEMICOLON);
    Statement* st=new Statement;
    st->type=STMT_ASSIGN;
    st->var=lhsTok.lexeme;
    st->rhs=rhsAst;
    st->lineNo=lhsTok.line_no;
    execStmts.push_back(st);
}

PolyEvalAST* Parser::parse_poly_evaluation(){
    PolyEvalAST* root=new PolyEvalAST;
    root->isVar=false;
    root->isLiteral=false;
    root->lineNo=0;
    Token nameTok=expect(ID);
    root->polyName=nameTok.lexeme;
    root->lineNo=nameTok.line_no;
    if(!isDeclaredPolynomial(root->polyName)){
        undeclaredPoly.push_back(nameTok.line_no);
    }
    int idx=getPolynomialIndex(root->polyName);
    expect(LPAREN);
    if(lexer.peek(1).token_type==RPAREN){
        syntax_error();
    }
    vector<PolyEvalAST*> argList;
    parseArgumentList(argList);
    expect(RPAREN);
    if(idx>=0){
        if((int)argList.size()!=(int)polynomials[idx].params.size()){
            wrongNumArgs.push_back(nameTok.line_no);
        }
    }
    for(auto a:argList){
        root->args.push_back(a);
    }
    return root;
}

void Parser::parseArgumentList(vector<PolyEvalAST*> &args){
    PolyEvalAST* a=parseArgument();
    args.push_back(a);
    Token t=lexer.peek(1);
    if(t.token_type==COMMA){
        expect(COMMA);
        parseArgumentList(args);
    }
}

PolyEvalAST* Parser::parseArgument(){
    Token t=lexer.peek(1);
    if(t.token_type==ID){
        Token t2=lexer.peek(2);
        if(t2.token_type==LPAREN){
            return parse_poly_evaluation();
        }
        else{
            PolyEvalAST* node=new PolyEvalAST;
            node->isVar=true;
            node->isLiteral=false;
            Token varTok=expect(ID);
            node->polyName=varTok.lexeme;
            node->lineNo=varTok.line_no;
            return node;
        }
    }
    else if(t.token_type==NUM){
        PolyEvalAST* node=new PolyEvalAST;
        node->isVar=false;
        node->isLiteral=true;
        Token numTok=expect(NUM);
        node->literalValue=numTok.lexeme;
        node->polyName=node->literalValue;
        node->lineNo=numTok.line_no;
        return node;
    }
    else{
        syntax_error();
    }
    return nullptr;
}

void Parser::parse_inputs_section(){
    expect(INPUTS);
    Token peekToken=lexer.peek(1);
    if(peekToken.token_type!=NUM){
        syntax_error();
    }
    Token firstNum=lexer.GetToken();
    inputValues.push_back(std::stoi(firstNum.lexeme));
    while(lexer.peek(1).token_type==NUM){
        Token t=lexer.GetToken();
        inputValues.push_back(std::stoi(t.lexeme));
    }
}

void Parser::syntax_error(){
    cout<<"SYNTAX ERROR !!!!!&%!!"<<endl;
    exit(1);
}

Token Parser::expect(TokenType expected_type){
    Token t=lexer.GetToken();
    if(t.token_type!=expected_type){
        syntax_error();

    }
    return t;
}

void Parser::addPolynomial(const string &name,int lineNo,const vector<string> &params){
    auto it=polyMap.find(name);
    if(it!=polyMap.end()){
        duplicates.push_back(lineNo);

    }
    else{
        Polynomial poly;
        poly.name=name;
        poly.params=params;
        poly.line_declared=lineNo;
        poly.degree=0;
        polynomials.push_back(poly);
        int newIndex=(int)polynomials.size()-1;
        polyMap[name]=newIndex;
    }
}

bool Parser::isDeclaredPolynomial(const string &name){
    return (polyMap.find(name)!=polyMap.end());
}

int Parser::getPolynomialIndex(const string &name){
    auto it=polyMap.find(name);
    if(it==polyMap.end())
        return -1;
    return it->second;
}

bool Parser::isValidVariableInCurrentPoly(const string &id){
    if(currentPolyIndex<0 || currentPolyIndex>=(int)polynomials.size()){
        return true;
    }
    const Polynomial &poly=polynomials[currentPolyIndex];
    for(auto &pr:poly.params){
        if(pr==id)
            return true;
    }
    return false;
}

void Parser::reportSemanticErrorsAndExitIfAny(){
    if(!duplicates.empty()){
        sort(duplicates.begin(),duplicates.end());
        cout<<"Semantic Error Code 1: ";
        for(size_t i=0;i<duplicates.size();i++){
            cout<<duplicates[i]<<(i+1<duplicates.size()?" ":"");
        }
        cout<<endl;
        exit(0);
    }
    if(!invalidMonomial.empty()){
        sort(invalidMonomial.begin(),invalidMonomial.end());
        cout<<"Semantic Error Code 2: ";
        for(size_t i=0;i<invalidMonomial.size();i++){
            cout<<invalidMonomial[i]<<(i+1<invalidMonomial.size()?" ":"");
        }
        cout<<endl;
        exit(0);
    }
    if(!undeclaredPoly.empty()){
        sort(undeclaredPoly.begin(),undeclaredPoly.end());
        cout<<"Semantic Error Code 3: ";
        for(size_t i=0;i<undeclaredPoly.size();i++){
            cout<<undeclaredPoly[i]<<(i+1<undeclaredPoly.size()?" ":"");
        }
        cout<<endl;
        exit(0);
    }
    if(!wrongNumArgs.empty()){
        sort(wrongNumArgs.begin(),wrongNumArgs.end());
        cout<<"Semantic Error Code 4: ";
        for(size_t i=0;i<wrongNumArgs.size();i++){
            cout<<wrongNumArgs[i]<<(i+1<wrongNumArgs.size()?" ":"");
        }
        cout<<endl;
        exit(0);
    }
}

void Parser::freeAST(PolyEvalAST* root){
    if(!root)
        return;
    for(auto c:root->args){
        freeAST(c);
    }
    delete root;
}

void Parser::freeStatements(){
    for(auto st:execStmts){
        if(st->rhs){
            freeAST(st->rhs);
        }

        delete st;
    }
    execStmts.clear();
}

void Parser::checkUninitializedVariables(){
    set<string> definedVars;
    uninitializedWarnings.clear();
    for(auto st:execStmts){
        if(st->type==STMT_INPUT){
            definedVars.insert(st->var);

        }
        else if(st->type==STMT_ASSIGN){
            vector<PolyEvalAST*> stack;
            if(st->rhs)
                stack.push_back(st->rhs);
            while(!stack.empty()){
                PolyEvalAST* node=stack.back();
                stack.pop_back();
                if(node->isVar){
                    if(definedVars.find(node->polyName)==definedVars.end()){
                        uninitializedWarnings.push_back(node->lineNo);
                    }
                }
                for(auto child:node->args){
                    stack.push_back(child);
                }
            }
            definedVars.insert(st->var);
        }
    }
}

void Parser::checkUselessAssignmentsBackward(){
    uselessWarnings.clear();
    vector<set<string>> live(execStmts.size()+1);
    live[execStmts.size()]=set<string>();
    for(int i=execStmts.size()-1;i>=0;i--){
        Statement* st=execStmts[i];
        set<string> liveOut=live[i+1];
        set<string> liveIn;
        if(st->type==STMT_OUTPUT){
            liveIn=liveOut;
            liveIn.insert(st->var);
        }
        else if(st->type==STMT_ASSIGN){
            set<string> used;
            vector<PolyEvalAST*> stack;
            if(st->rhs)
                stack.push_back(st->rhs);
            while(!stack.empty()){
                PolyEvalAST* node=stack.back();
                stack.pop_back();
                if(node->isVar){
                    used.insert(node->polyName);
                }
                for(auto child:node->args){
                    stack.push_back(child);
                }
            }
            liveIn=liveOut;
            liveIn.erase(st->var);
            liveIn.insert(used.begin(),used.end());
            if(liveOut.find(st->var)==liveOut.end()){
                uselessWarnings.insert(uselessWarnings.end(),st->lineNo);
            }
        }
        else if(st->type==STMT_INPUT){
            liveIn=liveOut;
            liveIn.erase(st->var);

        }
        live[i]=liveIn;
    }
}

void Parser::executeProgram(){
    memory.clear();
    inputIndex=0;
    for(auto st:execStmts){
        if(st->type==STMT_INPUT){
            int val=(inputIndex<(int)inputValues.size())?inputValues[inputIndex++]:0;
            memory[st->var]=val;
        }
        else if(st->type==STMT_ASSIGN){
            int result=evalEvalAST(st->rhs);
            memory[st->var]=result;
         }
        else if(st->type==STMT_OUTPUT){
            int outVal=(memory.find(st->var)!=memory.end())?memory[st->var]:0;
            cout<<outVal<<endl;
        }
    }
}

int Parser::evalEvalAST(PolyEvalAST* ast){
    if(ast->isLiteral){
        return std::stoi(ast->literalValue);
    }
    else if(ast->isVar){
        return (memory.find(ast->polyName)!=memory.end())?memory[ast->polyName]:0;
    }
    else{
        vector<int> argValues;
        for(auto child:ast->args){
            argValues.push_back(evalEvalAST(child));
        }
        return evaluatePolynomial(ast->polyName,argValues);
    }
}

int Parser::evaluatePolynomial(const std::string &polyName,const vector<int>& args){
    int idx=getPolynomialIndex(polyName);
    if(idx<0)
        return 0;
    const Polynomial &poly=polynomials[idx];
    if(poly.body==nullptr)
        return 0;
    return evalPolyExpr(poly.body,poly.params,args);
}

int Parser::evalPolyExpr(PolyExpr* expr,const vector<string>& params,const vector<int>& argValues){
    if(expr==nullptr)
        return 0;
    switch(expr->type){
        case PolyExpr::LITERAL:
            return expr->value;
        case PolyExpr::VAR:{
            for(size_t i=0;i<params.size();i++){
                if(params[i]==expr->var)
                    return argValues[i];
            }
            return 0;
        }
        case PolyExpr::ADD:
            return evalPolyExpr(expr->left,params,argValues)+evalPolyExpr(expr->right,params,argValues);
        case PolyExpr::SUB:
            return evalPolyExpr(expr->left,params,argValues)-evalPolyExpr(expr->right,params,argValues);
        case PolyExpr::MUL:
            return evalPolyExpr(expr->left,params,argValues)*evalPolyExpr(expr->right,params,argValues);
        case PolyExpr::POW:{
            int base=evalPolyExpr(expr->left,params,argValues);
            int exp=expr->exponent;
            int result=1;
            for(int i=0;i<exp;i++)
                result*=base;
            return result;
        }
        default:
            return 0;
    }
}

PolyExpr* Parser::parse_poly_expr(){

    return parse_poly_term_list_expr();
}

PolyExpr* Parser::parse_poly_term_list_expr(){
    PolyExpr* left=parse_poly_term_expr();
    Token t=lexer.peek(1);
    if(t.token_type==PLUS || t.token_type==MINUS){
        Token op=expect(t.token_type);
        PolyExpr* right=parse_poly_term_list_expr();
        PolyExpr* node=new PolyExpr;
        node->type=(op.token_type==PLUS)?PolyExpr::ADD:PolyExpr::SUB;
        node->left=left;
        node->right=right;
        return node;
    }
    return left;
}

PolyExpr* Parser::parse_poly_term_expr(){
    Token t=lexer.peek(1);
    if(t.token_type==NUM){
        Token coeffTok=expect(NUM);
        int coeffValue=std::stoi(coeffTok.lexeme);
        Token t2=lexer.peek(1);
        if(t2.token_type==ID || t2.token_type==LPAREN){
            PolyExpr* monoListAST=parse_monomial_list_expr();
            PolyExpr* coeffNode=new PolyExpr;
            coeffNode->type=PolyExpr::LITERAL;
            coeffNode->value=coeffValue;
            coeffNode->left=coeffNode->right=nullptr;
            PolyExpr* mulNode=new PolyExpr;
            mulNode->type=PolyExpr::MUL;
            mulNode->left=coeffNode;
            mulNode->right=monoListAST;
            return mulNode;
        }
        else{
            PolyExpr* coeffNode=new PolyExpr;
            coeffNode->type=PolyExpr::LITERAL;
            coeffNode->value=coeffValue;
            coeffNode->left=coeffNode->right=nullptr;
            return coeffNode;
            
        }
    }
    else if(t.token_type==ID || t.token_type==LPAREN){
        return parse_monomial_list_expr();
    }
    else{
        syntax_error();
        return nullptr;
    }
}

PolyExpr* Parser::parse_monomial_list_expr(){
    PolyExpr* left=parse_monomial_expr();
    Token t=lexer.peek(1);
    if(t.token_type==ID || t.token_type==LPAREN){
        PolyExpr* right=parse_monomial_list_expr();
        PolyExpr* mulNode=new PolyExpr;
        mulNode->type=PolyExpr::MUL;
        mulNode->left=left;
        mulNode->right=right;
        return mulNode;
    }
    return left;
}

PolyExpr* Parser::parse_monomial_expr(){
    PolyExpr* p=parse_poly_primary_expr();
    Token t=lexer.peek(1);
    if(t.token_type==POWER){
        expect(POWER);
        Token numTok=expect(NUM);
        int exponentValue=std::stoi(numTok.lexeme);
        PolyExpr* powNode=new PolyExpr;
        powNode->type=PolyExpr::POW;
        powNode->left=p;
        powNode->right=nullptr;
        powNode->exponent=exponentValue;
        return powNode;
    }
    return p;
}

PolyExpr* Parser::parse_poly_monomial_expr(){
    PolyExpr* primary=parse_poly_primary_expr();
    Token t=lexer.peek(1);
    if(t.token_type==POWER){
        expect(POWER);
        Token numTok=expect(NUM);
        PolyExpr* node=new PolyExpr;
        node->type=PolyExpr::POW;
        node->left=primary;
        node->right=nullptr;
        node->exponent=std::stoi(numTok.lexeme);
        return node;
    }
    return primary;
}

PolyExpr* Parser::parse_poly_primary_expr(){
    Token t=lexer.peek(1);
    if(t.token_type==ID){
        Token idTok=expect(ID);
        if(currentPolyIndex>=0 && !isValidVariableInCurrentPoly(idTok.lexeme)){
            invalidMonomial.push_back(idTok.line_no);
        }
        PolyExpr* varNode=new PolyExpr;
        varNode->type=PolyExpr::VAR;
        varNode->var=idTok.lexeme;
        varNode->left=varNode->right=nullptr;
        return varNode;
    }
    else if(t.token_type==LPAREN){
         expect(LPAREN);
        PolyExpr* node=parse_poly_expr();
        expect(RPAREN);
        return node;
    }
    else{
         syntax_error();
        return nullptr;
    }
}

void Parser::printDegrees(){
    for(auto &p:polynomials){
        cout<<p.name<<": "<<p.degree<<endl;
    }
}