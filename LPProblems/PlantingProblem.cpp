#include <ilcplex/ilocplex.h>
#include <iostream>
ILOSTLBEGIN

int PlantingProblem() {
    IloEnv env;
    try {
        // Criar modelo
        IloModel model(env);

        //Constantes
        const int NUM_FAZENDAS = 3;
        const int NUM_PLANTIOS = 3;
        const int area_fazendas[NUM_FAZENDAS] = {
            400, 650, 350
        };
        const int agua_fazendas[NUM_FAZENDAS] = {
            1800, 2200, 950
        };
        const int area_max_plantios[NUM_PLANTIOS] = {
            660, 880, 400
        };
        const int lucro_plantios[NUM_PLANTIOS] = {
            5000, 4000, 1800
        };
        const double agua_por_area_plantios[NUM_PLANTIOS] = {
            5.5, 4, 3.5
        };

        // Variáveis
        IloArray <IloIntVarArray> x(env, NUM_FAZENDAS);
        for (int i = 0; i < NUM_FAZENDAS; i++) {
            x[i] = IloIntVarArray(env, NUM_PLANTIOS, 0, area_max_plantios[i]);
            for (int j = 0; j < NUM_PLANTIOS; j++) {
                x[i][j].setName(("x" + std::to_string(i + 1) + std::to_string(j + 1)).c_str());
            }
        }

        // Função objetivo: Maximizar lucro
        IloExpr lucro(env);
        for (int i = 0; i < NUM_FAZENDAS; i++) {
            for (int j = 0; j < NUM_PLANTIOS; j++) {
                lucro += lucro_plantios[j] * x[i][j];
            }
        }
        model.add(IloMaximize(env, lucro));
        lucro.end();

        // Restrições
        // Área total plantada em cada fazenda
        for (int i = 0; i < NUM_FAZENDAS; i++) {
            IloExpr area_total(env);
            for (int j = 0; j < NUM_PLANTIOS; j++) {
                area_total += x[i][j];
            }
            model.add(area_total <= area_fazendas[i]);
            area_total.end();
        }
        // Água total utilizada em cada fazenda
        for (int i = 0; i < NUM_FAZENDAS; i++) {
            IloExpr agua_total(env);
            for (int j = 0; j < NUM_PLANTIOS; j++) {
                agua_total += agua_por_area_plantios[j] * x[i][j];
            }
            model.add(agua_total <= agua_fazendas[i]);
            agua_total.end();
        }
        // Área total plantada em cada plantio
        for (int j = 0; j < NUM_PLANTIOS; j++) {
            IloExpr area_total(env);
            for (int i = 0; i < NUM_FAZENDAS; i++) {
                area_total += x[i][j];
            }
            model.add(area_total <= area_max_plantios[j]);
            area_total.end();
        }
        // Proporção igual para cada fazenda
        IloNumExprArray proporcoes(env,NUM_FAZENDAS);
        for (int i = 0; i < NUM_FAZENDAS; i++) {
            IloExpr proporcao(env);
            for (int j = 0; j < NUM_PLANTIOS; j++) {
                proporcao += x[i][j];
            }
            proporcoes[i] = proporcao / area_fazendas[i];
        }
        model.add(proporcoes[0] == proporcoes[1]);
        model.add(proporcoes[1] == proporcoes[2]);
        for (int i = 0; i < NUM_FAZENDAS; i++) {
            proporcoes[i].end();
        } 
        // Resolver
        IloCplex cplex(model);
        if (!cplex.solve()) {
            env.error() << "Falha ao resolver o modelo" << endl;
            throw(-1);
        }
        
        // Resultados
        cout << "Solução ótima = " << cplex.getObjValue() << endl;
        cout << "Quantidade de cada plantio em cada fazenda:" << endl;
        for (int i = 0; i < NUM_FAZENDAS; i++) {
            cout << "Fazenda " << i + 1 << ": ";
            for (int j = 0; j < NUM_PLANTIOS; j++) {
                cout << x[i][j].getName() << " = " << cplex.getValue(x[i][j]) << " ";
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