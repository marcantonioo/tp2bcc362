class Message{
    int timestamp;
    int clientID;
    Operation operation;
public:
    Message(int timestamp, int clientID, Operation operation);
};

enum class Operation{
    WRITE
};