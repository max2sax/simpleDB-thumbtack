// simpleDB.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <bitset>
#include <cstdio>
#include <limits>
#include <vector>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <numeric>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>

using namespace std;
class Command {
private:
public:
}; 
class Transaction{
private: 
	std::stack<Command> commandStack; 
public: 
	Transaction(); 
	Transaction(Transaction& other); 
	~Transaction() { /*empty the stack: */while (!commandStack.empty()) commandStack.pop(); };
};

int _tmain() {
	/* Enter your code here. Read input from STDIN. Print output to STDOUT */
	return 0;
}
