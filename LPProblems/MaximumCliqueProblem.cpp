#include <ilcplex/ilocplex.h>
#include <vector>
#include <iostream>
ILOSTLBEGIN

struct Edge {
    int from;
    int to;
};

int MaximumCliqueProblem() {
    IloEnv env;
    try {
        // Criar modelo
        IloModel model(env);

        //Constantes
        const std::vector<Edge> EDGES = {
            {0, 1}, {0, 2}, {0, 3}, {0, 4},
            {1, 2}, {1, 5}, {1, 6},
            {2, 3}, {2, 7},
            {3, 4}, {3, 8}, {3, 9},
            {4, 9}, {4, 10},
            {5, 6}, {5, 7}, {5, 11},
            {6, 7}, {6, 11},
            {7, 8}, {7, 11},
            {8, 9}, {8, 11},
            {9, 10}, {9, 11},
            {10, 11}
        };
        const int NUM_NOS = 12;

        vector<vector<int>> adj(NUM_NOS, vector<int>(NUM_NOS, 0));
        for (const auto& e : EDGES) {
            adj[e.from][e.to] = 1;
            adj[e.to][e.from] = 1;
        }

        // Variáveis de decisão
        // Vértices que fazem ou não parte da clique
        IloIntVarArray x(env, NUM_NOS, 0, 1);
        for (int i = 0; i < NUM_NOS; i++) {
            x[i].setName(("Vértice " + std::to_string(i) ).c_str());
        }
        // Função objetivo: Maximizar os vértices que fazem parte da clique
        model.add(IloMaximize(env, IloSum(x)));
        // Restrições: Para cada dois vértices não adjacentes, pelo menos um deles não pode fazer parte da clique
        for (int i = 0; i < NUM_NOS; i++) {
            for (int j = i + 1; j < NUM_NOS; j++) {
                if (adj[i][j] == 0) {
                    model.add(x[i] + x[j] <= 1);
                }
            }
        }
        // Resolver
        IloCplex cplex(model);
        if (!cplex.solve()) {
            env.error() << "Falha ao resolver o modelo" << endl;
            throw(-1);
        }
        
        // Resultados
        cout << "Solução ótima = " << cplex.getObjValue() << endl;
        cout << "Vértices na clique: " <<  cplex.getObjValue() << endl;
        for (int i = 0; i < NUM_NOS; i++) {
            if (cplex.getValue(x[i]) > 0.5) {
                cout << "\t" << x[i].getName() << " faz parte da clique" << endl;
            }
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