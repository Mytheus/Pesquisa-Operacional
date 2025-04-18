#include <ilcplex/ilocplex.h>
#include <vector>
#include <iostream>
ILOSTLBEGIN

struct Edge {
    int from;
    int to;
    int capacity;
};

int MaxFlowProblem() {
    IloEnv env;
    try {
        // Criar modelo
        IloModel model(env);

        // // Constantes de exemplo
        // const int ORIGEM = 1;   // Nó de origem (s)
        // //const int DESTINO = 6;     // Nó de destino (t) //nao usada
        // const int NUM_NOS = 6; // Total de nós

        // // Lista de arestas (dutos) com capacidades
        // std::vector<Edge> EDGES = {
        //     {1, 2, 16},  // s → 2
        //     {1, 3, 13},  // s → 3
            
        //     {2, 3, 10},  // 2 → 3
        //     {2, 4, 12},  // 2 → 4
        //     {3, 5, 14},  // 3 → 5
            
        //     {4, 3, 9},   // 4 → 3
        //     {4, 6, 20},  // 4 → t
        //     {5, 4, 7},   // 5 → 4
        //     {5, 6, 4},   // 5 → t
        // };

        const int ORIGEM = 1;   // Nó de origem (s)
        // const int DESTINO = 4;     // Nó de destino (t) //nao usada
        const int NUM_NOS = 4; // Total de nós

        std::vector<Edge> EDGES = {
                {1, 2, 10},
                {1, 3, 5},
                {2, 3, 15},
                {2, 4, 8},
                {3, 4, 10}
            };


        // Variáveis de decisão
        IloArray<IloIntVarArray> x(env, NUM_NOS + 1);
        for (int i = 0; i < NUM_NOS; i++) {
            x[i] = IloIntVarArray(env, NUM_NOS + 1, 0, 1000); // Limite superior arbitrário
            for (int j = 0; j < NUM_NOS; j++) {
                if(i != j)
                    x[i][j].setName(("x" + std::to_string(i) + std::to_string(j)).c_str());
            }
        }
        // Função objetivo: maximizar o fluxo
        IloExpr obj(env);
        for (const auto& edge : EDGES) {
            if (edge.from == ORIGEM) {
                obj += x[edge.from][edge.to];
            }
        }
        model.add(IloMaximize(env, obj));        
        obj.end();
        // Restrições de capacidade
        for (const auto& edge : EDGES) {
            model.add(x[edge.from][edge.to] <= edge.capacity);
        }
        // Restrições de conservação de fluxo
        for (int i = ORIGEM + 1; i < NUM_NOS; i++) { // Ignora origem e destino
            IloExpr in(env);
            IloExpr out(env);
            for (const auto& edge : EDGES) {
                if (edge.from == i) {
                    out += x[edge.from][edge.to];
                }
                if (edge.to == i) {
                    in += x[edge.from][edge.to];
                }
            }
            model.add(in == out);
            in.end();
            out.end();
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
        for (const auto& edge : EDGES) {
            double flow = cplex.getValue(x[edge.from][edge.to]);
            cout << edge.from << " -> " << edge.to << " : " << flow << endl;
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