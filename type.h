#ifndef TYPE_H
#define TYPE_H

#include <iostream>

using namespace std;

enum SymbolType{
    NonTerm,
    Term
};

class Symbol {
    char val;
    SymbolType type; // 0 nonterm, 1 term
    bool start;

public:
    // Symbol() {
    //     val = '\0';
    //     type = SymbolType::Term;
    //     start = false;
    // }
    Symbol(char _val='\0', SymbolType _type=SymbolType::NonTerm, bool _start=false) {
        val = _val;
        type = _type;
        start = _start;
    }
    
    bool operator==(const Symbol& a) const {
        return type == a.type && val == a.val;
    }

    bool operator!=(const Symbol& a) const {
        return type != a.type || val != a.val;
    }

    bool isStart() const {
        return start;
    }

    char getVal() const {
        return val;
    }

    SymbolType getType() const {
        return type;
    }

    friend ostream& operator<<(std::ostream &s, const Symbol& symbol) {
        return s << symbol.getVal() << " ";
    }
};


template <>
struct std::hash<Symbol>
{
    std::size_t operator()(const Symbol& symbol) const noexcept {
        if (symbol.getType() == SymbolType::Term) {
            return hash<string>{}(to_string(symbol.getVal()) + "@term");
        } else {
            return hash<string>{}(to_string(symbol.getVal()) + "@nonterm");
        }
    }
};

template <>
struct std::hash<pair<Symbol, Symbol>>
{
    std::size_t operator()(const pair<Symbol, Symbol>& p) const noexcept {
        string concat;

        concat += to_string(p.first.getVal()) + "@" + ((p.first.getType() == SymbolType::Term) ? "Term" : "NonTerm") + "@";
        concat += to_string(p.second.getVal()) + "@" + ((p.second.getType() == SymbolType::Term) ? "Term" : "NonTerm");

        return hash<string>{}(concat);
    }
};
#endif