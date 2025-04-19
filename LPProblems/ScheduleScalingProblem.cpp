#include <ilcplex/ilocplex.h>

ILOSTLBEGIN


int ScheduleScalingProblem() {
    IloEnv env;
    try {
        // Criar modelo
        IloModel model(env);

        // Constantes de exemplo
        const int WEEK = 7;
        const int DEMANDA[WEEK] = {2, 3, 4, 2, 3, 1, 1};
        const int WORK = 5;

        // 0 --> Domingo 
        // 1 --> Segunda
        // 2 --> Terça
        // 3 --> Quarta
        // 4 --> Quinta
        // 5 --> Sexta
        // 6 --> Sábado
        
        // Variáveis de decisão
        IloIntVarArray x(env, WEEK, 0, 1000); // Limite superior arbitrário
        for (int i = 0; i < WEEK; i++) {
            x[i].setName(("x" + std::to_string(i)).c_str());
        }
        // Função objetivo: Minimizar a demanda
        IloExpr obj(env);
        for (int i = 0; i < WEEK; i++) {
            obj += x[i];
        }
        model.add(IloMinimize(env, obj));
        obj.end();
        // Restrições
        // Restrição de dias de trabalho (D[i] <= Xi-4 + Xi-3 + Xi-2 + Xi-1 + Xi) (Assumindo um array circular)
        for (int i = 0; i < WEEK; i++) {
            int cont = WORK -1;
            IloExpr expr(env);
            while (cont >= 0){
                int indice = ((i - cont) + WEEK) % WEEK;
                expr += x[indice];
                cont--;
            }
            model.add(expr >= DEMANDA[i]);
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
        for (int i = 0; i < WEEK; i++) {
            cout << " DIA "<< i + 1 << " x[" << i << "] = " << cplex.getValue(x[i]) << endl;
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