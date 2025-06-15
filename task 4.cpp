#include <iostream>
#include <stack>
#include <string>
#include <cctype>
#include <cmath>

using namespace std;

// Check if character is an operator
bool isOperator(char ch) {
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '^';
}

// Convert postfix to infix
string postfixToInfix(const string& postfix) {
    stack<string> stk;

    for (size_t i = 0; i < postfix.length(); ) {
        if (isdigit(postfix[i])) {
            string number;
            while (i < postfix.length() && isdigit(postfix[i])) {
                number += postfix[i];
                i++;
            }
            stk.push(number);
        }
        else if (postfix[i] == ' ') {
            i++;
        }
        else if (isOperator(postfix[i])) {
            if (stk.size() < 2)
                throw runtime_error("Invalid postfix expression: not enough operands.");

            string b = stk.top(); stk.pop();
            string a = stk.top(); stk.pop();

            string expr = "(" + a + " " + postfix[i] + " " + b + ")";
            stk.push(expr);
            i++;
        }
        else {
            throw runtime_error("Invalid character in postfix expression.");
        }
    }

    if (stk.size() != 1)
        throw runtime_error("Invalid postfix expression.");

    return stk.top();
}

// Evaluate postfix expression
int evaluatePostfix(const string& postfix) {
    stack<int> stk;

    for (size_t i = 0; i < postfix.length(); ) {
        if (isdigit(postfix[i])) {
            int num = 0;
            while (i < postfix.length() && isdigit(postfix[i])) {
                num = num * 10 + (postfix[i] - '0');
                i++;
            }
            stk.push(num);
        }
        else if (postfix[i] == ' ') {
            i++;
        }
        else if (isOperator(postfix[i])) {
            if (stk.size() < 2)
                throw runtime_error("Invalid postfix expression: insufficient operands.");

            int b = stk.top(); stk.pop();
            int a = stk.top(); stk.pop();

            switch (postfix[i]) {
                case '+': stk.push(a + b); break;
                case '-': stk.push(a - b); break;
                case '*': stk.push(a * b); break;
                case '/':
                    if (b == 0) throw runtime_error("Division by zero!");
                    stk.push(a / b);
                    break;
                case '%':
                    if (b == 0) throw runtime_error("Modulo by zero!");
                    stk.push(a % b);
                    break;
                case '^': stk.push(pow(a, b)); break;
            }
            i++;
        }
        else {
            throw runtime_error("Invalid character in postfix expression.");
        }
    }

    if (stk.size() != 1)
        throw runtime_error("Invalid postfix expression.");

    return stk.top();
}

int main() {
    string postfix;

    cout << "Enter a postfix expression (with space-separated tokens): ";
    getline(cin, postfix);

    try {
        string infix = postfixToInfix(postfix);
        cout << "\nConverted Infix Expression: " << infix << endl;

        int result = evaluatePostfix(postfix);
        cout << "Evaluated Result: " << result << endl;
    }
    catch (runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}
