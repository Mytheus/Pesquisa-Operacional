#include <ilcplex/ilocplex.h>
#include "Problems.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <functional>
using namespace std;

int main() {
    const vector<string> PROBLEMS= {
        "RationProblem",
        "DietProblem",
        "PlantingProblem"
    };
    const int NUM_PROBLEMS = PROBLEMS.size();
    vector<function<void(void)>> functions = {RationProblem, DietProblem, PlantingProblem};
    while (true) {
        int problemChoice;
        cout << "Escolha o problema para executar:\n";
        for (int i = 0; i < NUM_PROBLEMS; i++) {
            cout << i + 1 << ". " << PROBLEMS[i] << endl;
        }
        cout << NUM_PROBLEMS + 1 << ". Sair\n";
        cout << "Digite o número do problema: ";
        cin >> problemChoice;

        if (problemChoice < 1 || problemChoice > NUM_PROBLEMS + 1) {
            cout << "Opção inválida. Tente novamente.\n";
            continue;
        }
        else if (problemChoice == NUM_PROBLEMS + 1) {
            cout << "Saindo...\n";
            break;
        }

        else {
            cout << "Executando " << PROBLEMS[problemChoice - 1] << "...\n";
            functions[problemChoice - 1]();
        }
    }
}