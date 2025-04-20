/*
 * Implementação de uma Meta-heurística de Solução Única para o Bin Packing.
 * (Iterated Local Search - ILS)
 * 
 * 1. Representação da Solução:
 * - Uma solução é representada por "Solution", que é um "vector<Bin>".
 * - Cada "Bin" (um "vector<Item>") contém os itens (seus tamanhos "double") alocados naquele recipiente.
 *
 * 2. Função de Avaliação:
 * - A função "fitness(Solution, capacity)" calcula a qualidade de uma solução.
 * - Retorna um "pair<int, double>":
 * - first: O número de bins utilizados - objetivo primário a minimizar.
 * - second: O desperdício total (soma das capacidades não utilizadas em cada bin) - objetivo secundário a minimizar.
 * - Comparações entre "Fit" priorizam o número de bins, depois o desperdício.
 *
 * 3. Geração da Solução Inicial:
 * - A heurística Best Fit é usada.
 * - Cada item é inserido no bin onde ele melhor se encaixa (deixando o menor espaço residual),
 *   dentre os bins onde ele cabe. Se não couber em nenhum, um novo bin é criado.
 *
 * 4. Estratégia de Busca Local:
 * - A função "local_search" implementa a busca local.
 * - Vizinhança Explorada: Foca em bins que contêm *apenas um item*. Tenta mover este item
 * para outro bin existente onde ele caiba, escolhendo o bin destino que resulta no menor
 * espaço residual após a inserção (estratégia 'best fit' para a movimentação).
 * - Tipo de Busca: É uma forma de Best Improvement dentro da vizinhança. A busca
 * para assim que uma melhoria é encontrada. O processo se repete até que nenhuma melhoria 
 * desse tipo seja possível ou o tempo limite se esgote.
 *
 * 5. Perturbação:
 * - A função "perturb" modifica a solução atual para escapar de ótimos locais.
 * - Estratégia: Remove "p" itens aleatoriamente de seus bins atuais.
 * - Os itens removidos são então reinseridos na solução usando a heurística Best Fit.
 *
 * 6. Critério de Parada:
 * - O algoritmo executa iterações de perturbação e busca local até que um tempo limite
 * global ("time_limit"), fornecido como argumento de linha de comando, seja atingido.
 *
 */

 #include <iostream>
 #include <vector>
 #include <numeric>
 #include <algorithm>
 #include <chrono>
 #include <random>
 #include <string>
 #include <iomanip>
 #include <utility>
 #include <cstring>
 #include <limits>
 
 using namespace std;
 
 using Item = double;
 using Bin = vector<Item>;
 using Solution = vector<Bin>;
 using Fit = pair<int, double>;
 
 struct Instance {
     double capacity;
     int n;
     vector<Item> items;
 };
 
 Fit fitness(const Solution &bins, double capacity) {
     if (bins.empty()) {
         return {0, 0};
     }
 
     int num_bins = bins.size();
     double total_waste = 0.0;
     for (const auto &b : bins) {
         double sum = accumulate(b.begin(), b.end(), 0.0);
         total_waste += (capacity - sum);
     }
 
     return {num_bins, total_waste};
 }
 
 Solution best_fit(const vector<Item> &items, double capacity) {
     Solution bins;
     vector<double> bin_sums;
 
     for (auto item : items) {
         int best_idx = -1;
         double min_remain = capacity + 1.0;
 
         for (size_t i = 0; i < bins.size(); ++i) {
             if (bin_sums[i] + item <= capacity) {
                 double remain = capacity - (bin_sums[i] + item);
                 if (remain < min_remain) {
                     min_remain = remain;
                     best_idx = i;
                 }
             }
         }
 
         if (best_idx != -1) {
             bins[best_idx].push_back(item);
             bin_sums[best_idx] += item;
         }
         else {
             bins.emplace_back(Bin{item});
             bin_sums.push_back(item);
         }
     }
     return bins;
 }
 
 Solution local_search(Solution current, double capacity, double time_limit) {
     auto start = chrono::steady_clock::now();
     auto end = start + chrono::duration<double>(time_limit);
 
     bool improved;
     do {
         if (chrono::steady_clock::now() >= end) break;
 
         improved = false;
         int best_src_bin = -1;
         int best_item_idx = -1;
         int best_dest_bin = -1;
 
         for (size_t i = 0; i < current.size(); ++i) {
             if (current[i].size() == 1) {
                 Item item = current[i][0];
 
                 int current_best_j = -1;
                 double min_remain_in_j = capacity + 1.0;
 
                 for (size_t j = 0; j < current.size(); ++j) {
                     if (i == j) continue;
 
                     double sum_j = accumulate(current[j].begin(), current[j].end(), 0.0);
                     if (sum_j + item <= capacity) {
                         double remain = capacity - (sum_j + item);
                         if (remain < min_remain_in_j) {
                             min_remain_in_j = remain;
                             current_best_j = j;
                         }
                     }
                 }
 
                 if (current_best_j != -1) {
                     best_src_bin = i;
                     best_item_idx = 0;
                     best_dest_bin = current_best_j;
                     improved = true;
                     break;
                 }
             }
         }
 
         if (improved) {
             Item item = current[best_src_bin][best_item_idx];
             current[best_dest_bin].push_back(item);
             current.erase(current.begin() + best_src_bin);
         } else {
             break;
         }
 
     } while (chrono::steady_clock::now() < end);
 
     current.erase(remove_if(current.begin(), current.end(), [](const Bin &b) { return b.empty(); }), current.end());
 
     return current;
 }
 
 
 Solution perturb(Solution pert, double capacity, mt19937 &rng, int p) {
     vector<Item> removed_items;
 
     vector<pair<size_t, size_t>> item_coords;
     for (size_t i = 0; i < pert.size(); ++i) {
         for (size_t j = 0; j < pert[i].size(); ++j) {
             item_coords.push_back({i, j});
         }
     }
 
     if (item_coords.empty()) return pert;
 
     shuffle(item_coords.begin(), item_coords.end(), rng);
     int num_to_remove = min((int)item_coords.size(), p);
 
     vector<pair<size_t, size_t>> coords_to_remove;
     for (int k = 0; k < num_to_remove; ++k) {
         coords_to_remove.push_back(item_coords[k]);
     }
     sort(coords_to_remove.begin(), coords_to_remove.end(), [](const auto& a, const auto& b){
         if (a.first != b.first) return a.first > b.first;
         return a.second > b.second;
     });
 
     for(const auto& coord : coords_to_remove) {
         size_t bin_idx = coord.first;
         size_t item_idx = coord.second;
         if (bin_idx < pert.size() && item_idx < pert[bin_idx].size()) {
              removed_items.push_back(pert[bin_idx][item_idx]);
              pert[bin_idx].erase(pert[bin_idx].begin() + item_idx);
         }
     }
 
 
     pert.erase(remove_if(pert.begin(), pert.end(), [](const Bin &b) { return b.empty(); }), pert.end());
 
     vector<double> bin_sums;
     for(const auto& bin : pert) {
         bin_sums.push_back(accumulate(bin.begin(), bin.end(), 0.0));
     }
 
     shuffle(removed_items.begin(), removed_items.end(), rng);
 
     for (auto item : removed_items) {
         int best_idx = -1;
         double min_remain = capacity + 1.0;
 
         for (size_t i = 0; i < pert.size(); ++i) {
             if (bin_sums[i] + item <= capacity) {
                 double remain = capacity - (bin_sums[i] + item);
                 if (remain < min_remain) {
                     min_remain = remain;
                     best_idx = i;
                 }
             }
         }
 
         if (best_idx != -1) {
             pert[best_idx].push_back(item);
             bin_sums[best_idx] += item;
         } else {
             pert.emplace_back(Bin{item});
             bin_sums.push_back(item);
         }
     }
 
     return pert;
 }
 
 // Leitura dos parametros
 vector<Instance> read_instances(istream &in) {
     int P;
     in >> P;
     vector<Instance> instances;
     instances.reserve(P);
     for (int p = 0; p < P; ++p) {
         Instance inst;
         in >> inst.capacity >> inst.n;
         inst.items.resize(inst.n);
         for (int i = 0; i < inst.n; ++i) {
             in >> inst.items[i];
         }
         instances.push_back(move(inst));
     }
     return instances;
 }
 
 // Help dos parametros
 void print_usage(const char *prog) {
     cerr << "Uso: " << prog << " -time <segundos> [-s] < <arquivo_entrada>\n";
     cerr << "  -time <segundos>: Tempo limite total em segundos (obrigatorio, > 0).\n";
     cerr << "  -s              : Mostrar a composicao dos bins na saida (opcional).\n";
     cerr << "  <arquivo_entrada>: Arquivo contendo as instancias.\n";
 }
 
 int main(int argc, char **argv) {
     ios_base::sync_with_stdio(0);
     cin.tie(NULL);
 
     // Identificação dos parametros para execução do código
     double time_limit = 0;
     bool show_solution = false;
 
     for (int i = 1; i < argc; ++i) {
         if (strcmp(argv[i], "-time") == 0 && i + 1 < argc) {
             time_limit = stod(argv[++i]);
         }
         else if (strcmp(argv[i], "-s") == 0) {
             show_solution = true;
         }
     }
 
     if (time_limit <= 0) {
         print_usage(argv[0]);
         return EXIT_FAILURE;
     }
 
     // Leitura das instancias
     vector<Instance> instances = read_instances(cin);
     random_device rd;
     mt19937 rng(rd());
     cout << fixed << setprecision(2);
 
     auto start = chrono::steady_clock::now();
     auto end = start + chrono::duration<double>(time_limit);
 
     int inst_counter = 0;
     for (auto &inst : instances) {
         inst_counter++;
         if (chrono::steady_clock::now() >= end) break;
 
         auto instance_start_time = chrono::steady_clock::now();
 
         // Solução inicial de forma gulosa via best fit
         Solution best_sol = best_fit(inst.items, inst.capacity);
         Fit best_fit = fitness(best_sol, inst.capacity);
         Solution current_sol = best_sol;
 
         int perturb_strength = max(1, (int)(inst.n * 0.05));
 
         cout << "Instancia " << inst_counter << '\n';
         cout << "   -> Solucao Inicial: Bins=" << best_fit.first << ", Waste=" << best_fit.second << '\n';
 
         int iterations = 0;
         while (chrono::steady_clock::now() < end) {
             iterations++;
             double time_left_global = chrono::duration<double>(end - chrono::steady_clock::now()).count();
             if (time_left_global <= 0) break;
 
             // '1. Perturbação da solução atual
             Solution perturbed_sol = perturb(current_sol, inst.capacity, rng, perturb_strength);
 
             double ls_time = min({0.1, time_left_global * 0.1, time_left_global - 0.01});
             if (ls_time <= 0) break;
 
             // '2. Busca local a partir da solução
             Solution local_opt_sol = local_search(perturbed_sol, inst.capacity, ls_time);
 
             // '3. Avaliação da melhor solução vizinha
             Fit local_opt_fit = fitness(local_opt_sol, inst.capacity);
 
 
             // '3. Aceitação da solução (caso melhor)
             if (local_opt_fit < best_fit) {
                 best_sol = local_opt_sol;
                 best_fit = local_opt_fit;
                 current_sol = local_opt_sol;
                 cout << "   -> Melhoria! Bins: " << best_fit.first << " (Iter: " << iterations << ")" << '\n';
             } 
             else {
                 current_sol = local_opt_sol;
             }
             
             // '4. Critério de parada
             if (chrono::steady_clock::now() >= end) break;
         }
 
         auto instance_end_time = chrono::steady_clock::now();
         chrono::duration<double> instance_duration = instance_end_time - instance_start_time;
 
         cout << "Instancia " << inst_counter << ": Tempo=" << instance_duration.count() << "s, Iter=" << iterations
              << ", Bins=" << best_fit.first << ", Waste=" << best_fit.second << '\n';
 
         if (show_solution) {
             for (size_t i = 0; i < best_sol.size(); ++i) {
                 cout << "  Bin " << setw(3) << i + 1 << " (" << fixed << setprecision(2) << accumulate(best_sol[i].begin(), best_sol[i].end(), 0.0) << "/" << inst.capacity << "): ";
                 for (auto item : best_sol[i]) {
                     cout << item << " ";
                 }
                 cout << '\n';
             }
         }
         cout << "---------------------------------------------------" << '\n';
 
     }
 
     return EXIT_SUCCESS;
 }