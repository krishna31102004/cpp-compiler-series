
//Name:Krishna Balaji
//ASU ID:1226254343
//Class:CSE 340
//Section:20660







#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <algorithm>
#include "lexer.h"
using namespace std;

struct Rule{
    string lhs;
    vector<string> rhs;
};

vector<Rule> grammar;
vector<string> overallAppearanceOrder;

void recordAppearance(const string &s) {
    if(find(overallAppearanceOrder.begin(), overallAppearanceOrder.end(), s)==overallAppearanceOrder.end()){
        overallAppearanceOrder.push_back(s);
    }
}

void ReadGrammar(){
    LexicalAnalyzer lexer;
    Token token = lexer.GetToken();
    while(token.token_type != HASH){
        if(token.token_type != ID){
            cout << "SYNTAX ERROR !!!!!!!!!!!!!!\n";
            exit(1);
        }
        recordAppearance(token.lexeme);
        string lhs = token.lexeme;
        token = lexer.GetToken();
        if(token.token_type != ARROW){
            cout << "SYNTAX ERROR !!!!!!!!!!!!!!\n";
            exit(1);
        }
        bool done = false;
        while(!done){
            vector<string> production;
            token = lexer.GetToken();
            if(token.token_type == STAR){
                // epsilon production
            } else {
                while(token.token_type == ID){
                    recordAppearance(token.lexeme);
                    production.push_back(token.lexeme);
                    token = lexer.GetToken();
                }
            }
            grammar.push_back({lhs, production});
            if(token.token_type == OR){
                continue;
            } else if(token.token_type == STAR){
                done = true;
            } else {
                cout << "SYNTAX ERROR !!!!!!!!!!!!!!\n";
                exit(1);
            }
        }
        token = lexer.GetToken();
    }
}

void Task1(){ 
    set<string>nonterminalSet;
    for(size_t i=0;i<grammar.size();i++){
        nonterminalSet.insert(grammar[i].lhs);
    }
    vector<string>terminals;
    vector<string>nonterminals;
    for(size_t i=0;i<overallAppearanceOrder.size();i++){
        string sym=overallAppearanceOrder[i];
        if(nonterminalSet.find(sym)==nonterminalSet.end())
            terminals.push_back(sym);
        else
            nonterminals.push_back(sym);
    }
    bool firstPrinted=false;
    for(size_t i=0;i<terminals.size();i++){
        if(firstPrinted) cout<<" ";
        cout<<terminals[i];
        firstPrinted=true;
    }
    for(size_t i=0;i<nonterminals.size();i++){
        if(firstPrinted) cout<<" ";
        cout<<nonterminals[i];
        firstPrinted=true;
    }
    cout<<endl;
    cout.flush();
}

void Task2() {
    set<string> nonterminalSet;
    for (size_t i = 0; i < grammar.size(); i++) {
        nonterminalSet.insert(grammar[i].lhs);
    }
    set<string> nullableSet;
    for (size_t i = 0; i < grammar.size(); i++) {
        if (grammar[i].rhs.empty()) {
            nullableSet.insert(grammar[i].lhs);
        }
    }
    bool changed = true;
    while (changed) {
        changed = false;
        for (size_t i = 0; i < grammar.size(); i++) {
            string A = grammar[i].lhs;
            if (nullableSet.find(A) != nullableSet.end())
                continue;
            bool allNullable = true;
            if (!grammar[i].rhs.empty()) {
                for (size_t j = 0; j < grammar[i].rhs.size(); j++) {
                    string sym = grammar[i].rhs[j];
                    if (nonterminalSet.find(sym) != nonterminalSet.end()) {
                        if (nullableSet.find(sym) == nullableSet.end()) {
                            allNullable = false;
                            break;
                        }
                    } else {
                        allNullable = false;
                        break;
                    }
                }
            }
            if (allNullable) {
                nullableSet.insert(A);
                changed = true;
            }
        }
    }
    vector<string> orderedNullable;
    for (size_t i = 0; i < overallAppearanceOrder.size(); i++) {
        string sym = overallAppearanceOrder[i];
        if (nonterminalSet.find(sym) != nonterminalSet.end() && nullableSet.find(sym) != nullableSet.end()) {
            orderedNullable.push_back(sym);
        }
    }
    cout << "Nullable = { ";
    for (size_t i = 0; i < orderedNullable.size(); i++) {
        cout << orderedNullable[i];
        if (i < orderedNullable.size() - 1)
            cout << ", ";
    }
    cout << " }" << "\n";
}

