#include "Network.h"
#include "NodeInfoSerializer.hpp"
#include "RequestVoteMessageSerializer.hpp"
#include <memory>
#include "LogEntrySerializer.hpp"
#include <sys/socket.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "ClientCommandSerializer.h"


void Network::startListening(int port)
{
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if (listenfd < 0)
    {
        perror("socket");
        return;
    }

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(listenfd,
             (sockaddr*)&serverAddr,
             sizeof(serverAddr)) < 0)
    {
        perror("bind");
        close(listenfd);
        return;
    }

    if (listen(listenfd, 5) < 0)
    {
        perror("listen");
        close(listenfd);
        return;
    }

    std::cout << "Escutando na porta " << port << std::endl;

    while (true)
    {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);

        sockfd = accept(
            listenfd,
            (sockaddr*)&clientAddr,
            &clientLen
        );

        if (sockfd < 0)
        {
            perror("accept");
            continue;
        }

        std::cout << "Cliente conectado." << std::endl;

        auto msg = receiveMessage();

        if (msg)
        {
            std::cout << "Mensagem recebida. Tipo = "
                      << static_cast<int>(msg->msgtype)
                      << std::endl;
        }

        close(sockfd);
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
                int nodeSize;
                recv(sockfd, &nodeSize, sizeof(nodeSize), 0);

                std::vector<char> nodeBuffer(nodeSize);
                recv(sockfd, nodeBuffer.data(), nodeSize, 0);

                NodeInfo target =
                    NodeInfoSerializer::deserialize(nodeBuffer);

                int commandSize;
                recv(sockfd, &commandSize, sizeof(commandSize), 0);

                std::vector<char> commandBuffer(commandSize);
                recv(sockfd, commandBuffer.data(), commandSize, 0);

                ClientCommand command =
                    ClientCommandSerializer::deserialize(commandBuffer);

                return std::make_unique<sendClientCommandStruct>(
                    target,
                    command
                );
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

int Network::createConnection(const std::string& ip, int port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock < 0)
    {
        perror("socket");
        return -1;
    }


    sockaddr_in server{};

    server.sin_family = AF_INET;
    server.sin_port = htons(port);


    inet_pton(
        AF_INET,
        ip.c_str(),
        &server.sin_addr
    );


    if(connect(
        sock,
        (sockaddr*)&server,
        sizeof(server)
    ) < 0)
    {
        perror("connect");
        close(sock);
        return -1;
    }


    return sock;
}

void Network::sendRequestVote(sendRequestVoteStruct msg)
{
    int sock = createConnection(msg.target.getaddress(), msg.target.getport());
    if(sock < 0)
        return;

    messageType type = messageType::SEND_REQUEST_VOTE;

    send(sock, &type, sizeof(type), 0);

    auto data = RequestVoteMessageSerializer::serialize(msg.msg);

    int size = data.size();

    send(sock, &size, sizeof(size), 0);
    send(sock, data.data(), size, 0);

    close(sock);
}

void Network::sendClientCommand(sendClientCommandStruct msg)
{
    int sock = createConnection(
        msg.target.getaddress(),
        msg.target.getport()
    );

    if (sock < 0)
        return;

    messageType type = messageType::SEND_CLIENT_COMMAND;
    send(sock, &type, sizeof(type), 0);

    auto nodeBuffer = NodeInfoSerializer::serialize(msg.target);

    int nodeSize = nodeBuffer.size();
    send(sock, &nodeSize, sizeof(nodeSize), 0);
    send(sock, nodeBuffer.data(), nodeSize, 0);

    auto commandBuffer = ClientCommandSerializer::serialize(msg.msg);

    int commandSize = commandBuffer.size();
    send(sock, &commandSize, sizeof(commandSize), 0);
    send(sock, commandBuffer.data(), commandSize, 0);

    close(sock);
}

void Network::sendVoteResponse(sendVoteResponseStruct msg)
{
    int sock = createConnection(
        msg.target.getaddress(),
        msg.target.getport()
    );

    if (sock < 0)
        return;

    messageType type = messageType::SEND_VOTE_RESPONSE;
    send(sock, &type, sizeof(type), 0);

    std::vector<char> nodeBuffer =
        NodeInfoSerializer::serialize(msg.target);

    int size = nodeBuffer.size();

    send(sock, &size, sizeof(size), 0);
    send(sock, nodeBuffer.data(), size, 0);

    send(sock, &msg.voterID, sizeof(msg.voterID), 0);
    send(sock, &msg.currentTerm, sizeof(msg.currentTerm), 0);
    send(sock, &msg.granted, sizeof(msg.granted), 0);

    close(sock);
}

void Network::sendAppendAck(sendAppendAckStruct msg)
{
    int sock = createConnection(
        msg.target.getaddress(),
        msg.target.getport()
    );

    if (sock < 0)
        return;

    messageType type = messageType::SEND_APPEND_ACK;
    send(sock, &type, sizeof(type), 0);

    std::vector<char> nodeBuffer =
        NodeInfoSerializer::serialize(msg.target);

    int size = nodeBuffer.size();

    send(sock, &size, sizeof(size), 0);
    send(sock, nodeBuffer.data(), size, 0);

    send(sock, &msg.followerId, sizeof(msg.followerId), 0);
    send(sock, &msg.currentTerm, sizeof(msg.currentTerm), 0);
    send(sock, &msg.ack, sizeof(msg.ack), 0);
    send(sock, &msg.granted, sizeof(msg.granted), 0);

    close(sock);
}

void Network::sendAppendEntries(sendAppendEntriesStruct msg)
{
    int sock = createConnection(
        msg.target.getaddress(),
        msg.target.getport()
    );

    if (sock < 0)
        return;

    messageType type = messageType::SEND_APPEND_ENTRIES;
    send(sock, &type, sizeof(type), 0);

    // Envia o NodeInfo
    std::vector<char> nodeBuffer =
        NodeInfoSerializer::serialize(msg.target);

    int size = nodeBuffer.size();

    send(sock, &size, sizeof(size), 0);
    send(sock, nodeBuffer.data(), size, 0);

    // Envia os campos da mensagem
    send(sock, &msg.leaderId, sizeof(msg.leaderId), 0);
    send(sock, &msg.currentTerm, sizeof(msg.currentTerm), 0);
    send(sock, &msg.prefixLen, sizeof(msg.prefixLen), 0);
    send(sock, &msg.prefixTerm, sizeof(msg.prefixTerm), 0);
    send(sock, &msg.commitLength, sizeof(msg.commitLength), 0);

    // Envia a quantidade de entradas do log
    int logCount = msg.suffix.size();
    send(sock, &logCount, sizeof(logCount), 0);

    // Envia cada LogEntry
    for (auto& entry : msg.suffix)
    {
        std::vector<char> logBuffer =
            LogEntrySerializer::serialize(entry);

        size = logBuffer.size();

        send(sock, &size, sizeof(size), 0);
        send(sock, logBuffer.data(), size, 0);
    }

    close(sock);
}