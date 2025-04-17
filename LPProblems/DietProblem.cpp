#include <ilcplex/ilocplex.h>
#include <iostream>
ILOSTLBEGIN

int DietProblem() {
    IloEnv env;
    try {
        // Criar modelo
        IloModel model(env);
        
        // Máximo variável
        const IloInt MAX_QUANTITY = 10000;

        // Constantes
        const int NUM_VITAMINAS = 2;
        const int NUM_INGREDIENTES = 6;

        int vitaminas[NUM_VITAMINAS][NUM_INGREDIENTES] = {
            {1, 0, 2, 2, 1, 2}, // Vitamina A
            {0, 1, 3, 1, 3, 2}  // Vitamina C
        };

        int quantidade_minima[NUM_VITAMINAS] = { 9, 19 }; // Quantidade mínima de cada vitamina

        int precos[NUM_INGREDIENTES] = { 35, 30, 60, 50, 27, 22 }; // Preço de cada ingrediente

        // Variável
        IloIntVarArray x(env, NUM_INGREDIENTES, 0, MAX_QUANTITY);
        
        // Nomear as variáveis
        for (int i = 0; i < NUM_INGREDIENTES; i++) {
            x[i].setName(("x" + std::to_string(i + 1)).c_str());
        }

        // Função objetivo: Minimizar custo
        IloExpr custo(env);
        for (int i = 0; i < NUM_INGREDIENTES; i++) {
            custo += precos[i] * x[i];
        }
        model.add(IloMinimize(env, custo));
        custo.end();
        
        // Restrições
            // Total de cada vitamina
        for (int i = 0; i < NUM_VITAMINAS; i++) {
            IloExpr vitamina(env);
            for (int j = 0; j < NUM_INGREDIENTES; j++) {
                vitamina += vitaminas[i][j] * x[j];
            }
            model.add(vitamina >= quantidade_minima[i]);
            vitamina.end();
        }

        // Resolver
        IloCplex cplex(model);
        if (!cplex.solve()) {
            env.error() << "Falha ao resolver o modelo" << endl;
            throw(-1);
        }
        
        // Resultados
        cout << "Solução ótima = " << cplex.getObjValue() << endl;
        cout << "Quantidade de cada ingrediente:" << endl;
        for (int i = 0; i < NUM_INGREDIENTES; i++) {
            cout << x[i].getName() << " = " << cplex.getValue(x[i]) << endl;
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