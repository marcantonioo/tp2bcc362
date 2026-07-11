 
#include "../Network.h"
#include "../LogEntry.h"
#include "../NodeInfo.hpp"
#include <iostream>
#include <string>

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

    std::string key, value;
    std::cout << "==========================================" << std::endl;
    std::cout << " 🖥️ CLIENTE RAFT CONECTADO" << std::endl;
    std::cout << " Alvo: " << leaderIp << ":" << leaderPort << std::endl;
    std::cout << "==========================================\n" << std::endl;

    std::cout << "Digite a CHAVE para gravar no cluster: ";
    std::cin >> key;
    std::cout << "Digite o VALOR para a chave '" << key << "': ";
    std::cin >> value;

    // ID fictício para este cliente de teste
    int clientId = 999; 
    ClientCommand cmd(clientId, Operation::WRITE, key, value);

    std::cout << "\nEnviando comando (WRITE, " << key << " = " << value << ") para o líder..." << std::endl;
    
    // Usa a sua própria estrutura de rede para despachar a mensagem!
    sendClientCommandStruct sendCmd(leaderNode, cmd);
    network.sendClientCommand(sendCmd);

    std::cout << "Comando enviado com sucesso!" << std::endl;

    return 0;
}