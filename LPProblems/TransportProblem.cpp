#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

int TransportProblem() {
    IloEnv env;
    try {
        // Criar modelo
        IloModel model(env);
        
        //Constantes
        const int NUM_DEPOSITOS = 3;
        const int NUM_FABRICAS = 3;
        const int custoFabDep[NUM_FABRICAS][NUM_DEPOSITOS] = {
            {8, 5, 6},
            {15, 10, 12},
            {3, 9, 10}
        };
        const int OFERTA[NUM_FABRICAS] = {120, 80, 80};
        const int DEMANDA[NUM_DEPOSITOS] = {150, 70, 60};


        // Variáveis
        IloArray<IloIntVarArray> x(env, NUM_FABRICAS);
        for (int i = 0; i < NUM_FABRICAS; i++) {
            x[i] = IloIntVarArray(env, NUM_DEPOSITOS, 0, 120);
            for (int j = 0; j < NUM_DEPOSITOS; j++) {
                x[i][j].setName(("x" + std::to_string(i + 1) + std::to_string(j + 1)).c_str());
            }
        }
        
        // Função objetivo
        IloExpr obj(env);
        for (int i = 0; i < NUM_FABRICAS; i++) {
            for (int j = 0; j < NUM_DEPOSITOS; j++) {
                obj += custoFabDep[i][j] * x[i][j];
            }
        }
        model.add(IloMinimize(env, obj));
        obj.end();
        
        // Restrições
        // Oferta
        for (int i = 0; i < NUM_FABRICAS; i++) {
            IloExpr expr(env);
            for (int j = 0; j < NUM_DEPOSITOS; j++) {
                expr += x[i][j];
            }
            model.add(expr <= OFERTA[i]);
            expr.end();
        }
        // Demanda
        for (int j = 0; j < NUM_DEPOSITOS; j++) {
            IloExpr expr(env);
            for (int i = 0; i < NUM_FABRICAS; i++) {
                expr += x[i][j];
            }
            model.add(expr == DEMANDA[j]);
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
        cout << "Depósito\t1\t2\t3" << endl;
        for (int i = 0; i < NUM_FABRICAS; i++) {
            cout << "Fábrica "<< i << "\t";
            for (int j = 0; j < NUM_DEPOSITOS; j++) {
                cout << cplex.getValue(x[i][j]) << "\t";
            }
            cout << endl;
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