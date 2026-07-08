#ifndef NETWORK_H
#define NETWORK_H
#include "RequestVoteMessage.hpp"
#include "NodeInfo.hpp"
#include <memory>



enum struct messageType{
    SEND_REQUEST_VOTE,
    SEND_VOTE_RESPONSE,
    SEND_CLIENT_COMMAND,
    SEND_APPEND_ENTRIES,
    SEND_APPEND_ACK
};

struct messageBase{
    messageType msgtype;
    messageBase(messageType t):msgtype(t){}
};


struct sendRequestVoteStruct : messageBase{
    NodeInfo& target;
    RequestVoteMessage msg;
    sendRequestVoteStruct(NodeInfo& target, RequestVoteMessage msg):messageBase(messageType::SEND_REQUEST_VOTE), target(target), msg(msg){}
};



struct sendVoteResponseStruct: messageBase{
    NodeInfo& target; 
    int voterID; 
    int currentTerm; 
    bool granted;
    sendVoteResponseStruct(NodeInfo& target, int VoterID, int CurrentTerm, bool granted):messageBase(messageType::SEND_VOTE_RESPONSE), target(target), voterID(VoterID), currentTerm(CurrentTerm), granted(granted){}
};
struct sendClientCommandStruct : messageBase{
    NodeInfo target;
    ClientCommand& msg;

    sendClientCommandStruct(NodeInfo target, ClientCommand msg)
        : messageBase(messageType::SEND_CLIENT_COMMAND),
          target(target),
          msg(msg)
    {}
};

struct sendAppendEntriesStruct : messageBase{
    NodeInfo target;
    int leaderId;
    int currentTerm;
    int prefixLen;
    int prefixTerm;
    int commitLength;
    std::vector<LogEntry> suffix;

    sendAppendEntriesStruct(const NodeInfo& target,
                      int leaderId,
                      int currentTerm,
                      int prefixLen,
                      int prefixTerm,
                      int commitLength,
                      const std::vector<LogEntry>& suffix)
        : messageBase(messageType::SEND_APPEND_ENTRIES),
          target(target),
          leaderId(leaderId),
          currentTerm(currentTerm),
          prefixLen(prefixLen),
          prefixTerm(prefixTerm),
          commitLength(commitLength),
          suffix(suffix)
    {}
};

struct sendAppendAckStruct : messageBase{
    NodeInfo target;
    int followerId;
    int currentTerm;
    int ack;
    bool granted;

    sendAppendAckStruct(const NodeInfo& target,
                  int followerId,
                  int currentTerm,
                  int ack,
                  bool granted)
        : messageBase(messageType::SEND_APPEND_ACK),
          target(target),
          followerId(followerId),
          currentTerm(currentTerm),
          ack(ack),
          granted(granted)
    {}
};

class Network{
    int sockfd;
    int createConnection(const std::string& ip, int port);
public:
    void sendRequestVote(sendRequestVoteStruct msg);
    void sendVoteResponse(sendVoteResponseStruct msg);
    void sendClientCommand(sendClientCommandStruct msg);
    void sendAppendEntries(sendAppendEntriesStruct msg);
    void sendAppendAck(sendAppendAckStruct msg);
    
    std::unique_ptr<messageBase> receiveMessage();
    void startListening(int port);
};

#endif