#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

int RationProblem() {
    IloEnv env;
    try {
        // Criar modelo
        IloModel model(env);
        
        int MAX_CARNES = 10000;
        int MAX_CEREAIS = 30000;
        int MAX_AMGS = MAX_CEREAIS/5;
        int MAX_RE = MAX_CARNES/4;

        // Variáveis
        IloIntVar AMGS(env, 0.0, MAX_AMGS, "AMGS");
        IloIntVar RE(env, 0.0, MAX_RE, "RE");
        
        //AMGS utiliza 5kg de cereais e 1kg de carne
        //RE utiliza 2kg de cereais e 4kg de carne
        //Lucro líquido unidade da ração
            //AMGS = 20 - (5kg * 1 + 1kg * 4) = 11
            //RE = 30 - (2kg * 1 + 4kg * 4) = 12
        
            // Função objetivo: Maximizar lucro:
            // AMGS*11 + RE*12
        model.add(IloMaximize(env, AMGS * 11 + RE * 12));
        
        // Restrições
            // Total de cereais: AMGS * 5 + RE * 2 <=30000
            // Total de carne: AMGS * 1 + RE * 4 <= 10000
        model.add(AMGS * 5 + RE * 2 <= MAX_CEREAIS);
        model.add(AMGS * 1 + RE * 4 <= MAX_CARNES);
        
        // Resolver
        IloCplex cplex(model);
        if (!cplex.solve()) {
            env.error() << "Falha ao resolver o modelo" << endl;
            throw(-1);
        }
        
        // Resultados
        cout << "Solução ótima = " << cplex.getObjValue() << endl;
        cout << "AMGS = " << cplex.getValue(AMGS) << endl;
        cout << "RE = " << cplex.getValue(RE) << endl;
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