#include <cmaes.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <iomanip>
#include "pacman.h"

using namespace libcmaes;
using namespace std;

int iterations = 0;
int total_episodes = 10;

string rom_file = "Invalid Rom file";

FitFunc fsphere = [](const double *x, const int N)
{
    double val = 0.0;
    for (int i=0;i<N;i++)
        val += x[i]*x[i];
    return val;
};

FitFunc pacman_experiment = [](const double *x, const int N) {
    iterations++;
    cout << setprecision(10) << "Iteration: " << iterations << endl;
    cout << "Ghost Cost: " << x[0] << " ";
    cout << "Corner Cost: " << x[1] << " ";
    cout << "Teleportation Cost: " << x[2] << " ";
    cout << "Edible Ghost Cost: " << x[3] << " ";
    cout << "Pellet Cost: " << x[4] << endl;
    double fitness_value = - evaluate(rom_file, total_episodes, x, N);
    cout << "Fitness value: " << fitness_value << endl;
    return fitness_value;
};

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " rom_file" << std::endl;
        return 1;
    }
    rom_file = argv[1];
    int GHOST_COST = 5000;
    int CORNER_COST = 1000;
    int TELE_COST = 1000;
    int EDIBLE_GHOST_COST = -5000;
    int PELLET_COST = -100;
    int dim = 5; // problem dimensions.
    std::vector<double> x0;
    x0.push_back(GHOST_COST);
    x0.push_back(CORNER_COST);
    x0.push_back(TELE_COST);
    x0.push_back(EDIBLE_GHOST_COST);
    x0.push_back(PELLET_COST);
    double sigma = 0.1;
    int lambda = 20; // offsprings at each generation.
    CMAParameters<> cmaparams(x0, sigma, lambda);
    cmaparams.set_mt_feval(true);
    //cmaparams._algo = BIPOP_CMAES;
    CMASolutions cmasols = cmaes<>(pacman_experiment, cmaparams);
    std::cout << "best solution: " << cmasols << std::endl;
    std::cout << "optimization took " << cmasols.elapsed_time() / 1000.0 << " seconds\n";
    return cmasols.run_status();
}
