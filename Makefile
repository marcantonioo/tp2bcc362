# ==============================================================================
# Makefile para Compilação e Execução do Cluster Raft e Cliente de Teste
# ==============================================================================

# Compilador e Flags
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -I. -I..
LDFLAGS  := -pthread

# Diretórios
SRC_DIR  := .
TEST_DIR := testes
BIN_DIR  := bin
OBJ_DIR  := obj

# Arquivos de Origem do Raft/Rede (Diretório Raiz)
# Pega todos os arquivos .cpp da raiz (certifique-se de que server.cpp e client.cpp estejam APENAS na pasta testes/)
COMMON_SRCS := $(wildcard $(SRC_DIR)/*.cpp)
COMMON_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(COMMON_SRCS))

# Fontes dos Testes (Localizados em testes/)
CLUSTER_TEST_SRC := $(TEST_DIR)/server.cpp
CLIENT_TEST_SRC  := $(TEST_DIR)/client.cpp

CLUSTER_TEST_OBJ := $(OBJ_DIR)/server.o
CLIENT_TEST_OBJ  := $(OBJ_DIR)/client.o

# Executáveis Finais
TARGET_CLUSTER := $(BIN_DIR)/raft_cluster
TARGET_CLIENT  := $(BIN_DIR)/client

# ==============================================================================
# Regras Principais
# ==============================================================================

.PHONY: all clean run-node1 run-node2 run-node3 run-client help

all: $(TARGET_CLUSTER) $(TARGET_CLIENT)

# Criação dos diretórios de build
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# Compilação dos módulos comuns (raft, network, serializers, etc.)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilação do teste do Cluster
$(CLUSTER_TEST_OBJ): $(CLUSTER_TEST_SRC) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilação do teste do Cliente
$(CLIENT_TEST_OBJ): $(CLIENT_TEST_SRC) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Linkagem do Executável do Cluster
$(TARGET_CLUSTER): $(COMMON_OBJS) $(CLUSTER_TEST_OBJ) | $(BIN_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS)
	@echo "🟢 Executável $(TARGET_CLUSTER) gerado com sucesso!"

# Linkagem do Executável do Cliente
$(TARGET_CLIENT): $(COMMON_OBJS) $(CLIENT_TEST_OBJ) | $(BIN_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS)
	@echo "🟢 Executável $(TARGET_CLIENT) gerado com sucesso!"

# ==============================================================================
# Atalhos de Execução para Testes
# ==============================================================================

# Atalhos para rodar os nós locais em terminais separados (Cluster de 3 nós)
run-node1: $(TARGET_CLUSTER)
	./$(TARGET_CLUSTER) 1 8001 3

run-node2: $(TARGET_CLUSTER)
	./$(TARGET_CLUSTER) 2 8002 3

run-node3: $(TARGET_CLUSTER)
	./$(TARGET_CLUSTER) 3 8003 3

# Atalho para rodar a bateria de testes do cliente
run-client: $(TARGET_CLIENT)
	./$(TARGET_CLIENT) 127.0.0.1 8001

# Limpeza de artefatos de compilação
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "🧹 Limpeza concluída."

# Ajuda
help:
	@echo "Comandos disponíveis:"
	@echo "  make              - Compila os executáveis do cluster e do cliente"
	@echo "  make run-node1    - Inicia o Nó 1 na porta 8001"
	@echo "  make run-node2    - Inicia o Nó 2 na porta 8002"
	@echo "  make run-node3    - Inicia o Nó 3 na porta 8003"
	@echo "  make run-client   - Inicia o cliente de testes (alvo: 127.0.0.1 8001)"
	@echo "  make clean        - Remove diretórios obj/ e bin/"