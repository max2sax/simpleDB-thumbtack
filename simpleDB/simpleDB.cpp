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
#include <unordered_set>
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
		Transaction(const Transaction& other); 
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
	std::map<int, std::unordered_set<std::string>> values; 
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
DataBase::Transaction::Transaction(const Transaction& other) {
	this->commandStack = other.commandStack; 
}
std::string DataBase::update_database(Command& c)  {
	auto _getUndoCommand = [&](Command& com) {
		Command reverseCommand("", com.getKeyName(), 0);
		Command searchCommand("GET", com.getKeyName(), 0); 
		std::string valStr = run_command(searchCommand);// O(log(n)) for search
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
		return reverseCommand; 
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
		if (commitCommands == false) {
			auto reverseCommand = _getUndoCommand(c);
			if (reverseCommand.getCommandName() != "")
				transactions.front().addCommand(reverseCommand); 
		}
		return run_command(c); 
	}
	else if (c.getCommandName() == "UNSET") {
		if (commitCommands == false) {
			auto reverseCommand = _getUndoCommand(c); 
			if (reverseCommand.getCommandName() != "")
				transactions.front().addCommand(reverseCommand);
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
	//executes the most recently added transaction: 
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
		auto currentKeyValue = keys.find(c.getKeyName());
		auto newValueIt = values.find(c.getValue()); 

		if (currentKeyValue != keys.end()) {
			//then this is a valid value and we can just update it:
			//first get the element with the current value:
			auto val = values.find(currentKeyValue->second); //O(log(n)) where n = number values in DB
			auto& elements = val->second;
			auto oldLocation = std::find(elements.begin(), elements.end(), c.getKeyName()); //O(1) for a set

			//update the value of the current key
			currentKeyValue->second = c.getValue();

			//remove the key from the current value
			elements.erase(oldLocation);
			
			if (val->second.size() == 0) values.erase(val); 
		}
		else {
			//we don't have this key already, so just insert it
			keys.insert(std::pair<std::string, int>(c.getKeyName(), c.getValue()));
		}
		
		if (newValueIt != values.end()) {
			//If the value already exists just push the key to this value
			newValueIt->second.insert(c.getKeyName());
			return ""; 
		}
		//remove the old value: 
		
		//insert a new value for this key: 
		std::unordered_set<std::string> valKeys; 
		valKeys.insert(c.getKeyName()); 
		values.insert(std::pair<int, std::unordered_set<std::string>>(c.getValue(),valKeys));
		return "";
	}
	else if (c.getCommandName() == "GET") {
		auto currentValIt = keys.find(c.getKeyName());
		if (currentValIt == keys.end()) {
			return "NULL";
		}
		return std::to_string(currentValIt->second);
	}
	else if (c.getCommandName() == "UNSET") {
		auto currentKey = keys.find(c.getKeyName());
		if (currentKey != keys.end()) {
			//then this is a valid value and we can just remove it:
			//first get the value element with the current key:
			auto val = values.find(currentKey->second); //O(log(n)) where n = number values in DB
			auto& elements = val->second; 
			auto it = std::find(elements.begin(), elements.end(), c.getKeyName()); //O(1) for a set
			
			//remove the keys
			keys.erase(currentKey);
			elements.erase(it); 
			if (elements.size() == 0) values.erase(val);
			return "";
		}
		//there's nothing to remove.
	}
	else if (c.getCommandName() == "NUMEQUALTO") {
		auto val = values.find(c.getValue()); 
		if (val == values.end()) return "0"; 
		auto count = val->second.size();
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