void Task3(){
    set<string> nonterminalSet;
    for (size_t i = 0; i < grammar.size(); i++){
        nonterminalSet.insert(grammar[i].lhs);
    }
    set<string> nullableSet;
    for (size_t i = 0; i < grammar.size(); i++){
        if(grammar[i].rhs.empty()){
            nullableSet.insert(grammar[i].lhs);
        }
    }
    bool changed = true;
    while(changed){
        changed = false;
        for(size_t i = 0; i < grammar.size(); i++){
            string A = grammar[i].lhs;
            if(nullableSet.find(A) != nullableSet.end()) continue;
            if(!grammar[i].rhs.empty()){
                bool allNullable = true;
                for(size_t j = 0; j < grammar[i].rhs.size(); j++){
                    string sym = grammar[i].rhs[j];
                    if(nonterminalSet.find(sym) != nonterminalSet.end()){
                        if(nullableSet.find(sym) == nullableSet.end()){
                            allNullable = false;
                            break;
                        }
                    } else {
                        allNullable = false;
                        break;
                    }
                }
                if(allNullable){
                    nullableSet.insert(A);
                    changed = true;
                }
            }
        }
    }
    map<string, vector<string>> firstMap;
    for(size_t i = 0; i < overallAppearanceOrder.size(); i++){
        string sym = overallAppearanceOrder[i];
        if(nonterminalSet.find(sym) == nonterminalSet.end()){
            firstMap[sym].push_back(sym);
        } else {
            if(firstMap.find(sym) == firstMap.end())
                firstMap[sym] = vector<string>();
        }
    }
    bool changedFirst = true;
    while(changedFirst){
        changedFirst = false;
        for(size_t i = 0; i < grammar.size(); i++){
            string A = grammar[i].lhs;
            vector<string> & firstA = firstMap[A];
            bool allNullable = true;
            for(size_t j = 0; j < grammar[i].rhs.size(); j++){
                string X = grammar[i].rhs[j];
                vector<string> firstX;
                if(nonterminalSet.find(X) == nonterminalSet.end()){
                    firstX.push_back(X);
                } else {
                    firstX = firstMap[X];
                }
                for(size_t k = 0; k < firstX.size(); k++){
                    string t = firstX[k];
                    if(t=="EPSILON") continue;
                    if(find(firstA.begin(), firstA.end(), t) == firstA.end()){
                        firstA.push_back(t);
                        changedFirst = true;
                    }
                }
                if(nonterminalSet.find(X) != nonterminalSet.end()){
                    if(nullableSet.find(X) == nullableSet.end()){
                        allNullable = false;
                        break;
                    }
                } else {
                    allNullable = false;
                    break;
                }
            }
            if((grammar[i].rhs.empty() || allNullable) && find(firstA.begin(), firstA.end(), "EPSILON") == firstA.end()){
                firstA.push_back("EPSILON");
                changedFirst = true;
            }
        }
    }
    for(size_t i = 0; i < overallAppearanceOrder.size(); i++){
        string A = overallAppearanceOrder[i];
        if(nonterminalSet.find(A) != nonterminalSet.end()){
            vector<string> firstSet = firstMap[A];
            vector<string> orderedFirst;
            for(size_t j = 0; j < overallAppearanceOrder.size(); j++){
                string ref = overallAppearanceOrder[j];
                if(find(firstSet.begin(), firstSet.end(), ref) != firstSet.end() && ref != "EPSILON"){
                    orderedFirst.push_back(ref);
                }
            }
            cout << "FIRST(" << A << ") = { ";
            for(size_t j = 0; j < orderedFirst.size(); j++){
                cout << orderedFirst[j];
                if(j < orderedFirst.size()-1)
                    cout << ", ";
            }
            cout << " }" << endl;
        }
    }
}

vector<string> computeFirstOfSequence(const vector<string>& seq, const set<string>& nonterminalSet, const map<string, vector<string>>& firstMap, const set<string>& nullableSet){
    vector<string> result;
    bool allNullable = true;
    for(auto & sym : seq){
        vector<string> firstSym;
        if(nonterminalSet.find(sym) == nonterminalSet.end()){
            firstSym.push_back(sym);
        } else {
            firstSym = firstMap.at(sym);
        }
        for(auto & t : firstSym){
            if(t=="EPSILON") continue;
            if(find(result.begin(), result.end(), t) == result.end()){
                result.push_back(t);
            }
        }
        if(nonterminalSet.find(sym) != nonterminalSet.end()){
            if(nullableSet.find(sym)==nullableSet.end()){
                allNullable = false;
                break;
            }
        } else {
            allNullable = false;
            break;
        }
    }
    if(allNullable)
        result.push_back("EPSILON");
    return result;
}

