#include <ilcplex/ilocplex.h>
#include <vector>
#include <iostream>
ILOSTLBEGIN

struct Edge {
    int from;
    int to;
};

int FrequencyProblem() {
    IloEnv env;
    try {
        // Criar modelo
        IloModel model(env);

        //Constantes
        const std::vector<Edge> EDGES = {
            {0, 1}, {0, 2}, {0, 3}, {0, 4},
            {1, 2}, {1, 3}, {1, 4},
            {2, 3}, {2, 4},
            {3, 4}
        };
        const int NUM_NOS = 5;

        // Variáveis de decisão
        // Cor de cada vértice
        IloIntVarArray x(env, NUM_NOS, 1, NUM_NOS);
        for (int i = 0; i < NUM_NOS; i++) {
            x[i].setName(("Frequência da antena " + std::to_string(i) + ": ").c_str());
        }
        // Variável para o número de cores
        IloIntVar numCores(env, 1, NUM_NOS);
        numCores.setName("Número de frequências: ");

        // Função objetivo: minimizar o número de cores
        model.add(IloMinimize(env, numCores));
        
        // Restrições: cada vértice deve ter uma cor diferente de seus vizinhos
        for (const auto& edge : EDGES) {
            model.add(x[edge.from] != x[edge.to]);
        }
        // numCores tem que ser maior ou igual ao número de qualquer cor
        for (int i = 0; i < NUM_NOS; i++) {
            model.add(numCores >= x[i]);
        }
        
        // Resolver
        IloCplex cplex(model);
        if (!cplex.solve()) {
            env.error() << "Falha ao resolver o modelo" << endl;
            throw(-1);
        }
        
        // Resultados
        cout << "Solução ótima = " << cplex.getObjValue() << endl;
        cout << numCores.getName() << cplex.getValue(numCores) << endl;
        for (int i = 0; i < NUM_NOS; i++) {
            cout << x[i].getName() << cplex.getValue(x[i]) << endl;
        }
        
    }
    catch (IloException& e) {
        cerr << "Erro CPLEX: " << e << endl;
    }
    catch (...) {
        cerr << "Erro desconhecido" << endl;
    }
    env.end();
    return 0;
}