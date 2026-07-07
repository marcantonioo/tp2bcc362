#include "raft.h"
#include "RequestVoteMessage.hpp"


struct messageBase{
    messageType msgtype;
    messageBase(messageType t):msgtype(t){}
};
class Network : messageBase{
public:
    void sendRequestVote(sendRequestVote msg);
    void sendVoteResponse(sendVoteResponse msg);
    void sendClientCommand(sendClientCommand msg);
    void sendAppendEntries(sendAppendEntries msg);
    void sendAppendAck(sendAppendAck msg);
    

    messageType receiveMessage();
    void startListening(int port);
};

struct sendRequestVote : messageBase{
    NodeInfo& target;
    RequestVoteMessage msg;
    sendRequestVote(NodeInfo& target, RequestVoteMessage msg):messageBase(messageType::SEND_REQUEST_VOTE), target(target), msg(msg){}
};



struct sendVoteResponse: messageBase{
    NodeInfo& target; 
    int voterID; 
    int currentTerm; 
    bool granted;
    sendVoteResponse(NodeInfo& target, int VoterID, int CurrentTerm, bool granted):messageBase(messageType::SEND_VOTE_RESPONSE), target(target), voterID(VoterID), currentTerm(CurrentTerm), granted(granted){}
};
struct sendClientCommand : messageBase{
    const NodeInfo& target;
    const ClientCommand& msg;

    sendClientCommand(const NodeInfo& target, const ClientCommand& msg)
        : messageBase(messageType::SEND_CLIENT_COMMAND),
          target(target),
          msg(msg)
    {}
};

struct sendAppendEntries : messageBase{
    const NodeInfo& target;
    int leaderId;
    int currentTerm;
    int prefixLen;
    int prefixTerm;
    int commitLength;
    std::vector<LogEntry> suffix;

    sendAppendEntries(const NodeInfo& target,
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

struct sendAppendAck : messageBase{
    NodeInfo target;
    int followerId;
    int currentTerm;
    int ack;
    bool granted;

    sendAppendAck(const NodeInfo& target,
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

enum struct messageType{
    SEND_REQUEST_VOTE,
    SEND_VOTE_RESPONSE,
    SEND_CLIENT_COMMAND,
    SEND_APPEND_ENTRIES,
    SEND_APPEND_ACK
};