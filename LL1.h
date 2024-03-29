#ifndef LL1_H
#define LL1_H

#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cassert>

#include "type.h"
#include "tree.h"

#define assertm(exp, msg) assert(((void)msg, exp))

using namespace std;


class Parser {
    /**
        Default: grammar is CFG and there is no need to pre-proccess the grammar.    
    **/
    unordered_map<Symbol, vector<vector<Symbol>>, std::hash<Symbol>> grammars;
    unordered_set<Symbol, std::hash<Symbol>> symbols;
    unordered_map<pair<Symbol, Symbol>, pair<Symbol, vector<Symbol>>, std::hash<pair<Symbol, Symbol>>> parsing_table;
    unordered_map<Symbol, unordered_set<Symbol, std::hash<Symbol>>, std::hash<Symbol>> first;
    unordered_map<Symbol, unordered_set<Symbol, std::hash<Symbol>>, std::hash<Symbol>> follow;
    stack<Symbol> stk;

public:
    Parser(vector<pair<string, string>>& grams) {
        int n = grams.size();
        // read in grammars
        for (int i = 0; i < n; i++) {
            // CFG
            assert(grams[i].first.length() == 1);
            // NonTerm
            assert(grams[i].first[0] >= 'A' && grams[i].first[0] <= 'Z');
            bool isStart = grams[i].first[0] == 'S';
            Symbol left_nonterm = Symbol(grams[i].first[0], SymbolType::NonTerm, isStart);
            symbols.insert(left_nonterm);
            int m = grams[i].second.length();
            if (m == 0) {
                Symbol right = Symbol('\0', SymbolType::Term);
                symbols.insert(right);
                grammars[left_nonterm].push_back({right});
                continue;
            }
            vector<Symbol> rhs;
            for (int j = 0; j < m; j++) {
                SymbolType type = SymbolType::Term;
                bool isStart = false;
                if (grams[i].second[j] >= 'A' && grams[i].second[j] <= 'Z') {
                    type = SymbolType::NonTerm;
                    isStart = grams[i].second[j] == 'S';
                }
                Symbol right = Symbol(grams[i].second[j], type, isStart);
                symbols.insert(right);
                rhs.push_back(right);
            }
            grammars[left_nonterm].push_back(rhs);
        }
        for (auto it = grammars.begin(); it != grammars.end(); it++) {
            cout << it->first << ": " << endl;
            for (int j = 0; j < it->second.size(); j++) {
                for (int k = 0; k < it->second[j].size(); k++) {
                    cout << it->second[j][k];
                }
                cout << endl;
            }
        }
    }
    void buildFirsts(Symbol symbol) {
        if (!first[symbol].empty()) {
            // if has been constructed
            // return directly
            return;
        }
        if (symbol.getType() == SymbolType::Term) {
            first[symbol].insert(symbol);
            return;
        }
        // first 是针对每个symbol自己为lhs的产生式
        // only lhs has productions
        // 分析rhs得到答案
        vector<vector<Symbol>> productions;
        if (grammars.find(symbol) != grammars.end()) {
            productions = grammars.at(symbol);
        }
        for (auto production : productions) {
            for (Symbol s : production) {
                // if the first symbol is nonterm
                if (s.getType() == SymbolType::Term) {
                    first[symbol].insert(s);
                    parsing_table.emplace(std::make_pair(std::make_pair(symbol, s), std::make_pair(symbol, production)));
                    break;
                } else {
                    // s is also a non-term
                    buildFirsts(s);
                    for (auto first_it = first[s].begin(); first_it != first[s].end(); first_it++) {
                        parsing_table.emplace(std::make_pair(std::make_pair(symbol, *first_it), std::make_pair(symbol, production)));
                    }
                    first[symbol].insert(first[s].begin(), first[s].end());
                    // 如果 first[s] 不包含 epsilon, 不用继续迭代了，直接合并set得到答案
                    if (first[s].find(Symbol('\0', SymbolType::Term)) == first[s].end()) {
                        break;
                    }
                }
            }
        }
    }

    void constructFirstSets() {
        /**
        每个symbol都要用递归来构建firstset
        **/
        for (auto symbol : symbols) {
            buildFirsts(symbol);
        }
    }

    void buildFollows(Symbol symbol) {
        if (!follow[symbol].empty()) {
            return;
        }
        if (symbol.isStart()) {
            follow[symbol].insert(Symbol('$', SymbolType::Term));
        }
        // 答案要从rhs中含有 symbol 的产生式里得到
        auto productions = getProductionsWithSymbol(symbol);
        for (auto production : productions) {
            // production : pair<Symbol, vector<Symbol>>
            // symbol_it 一定不是 end
            auto symbol_it = find(production.second.begin(), production.second.end(), symbol);
            auto next_it = symbol_it;
            assert(symbol_it != production.second.end());
            while ((++next_it) !=  production.second.end()) {
                // 说明 symbol 的 follow 需要将 next 的 first 包含进去
                // first 一定创建好了，所以不用递归
                // 如果 first[*next_it] 不包含 epsilon
                assert(symbols.count(*next_it) > 0);
                assert(first.find(*next_it) != first.end());
                follow[symbol].insert(first[*next_it].begin(), first[*next_it].end());
                if (first[*next_it].find(Symbol('\0', SymbolType::Term)) == first[*next_it].end()) {
                    break;
                }
            }
            // 说明symbol_it指向的已经是最后一个symbol了
            if (next_it == production.second.end() && symbol != production.first) {
                buildFollows(production.first);
                follow[symbol].insert(follow[production.first].begin(), follow[production.first].end());
            }
        }
    }

