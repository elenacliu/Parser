#include <vector>

#include "type.h"
#include "LL1.h"

using namespace std;

int main() {
    vector<pair<string, string>> grams({
        {"S", "(S+F)"}, {"S", "F"}, {"F", "1"}
    });
    vector<Symbol> production({
        Symbol('F', NonTerm, false)
    });

    unordered_map<pair<Symbol, Symbol>, pair<Symbol, vector<Symbol>>, std::hash<pair<Symbol, Symbol>>> parsing_table;
    // map.emplace(a, b); 是直接将 a b视为构造参数传给了map
    parsing_table.emplace(make_pair(Symbol('F', NonTerm, false), Symbol('1', Term, false)), make_pair(Symbol('F', NonTerm, false), vector<Symbol>()));
    // 
    // parsing_table[make_pair(Symbol('F', NonTerm, false), Symbol('1', Term, false))] = make_pair(Symbol('F', NonTerm, false), vector<Symbol>());
    // 报错的本质原因是 symbol 没有默认构造函数
    // parsing_table[make_pair(Symbol('F', NonTerm, false), Symbol('1', Term, false))] = {Symbol('F', NonTerm, false), {}};
    auto it = find_if(production.begin(), production.end(), [](Symbol i){return i == Symbol('F', NonTerm, false);});
    auto next_it = ++it;

    if (next_it == production.end()) {
        cout << "yes" << endl;
    } else {
        cout << "no" << endl;
    }
    int a = 1;
    a++;
    return 0;
}