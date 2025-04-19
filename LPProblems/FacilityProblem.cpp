#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

int FacilityProblem() {
    IloEnv env;
    try {
        // Criar modelo
        IloModel model(env);
        
        // Constantes
        const int CENTROS = 3;
        const int CLIENTES = 4;
        const int CUSTO_INSTALACAO[CENTROS] = {100, 150, 120};
        const int CUSTO_ATENDIMENTO[CENTROS][CLIENTES]= {
            {20, 104, 11, 325},
        	{28, 104, 325, 8},
        	{325, 5, 5, 96}
        };
        // Variáveis de decisão
        IloIntVarArray x(env, CENTROS, 0, 1); // Variável binária para indicar se o centro é aberto
        IloArray <IloIntVarArray> y(env, CENTROS); // Variável binária para indicar se o cliente é atendido pelo centro
        for (int i = 0; i < CENTROS; i++) {
            y[i] = IloIntVarArray(env, CLIENTES);
            for (int j = 0; j < CLIENTES; j++) {
                y[i][j] = IloIntVar(env, 0, 1);
            }
        }
        
        // Função Objetivo
        IloExpr custo_instalacao_total(env);
        for (int i = 0; i < CENTROS; i++) {
            custo_instalacao_total += x[i] * CUSTO_INSTALACAO[i];
        }
        IloExpr custo_atendimento_total(env);
        for (int i = 0; i < CENTROS; i++) {
            for (int j = 0; j < CLIENTES; j++){
                custo_atendimento_total += y[i][j] * CUSTO_ATENDIMENTO[i][j];
            }
        }
        model.add(IloMinimize(env, custo_instalacao_total + custo_atendimento_total));
        custo_instalacao_total.end();
        custo_atendimento_total.end();
        
        // Restrições
        // Cada cliente deve ser atendido por exatamente um centro
        for (int j = 0; j < CLIENTES; j++) {
            IloExpr soma(env);
            for (int i = 0; i < CENTROS; i++) {
                soma += y[i][j];
            }
            model.add(soma == 1);
            soma.end();
        }
        // Um centro pode atender um cliente apenas se estiver aberto
        for (int i = 0; i < CENTROS; i++) {
            for (int j = 0; j < CLIENTES; j++) {
                model.add(y[i][j] <= x[i]);
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
        cout << "Centros abertos: " << endl;
        for (int i = 0; i < CENTROS; i++) {
            if (cplex.getValue(x[i]) > 0.5) {
                cout << "Centro " << i + 1 << " aberto" << endl;
                for (int j = 0; j < CLIENTES; j++) {
                    if (cplex.getValue(y[i][j]) > 0.5) {
                        cout << "\tCliente " << j + 1 << " atendido pelo centro " << i + 1 << endl;
                    }
                }
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