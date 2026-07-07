#include "raft.h"

class Network{
public:
    void sendRequestVote(const NodeInfo& target);
    void sendVoteResponse(const NodeInfo& target, int voterID, int currentTerm, bool granted);
    void sendClientCommand(const NodeInfo& target, const ClientCommand& msg);
    void sendAppendEntries(const NodeInfo& target, int currentTerm, int prefixLen,
    int prefixTerm, int commitLength, std::vector<LogEntry> suffix);

    void startListening(int port){}
};