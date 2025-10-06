//Name:Krishna Balaji
//ASU ID:1226254343
//Class:CSE 340
//Section:20660




#ifndef PARSER_H
#define PARSER_H
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <set>
#include "lexer.h"
enum StmtType{STMT_INPUT,STMT_OUTPUT,STMT_ASSIGN};
struct PolyExpr{
    enum Type{LITERAL,VAR,ADD,SUB,MUL,POW} type;
    int value;
    std::string var;
    PolyExpr* left;
    PolyExpr* right;
    int exponent;
};
struct Polynomial{
    std::string name;
    std::vector<std::string> params;
    int line_declared;
    int degree;
    PolyExpr* body;
};
struct PolyEvalAST{
    std::string polyName;
    bool isVar;
    bool isLiteral;
    std::string literalValue;
    int lineNo;
    std::vector<PolyEvalAST*> args;
};
struct Statement{
    StmtType type;
    std::string var;
    PolyEvalAST* rhs;
    int lineNo;
};
class Parser{
public:
    Parser();
    int run();
private:
    LexicalAnalyzer lexer;
    void syntax_error();
    Token expect(TokenType expected_type);
    int evaluatePolynomial(const std::string &polyName,const std::vector<int>& args);
    int evalPolyExpr(PolyExpr* expr,const std::vector<std::string>& params,const std::vector<int>& argValues);
    PolyExpr* parse_poly_term_list_expr();
    PolyExpr* parse_poly_term_expr();
    PolyExpr* parse_poly_monomial_expr();
    PolyExpr* parse_poly_primary_expr();
    PolyExpr* parse_poly_expr();
    PolyExpr* parse_monomial_list_expr();
    PolyExpr* parse_monomial_expr();
    int computeDegree(PolyExpr* expr);
    void parseInput();
    void parse_program();
    void parse_tasks_section();
    void parse_num_list();
    void parse_poly_section();
    void parse_poly_decl_list();
    void parse_poly_decl();
    Polynomial parse_poly_header();
    void parse_id_list(std::vector<std::string> &idlist);
    int parse_poly_body();
    int parse_term_list();
    int parse_term();
    int parse_monomial_list();
    int parse_monomial();
    int parse_primary();
    int parse_exponent();
    int parse_coefficient();
    void parse_add_operator();
    void parse_execute_section();
    void parse_statement_list();
    void parse_statement();
    void parse_input_statement();
    void parse_output_statement();
    void parse_assign_statement();
    PolyEvalAST* parse_poly_evaluation();
    void parseArgumentList(std::vector<PolyEvalAST*> &args);
    PolyEvalAST* parseArgument();
    void parse_inputs_section();
    std::vector<Polynomial> polynomials;
    std::map<std::string,int> polyMap;
    std::vector<int> duplicates;
    std::vector<int> invalidMonomial;
    std::vector<int> undeclaredPoly;
    std::vector<int> wrongNumArgs;
    int currentPolyIndex;
    bool currentPolyHasParams;
    void addPolynomial(const std::string &name,int lineNo,const std::vector<std::string> &params);
    bool isDeclaredPolynomial(const std::string &name);
    int getPolynomialIndex(const std::string &name);
    bool isValidVariableInCurrentPoly(const std::string &id);
    void reportSemanticErrorsAndExitIfAny();
    std::vector<Statement*> execStmts;
    std::vector<int> tasks;
    std::vector<int> uninitializedWarnings;
    void checkUninitializedVariables();
    std::vector<int> uselessWarnings;
    void checkUselessAssignmentsForward();
    void checkUselessAssignmentsBackward();
    void printDegrees();
    void executeProgram();
    int inputIndex;
    std::vector<int> inputValues;
    std::map<std::string,int> memory;
    int evalEvalAST(PolyEvalAST* ast);
    void freeAST(PolyEvalAST* root);
    void freeStatements();
};
#endif