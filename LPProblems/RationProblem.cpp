#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

int RationProblem() {
    IloEnv env;
    try {
        // Criar modelo
        IloModel model(env);
        
        // Variáveis
        IloNumVar x(env, 0.0, 40.0, ILOFLOAT, "x");
        IloNumVar y(env, 0.0, IloInfinity, ILOFLOAT, "y");
        
        // Função objetivo: Maximizar x + 2y
        model.add(IloMaximize(env, x + 2*y));
        
        // Restrições
        model.add(-x + y <= 20);
        model.add( x + y <= 60);
        model.add( x - 3*y <= 0);
        
        // Resolver
        IloCplex cplex(model);
        if (!cplex.solve()) {
            env.error() << "Falha ao resolver o modelo" << endl;
            throw(-1);
        }
        
        // Resultados
        cout << "Solução ótima = " << cplex.getObjValue() << endl;
        cout << "x = " << cplex.getValue(x) << endl;
        cout << "y = " << cplex.getValue(y) << endl;
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