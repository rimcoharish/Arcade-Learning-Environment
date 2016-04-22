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
#include <mutex>

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

mutex fmtx;

FitFunc pacman_experiment = [](const double *x, const int N) {
    fmtx.lock();
    int eval_num = iterations++;
    string command = "./pacman ../../roms/ms_pacman.bin --ghost_cost 1 --corner_cost ";
    command += to_string(x[0]);
    command += " --teleportation_cost ";
    command += to_string(x[1]);
    command += " --edible_ghost_cost ";
    command += to_string(x[2]);
    command += " --pellet_cost ";
    command += to_string(x[3]);
    string output_file = "result/result_" + to_string(eval_num) + ".txt";
    command += " > " + output_file;
    cout << command << endl;
    fmtx.unlock();
    system(command.c_str());
    ifstream infile(output_file);
    double fitness_value;
    infile >> fitness_value;
    infile.close();
    cout << setprecision(10) << "Iteration: " << eval_num << ", Fitness value: " << fitness_value << endl;
    return fitness_value;
};

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " rom_file" << std::endl;
        return 1;
    }
    rom_file = argv[1];
//    double GHOST_COST = 5000 / 5000.0;
    double CORNER_COST = 1000 / 5000.0;
    double TELE_COST = 1000 / 5000.0;
    double EDIBLE_GHOST_COST = -5000 / 5000.0;
    double PELLET_COST = -100 / 5000.0;
    int dim = 4; // problem dimensions.
    std::vector<double> x0;
//    x0.push_back(GHOST_COST);
    x0.push_back(CORNER_COST);
    x0.push_back(TELE_COST);
    x0.push_back(EDIBLE_GHOST_COST);
    x0.push_back(PELLET_COST);
    double sigma = 0.1;
    int lambda = 16; // offsprings at each generation.
    CMAParameters<> cmaparams(x0, sigma, lambda);
    cmaparams.set_mt_feval(true);
    cmaparams.set_max_fevals(400);
    cmaparams.set_fplot("data.dat");
    cmaparams.set_algo(aCMAES);
    //cmaparams._algo = BIPOP_CMAES;
    CMASolutions cmasols = cmaes<>(pacman_experiment, cmaparams);
    std::cout << "best solution: " << cmasols << std::endl;
    std::cout << "optimization took " << cmasols.elapsed_time() / 1000.0 << " seconds\n";
    return cmasols.run_status();
}