    vector<pair<Symbol, vector<Symbol>>> getProductionsWithSymbol(const Symbol& symbol) {
        vector<pair<Symbol, vector<Symbol>>> ans;
        for (auto it = grammars.begin(); it != grammars.end(); ++it) {
            for (auto& production : it->second) {
                for (Symbol& s : production) {
                    if (s == symbol) {
                        ans.push_back({it->first, production});
                        break;
                    }
                }
            }
        }
        return ans;
    }

    void constructFollowSets() {
        for (auto symbol : symbols) {
            buildFollows(symbol);
        }
    }
    void constructParseTable() {
        constructFirstSets();
        print(parsing_table);
        cout << endl;
        constructFollowSets();
        cout << "Firsts: " << endl;
        print(this->first);
        cout << "Follows: " << endl;
        print(this->follow);

        // 遍历 first 的每个非终结符
        // 如果它的first中有epsi
        // 那么考虑将parsing_table[{s, e in follow[s]}] = parsing_table[{s, \epsi}]
        // for s in follow[NonTerm]:
        //    parsing_table[{NonTerm, s}] = {NonTerm, production};
        auto epsi = Symbol('\0', SymbolType::Term, false);
        for (auto nonterm : first) {
            if (nonterm.first.getType() == SymbolType::NonTerm) {
                if (first[nonterm.first].count(epsi) > 0) {
                    for (auto term : follow[nonterm.first]) {
                        if (term.getType() == SymbolType::Term) {
                            parsing_table[{nonterm.first, term}] = parsing_table[{nonterm.first, epsi}];
                        }
                    }
                }
            }
        }
        print(parsing_table);
    }

    void print(unordered_map<Symbol, unordered_set<Symbol, hash<Symbol>>, hash<Symbol>>& s) {
        for (auto it = s.begin(); it != s.end(); ++it) {
            cout << it->first << ": {";
            for (auto element : it->second) {
                cout << element;
            }
            cout << "}" << endl;
        }
    }

    void print(unordered_map<pair<Symbol, Symbol>, pair<Symbol, vector<Symbol>>, std::hash<pair<Symbol, Symbol>>>& parsing_table) {
        for (auto& term_pair : parsing_table) {
            cout << "key1, key2: " << term_pair.first.first << term_pair.first.second << endl;
            for (auto s: term_pair.second.second) {
                cout << s << endl;
            }
        }
    }

    bool parse(string s) {
        stk.push(Symbol('$', SymbolType::Term));
        s += '$';
        int n = s.length();
        // left most derivation
        stk.push(Symbol('S', SymbolType::NonTerm, true));

        for (int i = 0; i < n; ) {
            if (s[i] == stk.top().getVal() && stk.top().getType() == SymbolType::Term) {
                stk.pop();
                i++;
            } else if (stk.top().getType() == SymbolType::NonTerm) {
                auto element = parsing_table.find({stk.top(), Symbol(s[i], SymbolType::Term, false)});
                if (element != parsing_table.end()) {
                    stk.pop();
                    // pop 的时候 p 更新到这个儿子
                    // 由于这里是反着push的，所以某种意义上这也是一种先序遍历
                    for (auto it = element->second.second.rbegin(); it != element->second.second.rend(); it++) {
                        stk.push(*it);
                    }
                } else {
                    return false;
                }
            } else {
                // 其实这种情况不可能走到
                return false;
            }
        }
        return true;
    }

    bool parse(string s, Node*& root) {
        s += "$";
        int n = s.length();

        root = new Node(Symbol('S', NonTerm, true));
        Node* p = root;
        root->setBrother(new Node(Symbol('$', Term, false)));

        for (int i = 0; i < n; ) {
            if (s[i] == p->getSymbol().getVal() && p->getSymbol().getType() == Term) {
                while (p->getBrother() == nullptr) {
                    // 如果最终到S了呢？既没有 parent 也没有 brother 了
                    // 通过加哨兵 $ 解决
                    if (p->getParent()) {
                        p = p->getParent();
                    } else {
                        break;
                    }
                }
                p = p->getBrother();
                i++;
            } else if (p->getSymbol().getType() == NonTerm) {
                auto element = parsing_table.find({p->getSymbol(), Symbol(s[i], SymbolType::Term, false)});
                if (element != parsing_table.end()) {
                    for (auto it = element->second.second.rbegin(); it != element->second.second.rend(); it++) {
                        p->insertChild(new Node(*it));
                    }
                    p = p->getChild(0);
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
        return true;
    }
};

#endif