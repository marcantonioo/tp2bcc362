#include "Log.h"

class RequestVoteMessage{
private:
    int nodeID;
    int currentTerm;
    int logLength;
    int lastTerm;
public:
    int getcurrentTerm(){return currentTerm;}
    int getlogLength(){return logLength;}
    int getlastTerm(){return lastTerm;}
    int getnodeID(){return nodeID;}
    RequestVoteMessage(int nodeID, int currentTerm, int logLength, int lastTerm){
        this->nodeID = nodeID;
        this->currentTerm = currentTerm;
        this->logLength = logLength;
        this->lastTerm = lastTerm;
    }
};