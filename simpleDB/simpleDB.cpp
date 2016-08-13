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
class Value; 

//this class, and it's associated iterator will make it easy to insert and find values, 
// without worrying about the underlying datastructure - in this case I'm going to use a binary tree: 
class ValueTree {
public: 
	class ValueTreeIterator {
	public: 
		ValueTreeIterator(){}; 
		Value* operator*() { return m_value; }; 
		ValueTreeIterator operator++(){}; 
	private: 
		Value* m_value; 
		ValueTreeIterator* nextValue; 
		ValueTreeIterator* prevValue; 
	};
public: 
	ValueTree(); //basic constructor: 
	ValueTreeIterator insertValue(Value& val){}; 
	ValueTreeIterator findValue(Value& val) {}; 
private:
	//I want to keep a well balanced tree, so I need to keep the min and max values. 
	ValueTreeIterator headValue; 
	ValueTreeIterator minValue;
	ValueTreeIterator maxValue; 
};
class Value {
public:
	Value(Key& referee, int val) : m_value(val) { value_valid = true; references.push_back(referee); };
	Value(Value& other): m_value(other.m_value){ 
		value_valid = other.value_valid; 
		for (auto&& key : other.references) {
			references.push_back(key); 
		}
	};
	bool has_references() {
		return !(references.empty());
	}; 
	void add_reference(Key& referee) { references.push_back(referee); };
private: 
	bool value_valid; 
	const int m_value; 
	std::vector<Key&> references; 
};
class Key {
public: 
	Key(){ m_key = "", m_value = 0; };
	Key(std::string keyName) : m_key(keyName) { };
	Key(std::string keyName, Value* v) : m_key(keyName), m_value(v) {};
	Key(Key& other){ m_key = other.m_key; m_value = new Value(*(other.m_value)); };
private:
	std::string m_key;
	Value* m_value;
};

class DataBase {
public:
	DataBase(){ commitCommands = true;  values = nullptr_t; };
	DataBase(DataBase& original) { commitCommands = original.commitCommands; };
	~DataBase(){ keys.clear(); delete values; };

	std::string update_database(Command& c) {
		//
		if (c.getCommandName() == "BEGIN") {
			add_transaction();
		}
		else if (c.getCommandName() == "ROLLBACK") {
			return remove_transaction();
		}
		else if (c.getCommandName() == "COMMIT") {
			commitCommands = true; 
			for (auto&& trans : transactions) {
				trans.executeTransaction(*this);
			}
		}
		else if (c.getCommandName() == "END") {
			return "";
		}
		else {
			if (commitCommands) {
				//execute now and return; 
				return run_command(c); 
			}
			transactions.front().addCommand(c);
		}
		return "";
	};
private:
	class Transaction{
	public:
		Transaction();
		Transaction(Command& c) { commandStack.push(c); }
		Transaction(Transaction& other);
		~Transaction() { /*empty the stack: */while (!commandStack.empty()) commandStack.pop(); };
		void addCommand(Command& newCommand);
		void executeTransaction(DataBase& db) {
			while (!commandStack.empty()) {
				auto& c = commandStack.top();
				commandStack.pop();
				db.run_command(c);
			}
		};
		friend class DataBase; 
	private:
		std::stack<Command> commandStack;
	};
	
	bool commitCommands;
	std::vector<Key&> keys;
	std::deque<Transaction&> transactions;
	Value* values;
	std::string remove_transaction() {
		//removes the most recently added transaction without executing it: 
		if (transactions.empty()) {
			return "NO TRANSACTION";
		}
		transactions.pop_front();
		if (transactions.empty()) commitCommands = true; 
		return "";
	};
	void add_transaction() { commitCommands = false;  this->transactions.push_front(Transaction()); };
	std::string run_command(Command& c){
		if (c.getCommandName() == "SET") {

		}
		else if (c.getCommandName() == "GET") {

		}
		else if (c.getCommandName() == "UNSET") {

		}
		else if (c.getCommandName() == "NUMEQUALTO") {

		}
		return ""; 
	};
};

class Command {
public:
	Command()  { command = ""; keyName = ""; value = 0; };
	Command(std::string &com) : command(com){};
	~Command() {}; //nothing to delete};
	std::string getCommandName(){
		return command;
	};
	friend std::stringstream& operator>>(stringstream& is, Command& c) {
		//read in command info, and set invalid if cannot parse. 
		try {
			is >> c.command;
			if (!is.eof()) {
				//read next, which will be a variable name: 
				is >> c.variable; 
			}
			if (!is.eof()) {
				//read next, which will be the value (if we're setting a variable): 
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
		//write command info to output stream
		out << c.command << " ";
		out << c.keyName << " ";
		out << c.value << " ";

		return out;
	};
private:
	std::string command;
	std::string keyName;
	int value;
};



int _tmain() {
	/* Enter your code here. Read input from STDIN. Print output to STDOUT */
	std::string input = ""; 

	DataBase masterDatabase; 
	while (input.compare("END") != 0) {
		std::getline(cin, input); //get new input line
		cout << input << std::endl; //echo the input
		//parse the input stream for the command:
		std::stringstream instream(input); 
		Command c; 
		instream >> c;
		auto updateResult = masterDatabase.update_database(c); 
		cout << updateResult << std::endl; 
	}
	return 0;
}
