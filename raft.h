#include <vector>
#include "Role.h"
#include "Log.h"
class raft{
private:
    int id;
    int currentTerm;
    int votedFor;
    int currentLeader;
    int commitLength;
    std::vector<int> sentLength;
    std::vector<int> ackedLength;
    std::vector<int> votesReceived;

    std::vector<Log> log;

    Role role;

public:
    raft (int id);
    void recoverFromCrash();
    void leaderCrashed();
    void newElection();
    void collectVotes(int VoteResponse, int VoterID, int Term, bool granted);
    void broadcastMessages(Message msg);
    void replicateLog(int prefixLength, int prefixTerm, std::vector<Log> sufix);
    void receiveMessage(Message msg, int term, int LeaderID, int prefixLength, int prefixTerm,  int leaderCommit, std::vector<Log> sufix);
    void AppendEntries(int prefixLength, int leaderCommit, std::vector<Log> sufix);
    void logAcknowledgment(Message LogResponse, int followerID, int term, bool success);
    void commitLog(int commitLength);
};
