#include "raft.h"

raft::raft(int id, int port, std::string addr):node(id, port, addr){
    currentTerm = 0;
    votedFor = -1;
    currentLeader = -1;
    commitLength = 0;
    sentLength = {};
    ackedLength = {};
    votesReceived = {};
    role = Role::FOLLOWER;
 }

void raft::recoverFromCrash(){ //falta colocar um random de timeout nessa funcao aqui
    role = Role::FOLLOWER;
    currentLeader = -1;
    votesReceived = {};
    sentLength = {};
    ackedLength = {};
 }
void raft::leaderCrashed(){
    if (std::chrono::steady_clock::now() - lastHeartBeat > timeout){
        currentTerm+=1;
        role = Role::CANDIDATE;
        votedFor = node.getid();
        int port;
        std::string addr;
        votesReceived.insert(node.getid());
        int lastTerm = 0;
        if (log.getEntries().size()) lastTerm = log.getEntries()[log.getEntries().size()-1].getTerm();
        RequestVoteMessage propinaDeVoto(node.getid(), currentTerm, log.getEntries().size(), lastTerm);
        for (auto node : cluster){
            broadcastElectionMessages(propinaDeVoto, node);
        }
        //colocar um random de timeout aqui
    }
    else return; 
}
void raft::receiveElectionMessage(RequestVoteMessage msg){
    if (msg.getcTerm() > currentTerm){
    currentTerm = msg.getcTerm();
    role = Role::FOLLOWER;
    votedFor = -1;
    }
    int lastTerm = 0;
    if (log.getEntries().size() > 0) lastTerm = log.getEntries()[log.getEntries().size()-1].getTerm();
    bool logOK = (msg.getcLogTerm() > lastTerm) || (msg.getcLogTerm() == lastTerm && msg.getcLogLength() >= log.getEntries().size());
    if (msg.getcTerm() == currentTerm && logOK && (votedFor == -1 ||votedFor == msg.getcID())) {
        votedFor = msg.getcID();
        collectVotes(node.getid(), currentTerm, true);
    }
    else
        collectVotes(node.getid(), currentTerm, false);
    return;
}

void raft::collectVotes(int VoterID, int Term, bool granted){
    if (role == Role::CANDIDATE && Term == currentTerm && granted) votesReceived.insert(VoterID);
    if (votesReceived.size() > ((cluster.size()+1)/2)){
        role = Role::LEADER;
       //dar um jeito de cancelar o election timer nessa linha em especifico
        for (auto node : cluster){
            sentLength[node.getid()] = log.getEntries().size();
            ackedLength[node.getid()] = 0;
            std::vector<LogEntry> entries(log.getEntries().begin() + sentLength[node.getid()], log.getEntries().end());
            Log suffix(entries);
            replicateLog(sentLength[node.getid()], suffix);
        } 
    }
    else if (Term > currentTerm){
        currentTerm = Term;
        role = Role::FOLLOWER;
        votedFor = {};
       //dar um jeito de cancelar o election timer nessa linha em especifico
    }
}

void raft::broadcastClientMessage(ClientCommand msg){
    if (role == Role::LEADER){
        log.append(LogEntry(msg, currentTerm));
        ackedLength[node.getid()] = log.getEntries().size();
        for (auto node : cluster){
            std::vector<LogEntry> entries(log.getEntries().begin() + sentLength[node.getid()], log.getEntries().end());
            Log suffix(entries);
            replicateLog(sentLength[node.getid()], suffix);
        }
    }
    No
    else{
        for (const auto& node : cluster)
            if (node.role == Role)
            network.sendClientCommand();
    }
}