void Task4(){
    set<string> nonterminalSet;
    for(size_t i = 0; i < grammar.size(); i++){
        nonterminalSet.insert(grammar[i].lhs);
    }
    set<string> nullableSet;
    for(size_t i = 0; i < grammar.size(); i++){
        if(grammar[i].rhs.empty()){
            nullableSet.insert(grammar[i].lhs);
        }
    }
    bool changedNullable = true;
    while(changedNullable){
        changedNullable = false;
        for(size_t i = 0; i < grammar.size(); i++){
            string A = grammar[i].lhs;
            if(nullableSet.find(A)!=nullableSet.end()) continue;
            bool allNullable = true;
            if(!grammar[i].rhs.empty()){
                for(size_t j = 0; j < grammar[i].rhs.size(); j++){
                    string sym = grammar[i].rhs[j];
                    if(nonterminalSet.find(sym)!=nonterminalSet.end()){
                        if(nullableSet.find(sym)==nullableSet.end()){
                            allNullable = false;
                            break;
                        }
                    } else {
                        allNullable = false;
                        break;
                    }
                }
            }
            if(allNullable){
                nullableSet.insert(A);
                changedNullable = true;
            }
        }
    }
    map<string, vector<string>> firstMap;
    for(size_t i = 0; i < overallAppearanceOrder.size(); i++){
        string sym = overallAppearanceOrder[i];
        if(nonterminalSet.find(sym)==nonterminalSet.end()){
            firstMap[sym].push_back(sym);
        } else {
            if(firstMap.find(sym)==firstMap.end())
                firstMap[sym] = vector<string>();
        }
    }
    bool changedFirst = true;
    while(changedFirst){
        changedFirst = false;
        for(size_t i = 0; i < grammar.size(); i++){
            string A = grammar[i].lhs;
            vector<string>& firstA = firstMap[A];
            bool allNullable = true;
            for(size_t j = 0; j < grammar[i].rhs.size(); j++){
                string X = grammar[i].rhs[j];
                vector<string> firstX;
                if(nonterminalSet.find(X)==nonterminalSet.end()){
                    firstX.push_back(X);
                } else {
                    firstX = firstMap[X];
                }
                for(size_t k = 0; k < firstX.size(); k++){
                    string t = firstX[k];
                    if(t=="EPSILON") continue;
                    if(find(firstA.begin(), firstA.end(), t) == firstA.end()){
                        firstA.push_back(t);
                        changedFirst = true;
                    }
                }
                if(nonterminalSet.find(X)!=nonterminalSet.end()){
                    if(nullableSet.find(X)==nullableSet.end()){
                        allNullable = false;
                        break;
                    }
                } else {
                    allNullable = false;
                    break;
                }
            }
            if((grammar[i].rhs.empty() || allNullable) && find(firstA.begin(), firstA.end(), "EPSILON") == firstA.end()){
                firstA.push_back("EPSILON");
                changedFirst = true;
            }
        }
    }
    map<string, vector<string>> followMap;
    for(auto & nt : nonterminalSet){
        followMap[nt] = vector<string>();
    }
    string startSymbol = "";
    for(auto & sym : overallAppearanceOrder){
        if(nonterminalSet.find(sym)!=nonterminalSet.end()){
            startSymbol = sym;
            break;
        }
    }
    followMap[startSymbol].push_back("$");
    bool changedFollow = true;
    while(changedFollow){
        changedFollow = false;
        for(size_t i = 0; i < grammar.size(); i++){
            string A = grammar[i].lhs;
            const vector<string>& rhs = grammar[i].rhs;
            for(size_t j = 0; j < rhs.size(); j++){
                string B = rhs[j];
                if(nonterminalSet.find(B)!=nonterminalSet.end()){
                    vector<string> beta;
                    for(size_t k = j+1; k < rhs.size(); k++){
                        beta.push_back(rhs[k]);
                    }
                    vector<string> firstBeta = computeFirstOfSequence(beta, nonterminalSet, firstMap, nullableSet);
                    for(auto & t : firstBeta){
                        if(t=="EPSILON") continue;
                        if(find(followMap[B].begin(), followMap[B].end(), t)==followMap[B].end()){
                            followMap[B].push_back(t);
                            changedFollow = true;
                        }
                    }
                    bool betaAllNullable = true;
                    for(auto & symBeta : beta){
                        if(nonterminalSet.find(symBeta)!=nonterminalSet.end()){
                            if(nullableSet.find(symBeta)==nullableSet.end()){
                                betaAllNullable = false;
                                break;
                            }
                        } else {
                            betaAllNullable = false;
                            break;
                        }
                    }
                    if(beta.empty() || betaAllNullable){
                        for(auto & t : followMap[A]){
                            if(find(followMap[B].begin(), followMap[B].end(), t)==followMap[B].end()){
                                followMap[B].push_back(t);
                                changedFollow = true;
                            }
                        }
                    }
                }
            }
        }
    }
    for(size_t i = 0; i < overallAppearanceOrder.size(); i++){
        string A = overallAppearanceOrder[i];
        if(nonterminalSet.find(A)!=nonterminalSet.end()){
            vector<string> followSet = followMap[A];
            vector<string> orderedFollow;
            if(find(followSet.begin(), followSet.end(), "$") != followSet.end())
                orderedFollow.push_back("$");
            for(size_t j = 0; j < overallAppearanceOrder.size(); j++){
                string sym = overallAppearanceOrder[j];
                if(sym != "$" && find(followSet.begin(), followSet.end(), sym) != followSet.end()){
                    if(find(orderedFollow.begin(), orderedFollow.end(), sym)==orderedFollow.end())
                        orderedFollow.push_back(sym);
                }
            }
            cout << "FOLLOW(" << A << ") = { ";
            for(size_t j = 0; j < orderedFollow.size(); j++){
                cout << orderedFollow[j];
                if(j < orderedFollow.size()-1)
                    cout << ", ";
            }
            cout << " }" << endl;
        }
    }
}

