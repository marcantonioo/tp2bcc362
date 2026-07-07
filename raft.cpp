#include "raft.h"

raft::raft(int id, int port, std::string addr) : node(id, port, addr), currentLeader(-1, -1, "")
{
    currentTerm = 0;
    votedFor = -1;
    commitLength = 0;
    sentLength = {};
    ackedLength = {};
    votesReceived = {};
    role = Role::FOLLOWER;
}

void raft::recoverFromCrash()
{ // falta colocar um random de timeout nessa funcao aqui
    role = Role::FOLLOWER;
    votesReceived = {};
    sentLength = {};
    ackedLength = {};
}
void raft::leaderCrashed()
{
    if (std::chrono::steady_clock::now() - lastHeartBeat > timeout)
    {
        currentTerm += 1;
        role = Role::CANDIDATE;
        votedFor = node.getid();
        int port;
        std::string addr;
        votesReceived.insert(node.getid());
        int lastTerm = 0;
        if (log.getEntries().size())
            lastTerm = log.getEntries()[log.getEntries().size() - 1].getTerm();
        RequestVoteMessage Voterequest(node.getid(), currentTerm, log.getEntries().size(), lastTerm);
        for (auto node : cluster)
        {
            network.sendRequestVote(node, Voterequest);
        }
        // colocar um random de timeout aqui
    }
    else
        return;
}
void raft::receiveElectionMessage(RequestVoteMessage msg)
{
    if (msg.getcTerm() > currentTerm)
    {
        currentTerm = msg.getcTerm();
        role = Role::FOLLOWER;
        votedFor = -1;
    }
    int lastTerm = 0;
    if (log.getEntries().size() > 0)
        lastTerm = log.getEntries()[log.getEntries().size() - 1].getTerm();
    bool logOK = (msg.getcLogTerm() > lastTerm) || (msg.getcLogTerm() == lastTerm && msg.getcLogLength() >= log.getEntries().size());
    if (msg.getcTerm() == currentTerm && logOK && (votedFor == -1 || votedFor == msg.getcID()))
    {
        votedFor = msg.getcID();
        network.sendVoteResponse(currentLeader, node.getid(), currentTerm, true);
    }
    else
        network.sendVoteResponse(currentLeader, node.getid(), currentTerm, false);
    return;
}

void raft::collectVotes(int VoterID, int Term, bool granted)
{
    if (role == Role::CANDIDATE && Term == currentTerm && granted)
        votesReceived.insert(VoterID);
    if (votesReceived.size() > ((cluster.size() + 1) / 2))
    {
        role = Role::LEADER;
        // dar um jeito de cancelar o election timer nessa linha em especifico
        for (auto follower : cluster)
        {
            sentLength[follower.getid()] = log.getEntries().size();
            ackedLength[follower.getid()] = 0;
            replicateLog(follower);
        }
    }
    else if (Term > currentTerm)
    {
        currentTerm = Term;
        role = Role::FOLLOWER;
        votedFor = -1;
        // dar um jeito de cancelar o election timer nessa linha em especifico
    }
}

void raft::broadcastClientMessage(ClientCommand msg)
{
    if (role == Role::LEADER)
    {
        log.append(LogEntry(msg, currentTerm));
        ackedLength[node.getid()] = log.getEntries().size();
        for (auto follower : cluster)
        {
            replicateLog(follower);
        }
    }
    else
    {
        network.sendClientCommand(currentLeader, msg);
    }
}

void raft::replicateLog(NodeInfo follower){
    int prefixLength = sentLength[follower.getid()];
    std::vector<LogEntry> entries(log.getEntries().begin() + sentLength[follower.getid()], log.getEntries().end());
    Log suffix(entries);
    int prefixTerm = 0;
    if (prefixLength > 0) prefixTerm = log.getEntries()[prefixLength-1].getTerm();
    network.sendAppendEntries(follower, node.getid(), currentTerm, prefixLength, prefixTerm, commitLength, suffix.getEntries());
}

void raft::followerReceiveAppendEntries(NodeInfo leader, int term, int prefixLen, int prefixTerm, int leaderCommit, std::vector<LogEntry> suffix){
    if (term > currentTerm) {
        currentTerm = term;
        votedFor = -1;
        //cancelar timeout
    }
    if (term == currentTerm){
        role = Role::FOLLOWER;
        currentLeader = leader;
    }
    bool logOK = (log.getEntries().size() >= prefixLen) && (prefixLen == 0 || log.getEntries()[prefixLen-1].getTerm() == prefixTerm);
    if (term == currentTerm && logOK){
        for (auto it : suffix){
            log.append(it);
        }
        int ack = prefixLen+suffix.size();
        //send LogResponse ,nodeID, CurrentTerm, ack, true
    }
    else
        //send LogResponse ,nodeID, CurrentTerm, 0, false
        return;
}