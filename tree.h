#ifndef TREE_H
#define TREE_H

#include <vector>
#include <queue>
#include <stack>
#include "type.h"

using namespace std;


class Node {
    vector<Node*> children;
    Node* parent;
    Node* brother;

    Symbol symbol;
public:
    Node(Symbol _symbol) {
        symbol = _symbol;
        brother = nullptr;
        parent = nullptr;
    }
    ~Node() {
        for (auto c : children) {
            delete c;
        }
        delete brother;
        delete parent;
    }
    Node* getParent() const {
        return parent;
    }
    Symbol getSymbol() const {
        return symbol;
    }
    Node* getChild(int index) const {
        return children[index];
    }
    Node* getBrother() const {
        return brother;
    }
    void setSymbol(Symbol s) {
        symbol = s;
    }
    void setBrother(Node* bro) {
        brother = bro;
    }
    void setParent(Node* pa) {
        parent = pa;
    }
    void insertChild(Node* n) {
        if (children.empty()) {
            n->setBrother(nullptr);
        } else {
            n->setBrother(*children.begin());
        }
        n->setParent(this);
        children.insert(children.begin(), n);
    }

    void visitTreePreOrder() {
        stack<Node*> stk;
        stk.push(this);

        Node* p = this;

        while (!stk.empty()) {
            p = stk.top();
            cout << *p;
            stk.pop();
            for (auto it = p->children.rbegin(); it != p->children.rend(); it++) {
                stk.push(*it);
            }
        }
        cout << endl;
        cout << endl;
    }

    void visitLevelOrder() {
        queue<Node*> q;

        q.push(this);
        Node* p;
        while (!q.empty()) {
            int n = q.size();
            while (n--) {
                p = q.front();
                cout << *p;
                q.pop();
                for (auto child : p->children) {
                    q.push(child);
                }
            }
            cout << endl;
        }
        cout << endl;
        cout << endl;
    }
    friend ostream& operator<<(std::ostream &s, const Node& node) {
        return s << node.symbol << " ";
    }
};

#endif