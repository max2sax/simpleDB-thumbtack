simpleDB assumptions: 
during a transaction - the database is in an undefined state. 
  It's not specified, so I assume all non-transaction commands can be a part of transaction. So, if a command that returns a value is in a transaction, the value will not be returned until the transaction is committed. 

In MY database, all transactions are stored until committed. However, any transaction command is run immediately
At this point, only commands executed outside of a transaction will display any output. 
Output is returned from the commands internally in the database class, but the only 
way to send output to the console is from main. 
    - This lends insight to a feature update in the future. Pass in an output stream to the Database. 
    - I don't want the Database to own writing to any streams on it's own. But it could write to a user
    provided stream. This makes it possible to implement logging separately, as an output stream class. 