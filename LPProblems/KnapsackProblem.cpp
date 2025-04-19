#include <ilcplex/ilocplex.h>
#include <vector>
ILOSTLBEGIN

int KnapsackProblem() {
    IloEnv env;
    try {
        // Criar modelo
        IloModel model(env);

        //Constantes
        const int NUM_ITENS = 5;
        const int CAPACIDADE_MOCHILA = 20;
        const std::vector<int> VALORES = {10, 14, 7, 9, 19};
        const std::vector<int> PESOS = {11, 13, 7, 5, 13};

        // Variáveis de decisão
        IloIntVarArray x(env, NUM_ITENS, 0, 1);
        for (int i = 0; i < NUM_ITENS; i++) {
            x[i].setName(("X" + std::to_string(i)).c_str());
        }


        // Função objetivo: Máximizar os valores dos itens na mochila
        IloExpr obj(env);
        for (int i = 0; i < NUM_ITENS; i++) {
            obj += VALORES[i] * x[i];
        }
        model.add(IloMaximize(env, obj));
        obj.end();

        // Restrições: Limite de peso
        IloExpr pesoExpr(env);
        for (int i = 0; i < NUM_ITENS; i++) {
            pesoExpr += PESOS[i] * x[i];
        }
        model.add(pesoExpr <= CAPACIDADE_MOCHILA);
        pesoExpr.end();
        // Resolver
        IloCplex cplex(model);
        if (!cplex.solve()) {
            env.error() << "Falha ao resolver o modelo" << endl;
            throw(-1);
        }
        
        // Resultados
        cout << "Solução ótima = " << cplex.getObjValue() << endl;
        // Itens incluídos na mochila
        for (int i = 0; i < NUM_ITENS; i++) {
            if (cplex.getValue(x[i]) > 0.5) { // Se o item foi incluído
                cout << "Item incluído: " << i << " Valor: "<< VALORES[i] << " Peso: "<< PESOS[i] << endl;
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