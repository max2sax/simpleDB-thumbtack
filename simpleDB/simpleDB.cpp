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
class Key;

class Value {
public: 
	Value(){ m_value = 0; };
	Value(Value& other){ 
		m_value = other.m_value; 
		for (auto&& key : other.references) {
			references.push_back(key); 
		}
	};
private: 
	int m_value; 
	std::vector<Key&> references; 
};
class Key {
private: 
	std::string m_key; 
	Value* m_value; 
public: 
	Key(){ m_key = "", m_value = 0; };
	Key(std::string keyName) : m_key(keyName) { };
	Key(std::string keyName, Value* v) : m_key(keyName), m_value(v) {};
	Key(Key& other){ m_key = other.m_key; m_value = new Value(*(other.m_value)); };
};
class DataBase {
private: 
	std::vector<Key&> keys; 
	Value* values; 
public: 
	DataBase(){ values = nullptr_t; };
	DataBase(DataBase& original) {};
	~DataBase(){}; 
};

class Command {
private:
	std::string command; 
	std::string variable; 
	int value; 
public:
	Command() { command = ""; variable = ""; value = 0; };
	Command(std::string &com) : command(com){};
	~Command() {}; //nothing to delete};
	friend std::stringstream& operator>>(stringstream& is, Command& c) {
		//read in command info, and set invalid if cannot parse. 
		try {
			is >> c.command;
			if (!is.eof()) {
				//read next, which will be a variable name: 
				is >> c.variable; 
			}
			if (!is.eof()) {
				//read next, which will be a variable name: 
				is >> c.value;
			}
		}
		catch (exception e){
			c.command = ""; 
			c.variable = ""; 
			c.value = 0; 
		}
		return is; 
	};
	friend std::ostream& operator<<(ostream& out, Command& c) {
		//read in command info, and set invalid if cannot parse. 
		try {
			out << c.command << " ";
			out << c.variable << " ";
			out << c.value << " ";
		}
		catch (exception e){
			out << "invalid command"; 
		}
		return out; 
	};
}; 

class Transaction{
private: 
	std::stack<Command> commandStack; 
public: 
	Transaction(); 
	Transaction(Transaction& other); 
	~Transaction() { /*empty the stack: */while (!commandStack.empty()) commandStack.pop(); };
	void addTransaction(Command& newCommand); 
	void executeTransaction(); 
};


int _tmain() {
	/* Enter your code here. Read input from STDIN. Print output to STDOUT */
	std::string input = ""; 
	std::deque<Transaction> transactionList;


	while (input.compare("END") != 0) {
		std::getline(cin, input); //get new input line
		cout << input << std::endl; //echo the input
		//parse the input stream for the command:
		std::stringstream instream(input); 
		Command c; 
		instream >> c;
		c.execute(); 
		//alkdjfadlkjasdsdfsdfsdfdfsfff
	}
	return 0;
}
