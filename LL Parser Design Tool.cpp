#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <cctype>
#define MATCH(x, y) { if (x != y) { cerr << "error: wrong format\n"; exit(1); }}
using namespace std;
/*
	S->aAB
	A->ABc
	A->Bb
	B->cA
	B->D
	D->-

	1. Transform left-recursion to right recursion
	2. Detect nullable var
	3. Construct First Set, Follow Set and Selection Set
	4. Collision detection
*/
class Variable {
public:
	char name;
	int nullable;
	set<char> first_set;
	set<char> follow_set;
	Variable(char _name) : name{_name}, nullable{-1} {}
	Variable() : name{'\0'} {}
	bool is_nullable();
	void first_set_add(char c) { first_set.insert(c); }
	void first_set_add(const set<char>& s);
	void follow_set_add(char c) { follow_set.insert(c); }
	void follow_set_add(const set<char>& s);
};

class ProductionRule {
public:
	char lefthand;
	vector<char> righthand;
	int nullable;
	set<char> selection_set;
	ProductionRule(char _lefthand, string _righthand) : nullable{-1} {
		lefthand = _lefthand;
		for (auto c : _righthand) righthand.push_back(c);
	}
	bool is_nullable();
	void selection_set_add(char c) { selection_set.insert(c); }
};

unsigned int i, j, k;
set<char> terminal;
map<char, Variable> variable;
vector<ProductionRule> grammar;

bool is_terminal(char x) { return terminal.count(x); }
bool is_variable(char x) { return variable.count(x); }
void add_variable(char v) { variable[v] = Variable(v); }
bool contains(vector<char>& arr, char c);
void transform_from_left_recursion(int i);
char new_variable();
set<char>& first_set(char x);
set<char>& constuct_first_set(char var, set<char>& constucted);
set<char>& constuct_follow_set(char var, set<char>& constucted);

int main()
{
	string line;
	while (getline(cin, line)){
		// format: S->ABcdE
		MATCH(line.substr(1, 2), "->");
		grammar.emplace_back(line[0], line.substr(3));
	}
	for (i = 0; i < grammar.size(); i++) {
		add_variable(grammar[i].lefthand);
		for (auto x : grammar[i].righthand) {
			if (islower(x))
				terminal.insert(x);
			else if (x != '-')
				add_variable(x);
		}
		if (contains(grammar[i].righthand, '-') && grammar[i].righthand.size() != 1) {
			cerr << "error: righthand having not only a null character\n";
			exit(1);
		}
	}
	cout << "checking for left recursion...\n";
	bool is_left_recursion = false;
	for (i = 0; i < grammar.size(); i++) {
		if (grammar[i].lefthand == grammar[i].righthand[0]) {
			if (!is_left_recursion) {
				cout << "left recursion detected.\ntransforming to right recursion...\n";
			}
			is_left_recursion = true;
			transform_from_left_recursion(i);
		}
	}
	if (is_left_recursion) { // print out new grammar
		for (i = 0; i < grammar.size(); i++) {
			cout << grammar[i].lefthand << "->";
			for (auto x : grammar[i].righthand) cout << x;
			cout << "\n";
		}
	}
	cout << "detecting null variable...\n";
	for (auto v : variable) { // top-down DP
		cout << v.first << ": " << boolalpha << v.second.is_nullable() << "\n";
	}
	cout << "constructing first set...\n";
	/*	1. term is term
		2. S->aA, S contains a
		3. S->ABC, S contains first(A)... (until first non-null)
		4. S->-, S contains null */
	for (i = 0; i < grammar.size(); i++) {
		char left = grammar[i].lefthand;
		if (grammar[i].righthand[0] == '-') { // rule 4
			variable[left].first_set_add('-');
		} else if (is_terminal(grammar[i].righthand[0])) { // rule 2
			variable[left].first_set_add(grammar[i].righthand[0]);
		}
	}
	set<char> constucted;
	for (auto v : variable) { // rule 3, top-down DP
		constuct_first_set(v.first, constucted);
		cout << v.first << ": ";
		for (auto s : v.second.first_set) cout << s;
		cout << "\n";
	}
	cout << "constructing follow set...\n";
	/*	1. S contains EOF
		2. X->ABC, A contains first(B)... (until first non-null) (A is var)
		3. X->ABCD, ...D contains follow(X)  (until first non-null) (D is var)
	*/
	variable['S'].follow_set_add('\0'); // rule 1
	for (i = 0; i < grammar.size(); i++) { // rule 2
		vector<char>& right = grammar[i].righthand;
		for (j = 0; j < right.size() - 1; j++) {
			if (is_variable(right[j])) {
				for (k = j + 1; k < right.size(); k++) { // add until first non-null
					if (is_terminal(right[k]))
						variable[right[j]].follow_set_add(right[k]);
					else
						variable[right[j]].follow_set_add(variable[right[k]].first_set);
					if (is_terminal(right[k]) || !variable[right[k]].is_nullable())
						break;
				}
			}
		}
	}
	constucted.clear();
	for (auto v : variable) { // rule 3, top-down DP
		constuct_follow_set(v.first, constucted);
		cout << v.first << ": ";
		for (auto s : v.second.follow_set) cout << s;
		cout << "\n";
	}

	// not yet verify first & follow set

	return 0;
}

