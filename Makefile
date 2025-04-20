# Configurações do CPLEX
CPLEX_DIR      := /opt/ibm/ILOG/CPLEX_Studio2212
CPLEX_INC      := $(CPLEX_DIR)/cplex/include
CONCERT_INC    := $(CPLEX_DIR)/concert/include
CPLEX_LIB      := $(CPLEX_DIR)/cplex/lib/x86-64_linux/static_pic
CONCERT_LIB    := $(CPLEX_DIR)/concert/lib/x86-64_linux/static_pic

# Verificação de caminhos
ifeq ($(wildcard $(CPLEX_INC)/ilcplex/ilocplex.h),)
$(error CPLEX headers not found at $(CPLEX_INC)/ilcplex/ilocplex.h - Please check CPLEX_DIR)
endif

# Configurações do projeto
PROJECTS := LPProblems BinPacking
OBJ_DIR := obj
TARGETS := $(addsuffix /solver.out,$(PROJECTS))

# Flags do compilador
CXX            := g++
CXXFLAGS       := -O2 -Wall -std=c++17
INCLUDES       := -I$(CPLEX_INC) -I$(CONCERT_INC) -I$(CPLEX_INC)/ilcplex
LDFLAGS        := -L$(CPLEX_LIB) -L$(CONCERT_LIB)
LIBS           := -lilocplex -lconcert -lcplex -lm -lpthread -ldl

# Parâmetros padrão para execução
BP_ARGS ?= -time 10 -s < bin_packing.dat
LP_ARGS ?= 

# Cria diretório de objetos se não existir
$(shell mkdir -p $(OBJ_DIR))

# Regras de construção
.PHONY: all clean help $(PROJECTS) run-LP run-BP

all: help

help:
	@echo "Opções disponíveis:"
	@echo "  make LPProblems   - Compila o projeto na pasta LPProblems"
	@echo "  make BinPacking   - Compila o projeto na pasta BinPacking"
	@echo "  make run-LP       - Executa o projeto LPProblems"
	@echo "  make run-BP       - Executa o projeto BinPacking"
	@echo "  make clean        - Limpa todos os projetos e objetos"

# Regras para cada projeto
define PROJECT_RULES
$(1)_SRC := $(wildcard $(1)/*.cpp)
$(1)_OBJ := $$(patsubst $(1)/%.cpp,$(OBJ_DIR)/$(1)_%.o,$$($(1)_SRC))

$(1)/solver.out: $$($(1)_OBJ)
	@echo "Linking $$@..."
	@$$(CXX) $$(CXXFLAGS) $$^ -o $$@ $$(LDFLAGS) $$(LIBS)
	@echo "Build successful for $(1)!"

$(OBJ_DIR)/$(1)_%.o: $(1)/%.cpp
	@echo "Compiling $$<..."
	@$$(CXX) $$(CXXFLAGS) $$(INCLUDES) -I$(1) -c $$< -o $$@
endef

# Aplica as regras para cada projeto
$(foreach proj,$(PROJECTS),$(eval $(call PROJECT_RULES,$(proj))))

# Atalhos para compilação
$(PROJECTS): %: %/solver.out

# Função para encontrar arquivo de input
define find_input
$(firstword $(wildcard $(1)/bin_packing.dat bin_packing.dat))
endef

# Regras de execução
run-LP: LPProblems/solver.out
	@echo "Running LPProblems..."
	@./$<

run-BP: BinPacking/solver.out
	@$(eval INPUT_FILE := $(call find_input,BinPacking))
	@if [ -z "$(INPUT_FILE)" ]; then \
		echo "Erro: Nenhum arquivo input.txt encontrado nem em BinPacking/ nem no diretório raiz"; \
		exit 1; \
	fi
	@echo "Running BinPacking com arquivo de input: $(INPUT_FILE)"
	@./$< -time 60 -s < "$(INPUT_FILE)"

clean:
	@rm -rf $(TARGETS) $(OBJ_DIR)
	@echo "Clean complete - all targets and objects removed."