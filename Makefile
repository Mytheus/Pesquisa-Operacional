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
TARGET         := solver.out
SRC_DIR        := LPProblems
SRC            := $(wildcard $(SRC_DIR)/*.cpp)
OBJ            := $(patsubst $(SRC_DIR)/%.cpp,%.o,$(SRC))

# Flags do compilador
CXX            := g++
CXXFLAGS       := -O2 -Wall -std=c++11
INCLUDES       := -I$(CPLEX_INC) -I$(CONCERT_INC) -I$(CPLEX_INC)/ilcplex -I$(SRC_DIR)
LDFLAGS        := -L$(CPLEX_LIB) -L$(CONCERT_LIB)
LIBS           := -lilocplex -lconcert -lcplex -lm -lpthread -ldl

# Regras de construção
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	@echo "Linking $@..."
	@$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)
	@echo "Build successful!"

%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@rm -f $(TARGET) $(OBJ)
	@echo "Clean complete."