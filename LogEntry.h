class LogEntry{
    int term;
    ClientCommand operation;
public:
    LogEntry(ClientCommand Operation, int Term):  operation(Operation), term(Term){}
    int getTerm(){return term;}
};

class ClientCommand {
private:
    int clientID;
    Operation op;
    std::string key;
    std::string value;

public:
    ClientCommand(int clientID,Operation op,const std::string& key,const std::string& value): clientID(clientID), op(op), key(key),value(value){}

    int getClientID() const { return clientID; }
    Operation getOperation() const { return op; }
    const std::string& getKey() const { return key; }
    const std::string& getValue() const { return value; }
};

enum class Operation{
    WRITE
};
