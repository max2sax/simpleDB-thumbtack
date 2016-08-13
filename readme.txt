simpleDB assumptions: 
during a transaction - the database is in an undefined state. 
  It's not specified, so I assume all non-transaction commands can be a part of transaction. So, if a command that returns a value is in a transaction, the value will not be returned until the transaction is committed. 

In MY database, all transactions are stored until committed. However, any transaction command is run immediately