void Task5(){
    bool factored = true;
    map<string, int> factorCount;
    while(factored){
        factored = false;
        vector<Rule> newGrammar;
        map<string, vector<Rule>> groups;
        for(auto & r : grammar){
            groups[r.lhs].push_back(r);
        }
        for(auto & entry : groups){
            string A = entry.first;
            vector<Rule> rules = entry.second;
            if(rules.size() < 2){
                for(auto & r : rules)
                    newGrammar.push_back(r);
                continue;
            }
            int bestL = 0;
            vector<string> bestPrefix;
            int maxPossible = 0;
            for(auto & r : rules){
                if((int)r.rhs.size() > maxPossible)
                    maxPossible = r.rhs.size();
            }
            for(int L = 1; L <= maxPossible; L++){
                map<vector<string>, vector<int>> prefixMap;
                for(int i = 0; i < (int)rules.size(); i++){
                    if((int)rules[i].rhs.size() >= L){
                        vector<string> prefix(rules[i].rhs.begin(), rules[i].rhs.begin()+L);
                        prefixMap[prefix].push_back(i);
                    }
                }
                for(auto & p : prefixMap){
                    if(p.second.size() >= 2){
                        if(L > bestL){
                            bestL = L;
                            bestPrefix = p.first;
                        } else if(L == bestL){
                            if(p.first < bestPrefix)
                                bestPrefix = p.first;
                        }
                    }
                }
            }
            if(bestL == 0){
                for(auto & r : rules)
                    newGrammar.push_back(r);
            } else {
                factored = true;
                vector<Rule> withPrefix, withoutPrefix;
                for(auto & r : rules){
                    if((int)r.rhs.size() >= bestL){
                        bool match = true;
                        for(int k = 0; k < bestL; k++){
                            if(r.rhs[k] != bestPrefix[k]){
                                match = false;
                                break;
                            }
                        }
                        if(match)
                            withPrefix.push_back(r);
                        else
                            withoutPrefix.push_back(r);
                    } else {
                        withoutPrefix.push_back(r);
                    }
                }
                int count = factorCount[A] + 1;
                factorCount[A] = count;
                string newNonterminal = A + to_string(count);
                vector<string> newRhs = bestPrefix;
                newRhs.push_back(newNonterminal);
                Rule factoredRule;
                factoredRule.lhs = A;
                factoredRule.rhs = newRhs;
                newGrammar.push_back(factoredRule);
                for(auto & r : withPrefix){
                    vector<string> remainder;
                    if((int)r.rhs.size() > bestL)
                        remainder.insert(remainder.end(), r.rhs.begin()+bestL, r.rhs.end());
                    Rule newRule;
                    newRule.lhs = newNonterminal;
                    newRule.rhs = remainder;
                    newGrammar.push_back(newRule);
                }
                for(auto & r : withoutPrefix){
                    newGrammar.push_back(r);
                }
            }
        }
        grammar = newGrammar;
    }
    sort(grammar.begin(), grammar.end(), [](const Rule & r1, const Rule & r2) {
        if(r1.lhs != r2.lhs)
            return r1.lhs < r2.lhs;
        return r1.rhs < r2.rhs;
    });
    for(auto & r : grammar){
        cout << r.lhs << " ->";
        for(auto & sym : r.rhs)
            cout << " " << sym;
        cout << " #" << endl;
    }
}

