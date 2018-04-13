#include <iostream>
#include <stack>
#include <deque>
#include <cctype>
using namespace std;
/*
	3+5*7+(1+2)*3+4
	3 5 7 1 2 3 4

	Pre: +++3*57*+1234
	Post: 357*+12+3*+4+
	O(N) complexity
*/

string preorder(string inorder);
string postorder(string inorder);
int priority(char c);

int main()
{
	string str;
	while (getline(cin, str)) {
		cout << "pre: " << preorder(str) << "\n";
		cout << "post: " << postorder(str) << "\n";
	}

	return 0;
}

string postorder(string inorder) {
	string result;
	stack<char> op;	
	for (int i = 0; i < inorder.size(); i++) {
		if (isdigit(inorder[i])) {
			result.push_back(inorder[i]);
		} else if (inorder[i] == '(') {
			// always push upon left braces
			op.push(inorder[i]);
		} else if (inorder[i] == ')') {
			while (op.top() != '(') {
				result.push_back(op.top());
				op.pop();
			}
			op.pop(); // pop braces
		} else {
			// stack: the higher the bigger priority
			while (!op.empty() && priority(inorder[i]) <= priority(op.top())) {
				result.push_back(op.top());
				op.pop();
			}
			op.push(inorder[i]);
		}
	}
	while (!op.empty()) {
		result.push_back(op.top());
		op.pop();
	}
	return result;
}

string preorder(string inorder) {
	deque<char> result;
	stack<char> op;
	for (int i = inorder.size()-1; i >= 0; i--) {
		if (isdigit(inorder[i])) {
			result.push_front(inorder[i]);
		} else if (inorder[i] == ')') {
			// always push upon right braces
			op.push(inorder[i]);
		} else if (inorder[i] == '(') {
			while (op.top() != ')') {
				result.push_front(op.top());
				op.pop();
			}
			op.pop(); // pop braces
		} else {
			// stack: the above should be no lesser than below
			while (!op.empty() && priority(inorder[i]) < priority(op.top())) {
				result.push_front(op.top());
				op.pop();
			}
			op.push(inorder[i]);
		}
	}
	while (!op.empty()) {
		result.push_front(op.top());
		op.pop();
	}
	string str;
	for (int i = 0; i < result.size(); i++) {
		str.push_back(result[i]);
	}
	return str;
}

int priority(char c) {
	switch (c) {
		case '+':
		case '-': return 1;
		case '*':
		case '/': return 2;
		case '(':
		case ')': return 0;
	}
}
