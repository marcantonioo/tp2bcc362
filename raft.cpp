#include "raft.h"

 raft::raft(int id){
    this->id = id;
    currentTerm = 0;
    votedFor = -1;
    currentLeader = -1;
    commitLength = 0;
    sentLength = {};
    ackedLength = {};
    votesReceived = {};
    role = Role::FOLLOWER;
 }

 void raft::recoverFromCrash(){
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
        votedFor = id;
        votesReceived.push_back(id);
        int lastTerm = 0;
        if (log.size()) lastTerm = log[log.size()-1].getTerm();
        RequestVoteMessage propinaDeVoto(id, currentTerm, log.size(), lastTerm);
        for (auto node : cluster){
            broadcastElectionMessages(propinaDeVoto, node);
        }
    }
    else return;
 }