#include <bits/stdc++.h>
using namespace std;

using Item     = double;
using Bin      = vector<Item>;
using Solution = vector<Bin>;
using Fit      = pair<int,double>;  // (bins, total_waste)

struct Instance {
    string id;
    double capacity;
    int    n;
    vector<Item> items;
};

Fit fitness(const Solution &bins, double capacity) {
    int num_bins = bins.size();
    double total_waste = 0.0;
    for (const auto &b : bins) {
        double sum = accumulate(b.begin(), b.end(), 0.0);
        total_waste += (capacity - sum);
    }
    return {num_bins, total_waste};
}

Solution first_fit(const vector<Item> &items, double capacity) {
    Solution bins;
    for (auto item : items) {
        bool placed = false;
        for (auto &b : bins) {
            double sum = accumulate(b.begin(), b.end(), 0.0);
            if (sum + item <= capacity) {
                b.push_back(item);
                placed = true;
                break;
            }
        }
        if (!placed) bins.emplace_back(Bin{item});
    }
    return bins;
}

Solution local_search(const Solution &initial, double capacity, double time_limit) {
    auto start = chrono::steady_clock::now();
    auto end   = start + chrono::duration<double>(time_limit);

    Solution current   = initial;
    Fit      current_f = fitness(current, capacity);

    while (chrono::steady_clock::now() < end) {
        bool improved = false;
        for (size_t i = 0; i < current.size() && !improved; ++i) {
            for (auto item : current[i]) {
                for (size_t j = 0; j < current.size() && !improved; ++j) {
                    if (i == j) continue;
                    double sum_j = accumulate(current[j].begin(), current[j].end(), 0.0);
                    if (sum_j + item <= capacity) {
                        Solution neighbor = current;
                        auto &bin_i = neighbor[i];
                        auto &bin_j = neighbor[j];
                        bin_i.erase(find(bin_i.begin(), bin_i.end(), item));
                        bin_j.push_back(item);
                        neighbor.erase(remove_if(neighbor.begin(), neighbor.end(),
                            [](const Bin &b){ return b.empty(); }), neighbor.end());
                        Fit new_f = fitness(neighbor, capacity);
                        if (new_f < current_f) {
                            current   = move(neighbor);
                            current_f = new_f;
                            improved  = true;
                        }
                    }
                }
            }
        }
        if (!improved) break;
    }
    return current;
}

Solution perturb(const Solution &sol, double capacity, mt19937 &rng, int p) {
    vector<pair<size_t,size_t>> indices;
    for (size_t i = 0; i < sol.size(); ++i)
        for (size_t j = 0; j < sol[i].size(); ++j)
            indices.emplace_back(i,j);

    Solution pert = sol;
    vector<Item> removed;
    uniform_int_distribution<> dist(0, (int)indices.size()-1);

    for (int k = 0; k < p && !indices.empty(); ++k) {
        int idx = dist(rng);
        auto [bi,bj] = indices[idx];
        removed.push_back(pert[bi][bj]);
        pert[bi].erase(pert[bi].begin() + bj);
        if (pert[bi].empty()) pert.erase(pert.begin() + bi);
        indices.clear();
        for (size_t i = 0; i < pert.size(); ++i)
            for (size_t j = 0; j < pert[i].size(); ++j)
                indices.emplace_back(i,j);
        if (!indices.empty()) dist = uniform_int_distribution<>(0, (int)indices.size()-1);
    }

    for (auto item : removed) {
        bool placed = false;
        for (auto &b : pert) {
            double sum = accumulate(b.begin(), b.end(), 0.0);
            if (sum + item <= capacity) {
                b.push_back(item);
                placed = true;
                break;
            }
        }
        if (!placed) pert.emplace_back(Bin{item});
    }
    return pert;
}

vector<Instance> read_instances(istream &in) {
    int P; in >> P;
    vector<Instance> instances;
    instances.reserve(P);
    for (int p = 0; p < P; ++p) {
        Instance inst;
        in >> inst.id >> inst.capacity >> inst.n;
        inst.items.resize(inst.n);
        for (int i = 0; i < inst.n; ++i)
            in >> inst.items[i];
        instances.push_back(move(inst));
    }
    return instances;
}

void print_usage(const char *prog) {
    cerr << "Usage: " << prog << " -time <seconds> [-s] < <input_file>\n";
}

int main(int argc, char **argv) {
    double time_limit = 0;
    bool showBins    = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-time") == 0 && i+1 < argc) {
            time_limit = stod(argv[++i]);
        } else if (strcmp(argv[i], "-s") == 0) {
            showBins = true;
        } else {
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }
    if (time_limit <= 0) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    auto instances = read_instances(cin);
    mt19937 rng(random_device{}());
    cout << fixed << setprecision(2);

    for (auto &inst : instances) {
        Solution best = first_fit(inst.items, inst.capacity);
        Fit best_f = fitness(best, inst.capacity);
        Solution current = best;
        int perturb_p = max(1, inst.n / 10);
        auto start = chrono::steady_clock::now();
        auto end = start + chrono::duration<double>(time_limit);

        while (chrono::steady_clock::now() < end) {
            double remaining = chrono::duration<double>(end - chrono::steady_clock::now()).count();
            Solution shaken = perturb(current, inst.capacity, rng, perturb_p);
            double ls_time = min(0.1, remaining);
            Solution local = local_search(shaken, inst.capacity, ls_time);
            Fit local_f = fitness(local, inst.capacity);

            if (local_f < best_f) {
                best   = local;
                best_f = local_f;
            }
            current = local;
        }

        cout << "Instance " << inst.id << ": bins = " << best_f.first << '\n';
        if (showBins) {
            for (size_t i = 0; i < best.size(); ++i) {
                cout << "   Bin " << i+1 << ": ";
                for (auto item : best[i]) cout << item << " ";
                cout << '\n';
            }
        }
    }
    return EXIT_SUCCESS;
}
