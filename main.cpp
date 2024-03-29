#include <vector>
#include <string>

#include "LL1.h"
#include "type.h"

using namespace std;

int main() {
    vector<pair<string, string>> grams({
        {"S", "(S+F)"}, {"S", "F"}, {"F", "1"}
    });
    auto parser = Parser(grams);
    parser.constructParseTable();

    Node* root = nullptr;
    cout << parser.parse("((1+1)+1)", root) << endl;
    root->visitTreePreOrder();
    root->visitLevelOrder();
    cout << parser.parse("(1+(1+1)+1)") << endl;
    cout << parser.parse("(+1)") << endl;
    cout << parser.parse("()") << endl;
    cout << parser.parse("1") << endl;
    cout << parser.parse("1+") << endl;
    cout << parser.parse("(1+1))))()") << endl;
    return 0;
}