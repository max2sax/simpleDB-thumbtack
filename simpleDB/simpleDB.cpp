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
public:
	Command();
	Command(std::string com, std::string key, int v); 
	Command(const Command&); 
	~Command() {}; //nothing to delete};
	std::string getCommandName(){ return command; };
	std::string getKeyName() { return this->keyName; };
	int getValue() { return this->value; };
	void setCommandName(const std::string com) { command = com; };
	void setKeyName(const std::string key) { keyName = key; }; 
	void setValue(const int v) { value = v; };
	friend std::stringstream& operator>>(stringstream& is, Command& c) {
		//read in command info, and set invalid if cannot parse. 
		try {
			is >> c.command;
			if (is.eof()) { return is; }
			if (c.command == "NUMEQUALTO") {
				// read into value: 
				is >> c.value;
			}
			if (is.eof()) {
				return is;
			}
			//read next, which will be a variable name - either get, set or unset: 
			is >> c.keyName;

			if (is.eof()) {
				return is;
			}
			//read next, which will be the value (if we're setting a variable): 
			is >> c.value;
		}
		catch (exception e){
			c.command = "";
			c.keyName = "";
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

class DataBase {
private: 
	class Transaction{
	public:
		Transaction() {};
		Transaction(Command c) { commandStack.push(c); }
		Transaction(Transaction& other); 
		~Transaction() { /*empty the stack: */while (!commandStack.empty()) commandStack.pop(); };
		void addCommand(Command newCommand) { commandStack.push(newCommand); };
		void executeTransaction(DataBase& db); 
		friend class DataBase;
	private:
		std::stack<Command, std::deque<Command>> commandStack;
	};
public:
	DataBase(){ commitCommands = true;  };
	DataBase(DataBase& original) { commitCommands = original.commitCommands; };
	~DataBase(){ };
	std::string update_database(Command &c);
private:
	//class variables
	std::multimap<int, std::string> values; 
	std::map<std::string, int> keys; 
	bool commitCommands;
	std::deque<Transaction> transactions;
	//functions
	std::string remove_transaction();
	void add_transaction() { commitCommands = false;  this->transactions.push_front(Transaction()); };
	std::string run_command(Command& c);
};
Command::Command() {
	command = ""; keyName = ""; value = 0;
}
Command::Command(std::string com, std::string key, int v) {
	this->command = com; 
	this->keyName = key; 
	this->value = v; 
}
Command::Command(const Command& other) {
	this->command = other.command; 
	this->keyName = other.keyName; 
	this->value = other.value; 
}
void DataBase::Transaction::executeTransaction(DataBase& db)
{
	while (!commandStack.empty()) {
		auto& c = commandStack.top();
		db.run_command(c);
		commandStack.pop();
	}
}
DataBase::Transaction::Transaction(Transaction& other) {
	std::deque<Command> temp;
	while (!other.commandStack.empty()) {
		temp.push_front(other.commandStack.top());
		other.commandStack.pop(); 
	}
	for (auto c : temp) {
		other.commandStack.push(c); 
		this->commandStack.push(c); 
	}
}
std::string DataBase::update_database(Command& c)  {
	auto _addUndoCommand = [&](Command& com) {
		Command reverseCommand("", com.getKeyName(), 0);
		Command searchCommand("GET", com.getKeyName(), 0);
		std::string valStr = run_command(searchCommand);
		if (valStr == "NULL" && com.getCommandName() == "SET") {
			reverseCommand.setCommandName("UNSET");
		}
		else if (valStr == "NULL" && com.getCommandName() == "UNSET") {
		}
		else if (valStr != "NULL") {
			//this should be true for either set or unset - set to previous value
			reverseCommand.setCommandName("SET");
			reverseCommand.setValue(std::strtol(valStr.c_str(), nullptr, 10));
		}
		//add reverse command to transaction in order to undo it; 
		if (reverseCommand.getCommandName() != "") {
			transactions.front().addCommand(reverseCommand);
		}
	};
	if (c.getCommandName() == "BEGIN") {
		add_transaction();
	}
	else if (c.getCommandName() == "ROLLBACK") {
		//run the most recent transaction: 
		return remove_transaction();
	}
	else if (c.getCommandName() == "COMMIT") {
		//remove all transactions: 
		if (transactions.empty()) return "NO TRANSACTIONS"; 
		transactions.clear();
	}
	else if (c.getCommandName() == "END") {
		return "";
	}
	else if (c.getCommandName() == "SET" ) {
		if (commitCommands == false)
			_addUndoCommand(c); 
		return run_command(c); 
	}
	else if (c.getCommandName() == "UNSET") {
		if (commitCommands == false) {
			_addUndoCommand(c); 
		}
		return run_command(c); 
	}
	else if (c.getCommandName() == "GET" || c.getCommandName() == "NUMEQUALTO"){
		//either get or numequalto - run these immediately on the current state: 
		return run_command(c); 
	}
	return ""; //if it's not a valid command then don't do anything and return.
}
std::string DataBase::remove_transaction(){
	//removes the most recently added transaction without executing it: 
	if (transactions.empty()) {
		return "NO TRANSACTION";
	}
	transactions.front().executeTransaction(*this);
	transactions.pop_front(); 
	if (transactions.empty()) commitCommands = true;
	return "";
};
std::string DataBase::run_command(Command& c) {
	if (c.getCommandName() == "SET") {
		//put in the values map and the keys map
		auto alreadySet = keys.find(c.getKeyName());
		if (alreadySet != keys.end()) {
			//then this is a valid value and we can just update it:
			//first get the element with the current value:
			auto elements = values.equal_range(alreadySet->second);
			auto it = elements.first;
			for (it; it != elements.second; it++) {
				if (it->second == c.getKeyName()) break;
			}
			//insert the new value and remove the old.
			values.insert(std::pair<int, std::string>(c.getValue(), c.getKeyName()));
			alreadySet->second = c.getValue();
			values.erase(it);
			return "";
		}
		//we don't have this key already, so just insert them. It doesn't matter if the
		// value already exists since a value can have multiple keys.
		keys.insert(std::pair<std::string, int>(c.getKeyName(), c.getValue()));
		values.insert(std::pair<int, std::string>(c.getValue(), c.getKeyName()));
		return "";
	}
	else if (c.getCommandName() == "GET") {
		auto cur = keys.find(c.getKeyName());
		if (cur == keys.end()) {
			return "NULL";
		}
		return std::to_string(cur->second);
	}
	else if (c.getCommandName() == "UNSET") {
		auto currentKey = keys.find(c.getKeyName());
		if (currentKey != keys.end()) {
			//then this is a valid value and we can just remove it:
			//first get the value element with the current key:
			auto elements = values.equal_range(currentKey->second);
			auto it = elements.first;
			for (it; it != elements.second; it++) {
				if (it->second == c.getKeyName()) break;
			}
			//remove the keys
			keys.erase(currentKey);
			values.erase(it);
			return "";
		}
		//there's nothing to remove.
	}
	else if (c.getCommandName() == "NUMEQUALTO") {
		auto count = values.count(c.getValue());
		return std::to_string(count);
	}
	return "";
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
		if (updateResult != "") cout << "> " << updateResult << std::endl; 
	}
	return 0;
}
