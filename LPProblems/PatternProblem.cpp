#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

enum linhas {
    TAM_FOLHAS,
    NUM_CORPO,
    NUM_TAMPA,
    TEMPO_IMPRESSAO,
};

int PatternProblem() {
    IloEnv env;
    try {
        // Criar modelo
        IloModel model(env);
        const int NUM_PADROES = 4;
        const int NUM_TIPOS_FOLHA = 2;
        const int QNTD_TIPOS_FOLHA[NUM_TIPOS_FOLHA] = {200, 90};
        const int PRECO_LATINHA = 50;
        const int CUSTO_ESTOCAGEM_CORPO = 50;
        const int CUSTO_TAMPA = 3;
        const int TABELA[][NUM_PADROES] = 
        {
            {1, 2, 1, 1},
            {1, 2, 0, 4},
            {7, 3, 9, 4},
            {2, 3, 2, 1},
        };

        // Variáveis de decisão
        IloIntVarArray x(env, NUM_PADROES, 0, 200);
        
        // Função objetivo
        // O número de latinhas vai ser o mínimo entre o somatório do número de corpos e o somatório do número de tampas/2.
        // min(C, T/2)
        // Com o número de latinhas, podemos calcular o custo de estocagem dos excedentes
        // Corpos não usado = C - L
        // Tampas não usadas = T - 2L
        // A função objetivo será o lucro com as latinhas menos os custos de estocagem
        // 50L - 50(C - L) - 3(T - 2L)
        // 50L - 50C + 50L - 3T + 6L
        // 106L - 50C - 3T
        
        // Calculando o número de corpos
        IloExpr corpos(env);
        for (int i = 0; i < NUM_PADROES; i++) {
            corpos += TABELA[NUM_CORPO][i] * x[i];
        }
        // Calculando o número de tampas
        IloExpr tampas(env);
        for (int i = 0; i < NUM_PADROES; i++) {
            tampas += TABELA[NUM_TAMPA][i] * x[i];
        }
        // Calculando o número de latinhas
        IloExpr latinhas(env);
        latinhas = IloMin(corpos, tampas / 2);

        // Função objetivo
        IloExpr lucro(env);
        lucro = PRECO_LATINHA * latinhas - CUSTO_ESTOCAGEM_CORPO * (corpos - latinhas) - CUSTO_TAMPA * (tampas - 2 * latinhas);
        
        model.add(IloMaximize(env, lucro));
        
        corpos.end();
        tampas.end();
        latinhas.end();
        lucro.end();



        // Restrições
        // Folhas de metal
        // A fábrica possui 200 folhas de metal de tam 1 e 90 de tam 2
        for (int i = 0; i < NUM_TIPOS_FOLHA; i++) {
            IloExpr folhas(env);
            for (int j = 0; j < NUM_PADROES; j++) {
                if (TABELA[TAM_FOLHAS][j] == i+1) {
                    folhas += x[j];
                }
            }
            model.add(folhas <= QNTD_TIPOS_FOLHA[i]);
            folhas.end();
        }
        // Resolver
        IloCplex cplex(model);
        if (!cplex.solve()) {
            env.error() << "Falha ao resolver o modelo" << endl;
            throw(-1);
        }
        
        // Resultados
        cout << "Solução ótima = " << cplex.getObjValue() << endl;
        int tempo = 0;
        for (int i = 0; i < NUM_PADROES; i++) {
            cout << "Quantidade padrão "<< i+1 << " = " << cplex.getValue(x[i]) << endl;
            tempo += cplex.getValue(x[i]) * TABELA[TEMPO_IMPRESSAO][i];
        }
        cout << "Tempo total de impressão = " << tempo << endl;
        
        
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