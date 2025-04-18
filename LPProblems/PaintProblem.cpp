#include <ilcplex/ilocplex.h>
#include <iostream>
ILOSTLBEGIN

int PaintProblem() {
    IloEnv env;
    try {
        // Criar modelo
        IloModel model(env);
        // Constantes
        const int MIN_SEC_SR = 1000 * 0.25;
        const int MIN_COR_SR = 1000 * 0.5;
        const int MIN_SEC_SN = 250 * 0.2;
        const int MIN_COR_SN = 250 * 0.5;

        
        
        // Xij = Quantidade do componente i na tinta j
        // i = {SolA, SolB, SEC, COR}
        const int NUM_COMPONENTES = 4;
        // j = {SR, SN}
        const int NUM_TINTAS = 2;

        const double PRECOS[NUM_COMPONENTES] = {
            1.5, 1, 4, 6
        };
        const int VOLUMES[NUM_TINTAS] = {
            1000, 250
        };

        const int SEC[NUM_TINTAS] = {
            MIN_SEC_SR, MIN_SEC_SN
        };
        const int COR[NUM_TINTAS] = {
            MIN_COR_SR, MIN_COR_SN
        };
        // Variáveis de decisão
        IloArray<IloIntVarArray> x(env, NUM_COMPONENTES);
        for (int i = 0; i < NUM_COMPONENTES; i++) {
            x[i] = IloIntVarArray(env, NUM_TINTAS, 0, 1000);
            for (int j = 0; j < NUM_TINTAS; j++) {
                x[i][j].setName(("x" + std::to_string(i + 1) + std::to_string(j + 1)).c_str());
            }
        }
        // Função objetivo: Minimizar custo
        IloExpr custo(env);
        for (int j = 0; j < NUM_TINTAS; j++) {
            for (int i = 0; i < NUM_COMPONENTES; i++) {
                custo += x[i][j] * PRECOS[i];
            }
        }
        model.add(IloMinimize(env, custo));
        custo.end();
        // Restrições
        // Restrição de volume
        for (int j = 0; j < NUM_TINTAS; j++) {
            IloExpr volume_total(env);
            for (int i = 0; i < NUM_COMPONENTES; i++) {
                volume_total += x[i][j];
            }
            model.add(volume_total == VOLUMES[j]);
            volume_total.end();
        }
        // Restrição QNTD mínima SEC
        for (int j = 0; j < NUM_TINTAS; j++) {
            IloExpr SEC_Total(env);
            SEC_Total = x[0][j] * 0.3 + x[1][j] * 0.6 + x[2][j];
            model.add(SEC_Total >= SEC[j]);
            SEC_Total.end();
        }
        // Restrição QNTD mínima COR
        for (int j = 0; j < NUM_TINTAS; j++) {
            IloExpr COR_Total(env);
            COR_Total = x[0][j] * 0.7 + x[1][j] * 0.4 + x[3][j];
            model.add(COR_Total >= COR[j]);
            COR_Total.end();
        }
        // Resolver
        IloCplex cplex(model);
        if (!cplex.solve()) {
            env.error() << "Falha ao resolver o modelo" << endl;
            throw(-1);
        }
        
        // Resultados
        cout << "Solução ótima = " << cplex.getObjValue() << endl;
        const char* linhas[] = {"SolA", "SolB", "SEC", "COR"};
        cout << "\tSR\tSN" << endl;
        for (int i = 0; i < NUM_COMPONENTES; i++) {
            cout << linhas[i]<< "\t";
            for (int j = 0; j < NUM_TINTAS; j++) {
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