void Task6(){
    map<string, vector<Rule>> rulesMap;
    set<string> originalNTSet;
    for(auto & r : grammar){
        rulesMap[r.lhs].push_back(r);
        originalNTSet.insert(r.lhs);
    }
    vector<string> originalNT(originalNTSet.begin(), originalNTSet.end());
    sort(originalNT.begin(), originalNT.end());
    map<string, vector<Rule>> newRulesMap;
    map<string, int> newNameCount;
    map<string, string> removedNT;
    for(string A : originalNT){
        for(string B : originalNT){
            if(B >= A) break;
            vector<Rule> newList;
            for(auto & r : rulesMap[A]){
                if(!r.rhs.empty() && r.rhs[0] == B){
                    for(auto & rB : rulesMap[B]){
                        Rule newRule;
                        newRule.lhs = A;
                        newRule.rhs = rB.rhs;
                        for(size_t k = 1; k < r.rhs.size(); k++){
                            newRule.rhs.push_back(r.rhs[k]);
                        }
                        newList.push_back(newRule);
                    }
                } else {
                    newList.push_back(r);
                }
            }
            rulesMap[A] = newList;
        }
        vector<Rule> alpha;
        vector<Rule> beta;
        for(auto & r : rulesMap[A]){
            if(!r.rhs.empty() && r.rhs[0] == A){
                Rule newRule;
                newRule.lhs = A;
                newRule.rhs.assign(r.rhs.begin()+1, r.rhs.end());
                alpha.push_back(newRule);
            } else {
                beta.push_back(r);
            }
        }
        bool A_removed = false;
        vector<Rule> newRulesForA;
        if(alpha.empty()){
            newRulesForA = beta;
        } else {
            if(beta.empty()){
                A_removed = true;
            } else {
                for(auto & r : beta){
                    Rule newRule;
                    newRule.lhs = A;
                    newRule.rhs = r.rhs;
                    newRulesForA.push_back(newRule);
                }
            }
            int count = newNameCount[A] + 1;
            newNameCount[A] = count;
            string Aprime = A + to_string(count);
            if(!beta.empty()){
                for(auto & r : newRulesForA){
                    r.rhs.push_back(Aprime);
                }
            }
            vector<Rule> rulesForAprime;
            for(auto & r : alpha){
                Rule newRule;
                newRule.lhs = Aprime;
                newRule.rhs = r.rhs;
                newRule.rhs.push_back(Aprime);
                rulesForAprime.push_back(newRule);
            }
            Rule epsilonRule;
            epsilonRule.lhs = Aprime;
            rulesForAprime.push_back(epsilonRule);
            newRulesMap[Aprime] = rulesForAprime;
            if(A_removed){
                removedNT[A] = Aprime;
                rulesMap.erase(A);
            } else {
                rulesMap[A] = newRulesForA;
            }
        }
    }
    vector<Rule> finalGrammar;
    for(auto & entry : rulesMap){
        for(auto & r : entry.second){
            finalGrammar.push_back(r);
        }
    }
    for(auto & entry : newRulesMap){
        for(auto & r : entry.second){
            finalGrammar.push_back(r);
        }
    }
    sort(finalGrammar.begin(), finalGrammar.end(), [](const Rule & r1, const Rule & r2) {
        if(r1.lhs != r2.lhs)
            return r1.lhs < r2.lhs;
        return r1.rhs < r2.rhs;
    });
    for(auto & r : finalGrammar){
        cout << r.lhs << " ->";
        for(auto & sym : r.rhs)
            cout << " " << sym;
        cout << " #" << endl;
    }
}

int main(int argc, char* argv[]){
    int task;
    if(argc < 2){
        cout << "Error: missing argument\n";
        return 1;
    }
    task = atoi(argv[1]);
    ReadGrammar();
    switch(task){
        case 1: Task1(); break;
        case 2: Task2(); break;
        case 3: Task3(); break;
        case 4: Task4(); break;
        case 5: Task5(); break;
        case 6: Task6(); break;
        default: cout << "Error: unrecognized task number " << task << "\n"; break;
    }
    return 0;
}