bool Variable::is_nullable() {
	// if one of the grammar rule is nullable
	bool found = false;
	if (this->nullable != -1) { // -1 means unset
		return this->nullable;
	} else {
		for (i = 0; i < grammar.size(); i++) {
			if (grammar[i].lefthand == name) {
				found = true;
				if (grammar[i].is_nullable()) this->nullable = 1;
			}
		}
		if (!found) {
			cerr << "error: variable " << name << " undefined\n";
			exit(1);
		}
		if (this->nullable == -1) this->nullable = 0;
		return this->nullable;
	}
}

void Variable::first_set_add(const set<char>& s) {
	for (auto c : s) {
		this->first_set_add(c);
	}
}

void Variable::follow_set_add(const set<char>& s) {
	for (auto c : s) {
		this->follow_set_add(c);
	}
}

bool ProductionRule::is_nullable() {
	// if all righthand are nullable
	if (this->nullable != -1) { // -1 means unset
		return this->nullable;
	} else if (righthand[0] == '-') {
		this->nullable = 1;
		return this->nullable;
	} else {
		for (auto x : righthand) {
			if (is_terminal(x))
				this->nullable = 0;
			else if (!variable[x].is_nullable())
				this->nullable = 0;
		}
		if (this->nullable == -1) this->nullable = 1;
		return this->nullable;
	}
}

bool contains(vector<char>& arr, char c) {
	for (auto a : arr)
		if (c == a) return true;
	return false;
}

void transform_from_left_recursion(int idx) {
/*	S -> S...
	S -> A... (assume the remaindings don't contain S)
	S -> B...

	S -> A...S'
	S -> B...S'
	S'-> ...S'
	S'-> null */
	char s = grammar[idx].lefthand;
	char s2 = new_variable();
	grammar[idx].lefthand = s2;
	grammar[idx].righthand.erase(grammar[idx].righthand.begin());
	grammar[idx].righthand.push_back(s2);
	grammar.emplace_back(s2, "-");
	for (j = 0; j < grammar.size(); j++) {
		if (grammar[j].lefthand == s) grammar[j].righthand.push_back(s2);
	}
}

char new_variable() {
	for (char c = 'A'; c <= 'Z'; c++) {
		if (!is_variable(c)) return c;
	}
	cerr << "error: out of variable character set\n";
	exit(1);
}

set<char>& constuct_first_set(char var, set<char>& constucted) {
	// Rule:  X->ABC, X contains first(A)... (until first non-null)
	if (constucted.count(var)) {
		return variable[var].first_set;
	} else {
		for (i = 0; i < grammar.size(); i++) {
			if (grammar[i].lefthand == var && is_variable(grammar[i].righthand[0])) {
				vector<char>& right = grammar[i].righthand;		
				for (j = 0; j < right.size(); j++) { // add until first non-null
					variable[var].first_set_add(constuct_first_set(right[j], constucted));
					if (is_terminal(right[j]) || !variable[right[j]].is_nullable()) break;
				}
			}
		}
		constucted.insert(var);
		return variable[var].first_set;
	}
}

set<char>& constuct_follow_set(char var, set<char>& constucted) {
	// Rule:  X->ABCD, ...D contains follow(X)  (until first non-null) (D is var)
	if (constucted.count(var)) {
		return variable[var].follow_set;
	} else {
		for (i = 0; i < grammar.size(); i++) {
			char left = grammar[i].lefthand;
			vector<char>& right = grammar[i].righthand;
			for (int j = right.size()-1; j >= 0; j--) { // test until first non-null
				if (right[j] == var) {
					variable[var].follow_set_add(constuct_follow_set(left, constucted));
				}
				if (is_terminal(right[j]) || !variable[right[j]].is_nullable()) break;
			}
		}
		constucted.insert(var);
		return variable[var].follow_set;
	}
}
