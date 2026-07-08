#ifndef REQUESTVOTEMESSAGE_HPP
#define REQUESTVOTEMESSAGE_HPP
#include "Log.h"

class RequestVoteMessage{
private:
    int cID;
    int cTerm;
    int cLogLength;
    int cLogTerm;
public:
    int getcTerm(){return cTerm;}
    int getcLogLength(){return cLogLength;}
    int getcLogTerm(){return cLogTerm;}
    int getcID(){return cID;}
    RequestVoteMessage(int nodeID, int currentTerm, int logLength, int lastTerm){
        this->cID = nodeID; 
        this->cTerm = currentTerm;
        this->cLogLength = logLength;
        this->cTerm = lastTerm;
    }
};
#endif