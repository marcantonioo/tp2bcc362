class LogEntry{
    int timestamp;
    int clientID;
    Operation operation;
public:
    LogEntry(int timestamp, int clientID, Operation operation);
};

enum class Operation{
    WRITE
};
