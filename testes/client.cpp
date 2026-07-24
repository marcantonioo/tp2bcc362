#include "../Network.h"
#include "../LogEntry.h"
#include "../NodeInfo.hpp"
#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// Função auxiliar para abrir o socket de escuta
int createListeningSocket(int port) {
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) return -1;

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(listenfd, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        close(listenfd);
        return -1;
    }

    // Permite fila de conexões pendentes para não perder respostas rápidas
    if (listen(listenfd, 5) < 0) {
        close(listenfd);
        return -1;
    }

    return listenfd;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: ./client [IP_DO_LIDER] [PORTA_DO_LIDER]" << std::endl;
        std::cerr << "Exemplo: ./client 127.0.0.1 8001" << std::endl;
        return 1;
    }

    std::string leaderIp = argv[1];
    int leaderPort = std::stoi(argv[2]);

    NodeInfo leaderNode(-1, leaderPort, leaderIp); 
    Network network;

    std::cout << "==========================================" << std::endl;
    std::cout << " 🖥️ CLIENTE RAFT AUTOMATIZADO CONECTADO" << std::endl;
    std::cout << " Alvo: " << leaderIp << ":" << leaderPort << std::endl;
    std::cout << "==========================================\n" << std::endl;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> reqDist(10, 50);
    std::uniform_int_distribution<> timeDist(1, 5);

    int numRequests = reqDist(gen);
    int clientId = 999; 
    int clientPort = 9001; 

    // OTIMIZAÇÃO: Abre o socket de escuta UMA única vez antes de iniciar o loop[cite: 13]
    int listenfd = createListeningSocket(clientPort);
    if (listenfd < 0) {
        std::cerr << " [Erro Crítico] Falha ao abrir porta de escuta local " << clientPort << std::endl;
        return 1;
    }

    std::cout << "Iniciando bateria automatizada de " << numRequests << " requisições de escrita...\n" << std::endl;

    for (int i = 1; i <= numRequests; ++i) {
        std::string key = "chave_auto_" + std::to_string(i);
        std::string value = "valor_auto_" + std::to_string(i);

        // O comando agora tem a garantia de que o servidor enxergará "127.0.0.1:9001"[cite: 13]
        ClientInfo clientInfo(clientPort, "127.0.0.1", clientId);
        ClientCommand cmd(clientInfo, Operation::WRITE, key, value);

        std::cout << "[Req " << i << "/" << numRequests << "] Enviando: (WRITE, " 
                  << key << " = " << value << ")...\n";
        
        sendClientCommandStruct sendCmd(leaderNode, cmd);
        network.sendClientCommand(sendCmd);

        std::cout << "    -> Aguardando confirmação (ACK) do cluster..." << std::endl;

        // Aceita o socket da conexão da resposta sem precisar recriar a porta de escuta[cite: 13]
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int responseSock = accept(listenfd, (sockaddr*)&clientAddr, &clientLen);

        if (responseSock >= 0) {
            auto msg = network.receiveMessage(responseSock);
            
            if (msg && msg->msgtype == messageType::SEND_CLIENT_RESPONSE) {
                auto response = static_cast<sendClientResponseStruct*>(msg.get());
                std::cout << "    -> Resposta do Líder: " << response->status << std::endl;
            } else {
                std::cerr << "    -> Erro: Resposta inválida ou nula do cluster." << std::endl;
            }

            // Fecha apenas a conexão pontual da resposta atual[cite: 13]
            close(responseSock);
        } else {
            std::cerr << "    -> Erro ao aceitar conexão de resposta." << std::endl;
        }

        if (i < numRequests) {
            int delay = timeDist(gen);
            std::cout << "    -> Sucesso! Aguardando " << delay << " segundos para a próxima...\n\n";
            std::this_thread::sleep_for(std::chrono::seconds(delay));
        }
    }

    // Fecha o servidor que escutava a porta 9001 após o término de todos os testes[cite: 13]
    close(listenfd);

    std::cout << "\n==========================================" << std::endl;
    std::cout << " Bateria de testes finalizada com sucesso." << std::endl;
    std::cout << "==========================================" << std::endl;

    return 0;
}