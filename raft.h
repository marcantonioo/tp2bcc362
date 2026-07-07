#ifndef RAFT_H
#define RAFT_H

#include <vector>
#include <chrono>
#include "Role.h"
#include "Log.h"
#include "NodeInfo.hpp"
#include "RequestVoteMessage.hpp"
#include <unordered_set>
#include <unordered_map>
#include "Network.h"

class raft
{

private:
    NodeInfo node;
    int currentTerm;
    int votedFor;
    NodeInfo currentLeader;
    int commitLength;
    std::unordered_map<int, int> sentLength;
    std::unordered_map<int, int> ackedLength;
    std::unordered_set<int> votesReceived;
    std::chrono::steady_clock::time_point lastHeartBeat;
    std::chrono::milliseconds timeout;

    Log log;
    std::vector<NodeInfo> cluster;
    Network network;

    Role role;

public:
    raft(int id, int port, std::string addr);
    void recoverFromCrash();
    void leaderCrashed();

    void newElection();
    void collectVotes(int VoterID, int Term, bool granted);
    void broadcastElectionMessages(RequestVoteMessage msg, NodeInfo node);
    void receiveElectionMessage(RequestVoteMessage msg);

    void replicateLog(NodeInfo follower);
    void followerReceiveAppendEntries(NodeInfo Leader, int term, int prefixLen, int prefixTerm, int leaderCommit, std::vector<LogEntry> suffix);
    void followerAppend(int prefixLen, int leaderCommit, std::vector<LogEntry> suffix);
    
    void broadcastClientMessage(ClientCommand msg);

    void AppendEntries(int prefixLength, int leaderCommit, std::vector<Log> sufix);
    void logAcknowledgment(int followerID, int term, int ack, bool success);
    void commitLog();
};

#endif