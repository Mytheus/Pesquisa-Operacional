#include <ilcplex/ilocplex.h>
#include <vector>
#include <iostream>
ILOSTLBEGIN

struct Edge {
    int from;
    int to;
};

int CoverProblem() {
    IloEnv env;
    try {
        // Criar modelo
        IloModel model(env);

        //Constantes
        const int NUM_NOS = 5;
        const std::vector<Edge> EDGES = {
            {0, 1}, 
            {0, 2}, 
            {1, 2}, 
            {1, 3}, 
            {2, 3}, 
            {3, 4} 
        };

        // Variáveis de decisão
        IloIntVarArray x(env, NUM_NOS, 0, 1);
        for (int i = 0; i < NUM_NOS; i++) {
            x[i].setName(("X" + std::to_string(i)).c_str());
        }

        // Função objetivo: minimizar o número de nós cobertos
        model.add(IloMinimize(env, IloSum(x)));

        // Restrições: cada vértice deve ser coberto por pelo menos um nó
        for (int i = 0; i < NUM_NOS; i++) {
            IloExpr expr(env);
            for (const auto& edge : EDGES) {
                if (edge.from == i || edge.to == i) {
                    expr += x[edge.from] + x[edge.to];
                }
            }
            model.add(expr >= 1);
            expr.end();
        }

        // Resolver
        IloCplex cplex(model);
        if (!cplex.solve()) {
            env.error() << "Falha ao resolver o modelo" << endl;
            throw(-1);
        }
        
        // Resultados
        cout << "Solução ótima = " << cplex.getObjValue() << endl;
        // Exibir a solução como lista de adjacências
        for (int i = 0; i < NUM_NOS; i++) {
            cout << x[i].getName() << ": " << cplex.getValue(x[i]) << endl;
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