#include "Network.h"
#include "NodeInfoSerializer.hpp"
#include "RequestVoteMessageSerializer.hpp"
#include <memory>
#include "LogEntrySerializer.hpp"
#include <sys/socket.h>


void startListening(int port){
    while(1){
        
    }
}
std::unique_ptr<messageBase> Network::receiveMessage()
{
    messageType type;
    recv(sockfd, &type, sizeof(type), 0);

    switch (type)
    {
        case messageType::SEND_VOTE_RESPONSE:
        {
            int size;
            recv(sockfd, &size, sizeof(size), 0);

            std::vector<char> nodeBuffer(size);
            recv(sockfd, nodeBuffer.data(), size, 0);

            NodeInfo target = NodeInfoSerializer::deserialize(nodeBuffer);

            int voterID;
            int currentTerm;
            bool granted;

            recv(sockfd, &voterID, sizeof(voterID), 0);
            recv(sockfd, &currentTerm, sizeof(currentTerm), 0);
            recv(sockfd, &granted, sizeof(granted), 0);

            return std::make_unique<sendVoteResponseStruct>(
                target,
                voterID,
                currentTerm,
                granted
            );
        }

        case messageType::SEND_APPEND_ACK:
        {
            int size;
            recv(sockfd, &size, sizeof(size), 0);

            std::vector<char> nodeBuffer(size);
            recv(sockfd, nodeBuffer.data(), size, 0);

            NodeInfo target = NodeInfoSerializer::deserialize(nodeBuffer);

            int followerId;
            int currentTerm;
            int ack;
            bool granted;

            recv(sockfd, &followerId, sizeof(followerId), 0);
            recv(sockfd, &currentTerm, sizeof(currentTerm), 0);
            recv(sockfd, &ack, sizeof(ack), 0);
            recv(sockfd, &granted, sizeof(granted), 0);

            return std::make_unique<sendAppendAckStruct>(
                target,
                followerId,
                currentTerm,
                ack,
                granted
            );
        }

        case messageType::SEND_APPEND_ENTRIES:
        {
            int size;
            recv(sockfd, &size, sizeof(size), 0);

            std::vector<char> nodeBuffer(size);
            recv(sockfd, nodeBuffer.data(), size, 0);

            NodeInfo target = NodeInfoSerializer::deserialize(nodeBuffer);

            int leaderId;
            int currentTerm;
            int prefixLen;
            int prefixTerm;
            int commitLength;

            recv(sockfd, &leaderId, sizeof(leaderId), 0);
            recv(sockfd, &currentTerm, sizeof(currentTerm), 0);
            recv(sockfd, &prefixLen, sizeof(prefixLen), 0);
            recv(sockfd, &prefixTerm, sizeof(prefixTerm), 0);
            recv(sockfd, &commitLength, sizeof(commitLength), 0);

            int logCount;
            recv(sockfd, &logCount, sizeof(logCount), 0);

            std::vector<LogEntry> suffix;

            for (int i = 0; i < logCount; i++)
            {
                recv(sockfd, &size, sizeof(size), 0);

                std::vector<char> logBuffer(size);
                recv(sockfd, logBuffer.data(), size, 0);

                suffix.push_back(LogEntrySerializer::deserialize(logBuffer));
            }

            return std::make_unique<sendAppendEntriesStruct>(
                target,
                leaderId,
                currentTerm,
                prefixLen,
                prefixTerm,
                commitLength,
                suffix
            );
        }

        case messageType::SEND_CLIENT_COMMAND:
        {
            // Necessita de ClientCommandSerializer.
            break;
        }

        case messageType::SEND_REQUEST_VOTE:
        {
            int size;
            recv(sockfd, &size, sizeof(size), 0);

            std::vector<char> nodeBuffer(size);
            recv(sockfd, nodeBuffer.data(), size, 0);

            NodeInfo target = NodeInfoSerializer::deserialize(nodeBuffer);

            recv(sockfd, &size, sizeof(size), 0);

            std::vector<char> msgBuffer(size);
            recv(sockfd, msgBuffer.data(), size, 0);

            RequestVoteMessage msg =
                RequestVoteMessageSerializer::deserialize(msgBuffer);

            return std::make_unique<sendRequestVoteStruct>(
                target,
                msg
            );
        }
    }

    return nullptr;
}