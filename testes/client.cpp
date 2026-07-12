#include "../Network.h"
#include "../LogEntry.h"
#include "../NodeInfo.hpp"
#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: ./client [IP_DO_LIDER] [PORTA_DO_LIDER]" << std::endl;
        std::cerr << "Exemplo: ./client 127.0.0.1 8001" << std::endl;
        return 1;
    }

    std::string leaderIp = argv[1];
    int leaderPort = std::stoi(argv[2]);

    // Cria um NodeInfo apontando para o Líder que receberá a mensagem
    NodeInfo leaderNode(-1, leaderPort, leaderIp); 
    
    Network network;

    std::cout << "==========================================" << std::endl;
    std::cout << " 🖥️ CLIENTE RAFT AUTOMATIZADO CONECTADO" << std::endl;
    std::cout << " Alvo: " << leaderIp << ":" << leaderPort << std::endl;
    std::cout << "==========================================\n" << std::endl;

    // Configuração dos geradores de números aleatórios
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Distribuições solicitadas: 10 a 50 acessos; 1 a 5 segundos de espera
    std::uniform_int_distribution<> reqDist(10, 50);
    std::uniform_int_distribution<> timeDist(1, 5);

    int numRequests = reqDist(gen);
    int clientId = 999; // ID fictício para esta bateria de testes

    std::cout << "Iniciando bateria automatizada de " << numRequests << " requisições de escrita...\n" << std::endl;

    // Loop de envio de comandos aleatorizados
    for (int i = 1; i <= numRequests; ++i) {
        // Gera dados sintéticos formatados
        std::string key = "chave_auto_" + std::to_string(i);
        std::string value = "valor_auto_" + std::to_string(i);

        ClientCommand cmd(clientId, Operation::WRITE, key, value);

        std::cout << "[Req " << i << "/" << numRequests << "] Enviando: (WRITE, " 
                  << key << " = " << value << ")...\n";
        
        // Empacota e envia a mensagem utilizando a assinatura original
        sendClientCommandStruct sendCmd(leaderNode, cmd);
        network.sendClientCommand(sendCmd);

        // Aguarda um tempo aleatório (1 a 5 seg) antes da próxima requisição, exceto no último envio
        if (i < numRequests) {
            int delay = timeDist(gen);
            std::cout << "    -> Aguardando " << delay << " segundos...\n";
            std::this_thread::sleep_for(std::chrono::seconds(delay));
        }
    }

    std::cout << "\n==========================================" << std::endl;
    std::cout << " Bateria de testes finalizada com sucesso." << std::endl;
    std::cout << "==========================================" << std::endl;

    return 0;
}