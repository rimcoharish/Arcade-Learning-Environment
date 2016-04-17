#include <cmaes.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <fstream>

using namespace libcmaes;
using namespace std;

int iterations = 0;

FitFunc fsphere = [](const double *x, const int N)
{
    double val = 0.0;
    for (int i=0;i<N;i++)
        val += x[i]*x[i];
    return val;
};

FitFunc pacman_experiment = [](const double *x, const int N) {
    iterations++;
    cout << "Iteration: " << iterations << endl;
    pid_t rl_glue_pid = fork();
    if (rl_glue_pid == 0) {
        int status = system("rl_glue > rl_glue_out.txt");
        cout << "RL Glue status: " << status << endl;
        exit(EXIT_SUCCESS);
    }
    else {
        cout << "RL Glue pid: " << rl_glue_pid << endl;
        pid_t agent_pid = fork();
        if (agent_pid == 0) {
            int status = system("./pacman_agent > agent_out.txt");
            cout << "Agent status: " << status << endl;
            exit(EXIT_SUCCESS);
        }
        else {
            cout << "Agent pid: " << agent_pid << endl;
            string command = "./pacman_experiment --ghost_cost ";
            command += to_string(x[0]);
            command += " --corner_cost ";
            command += to_string(x[1]);
            command += " --teleportation_cost ";
            command += to_string(x[2]);
            command += " --edible_ghost_cost ";
            command += to_string(x[3]);
            command += " --pellet_cost ";
            command += to_string(x[4]);
            command += " > experiment_out.txt";
            cout << command << endl;
            pid_t experiment_pid = fork();
            if (experiment_pid == 0) {
                system(command.c_str());
                exit(EXIT_SUCCESS);
            }
            else {
                cout << "Experiment pid: " << experiment_pid << endl;
                pid_t ale_pid = fork();
                if (ale_pid == 0) {
                    system("../ale -display_screen false -game_controller rlglue ../roms/ms_pacman.bin");
                    exit(EXIT_SUCCESS);
                }
                else {
                    cout << "ALE pid: " << ale_pid << endl;
                    int returnStatus;
                    waitpid(experiment_pid, &returnStatus, 0);  // Parent process waits here for child to terminate.
                    if (returnStatus == 0) {
                        std::cout << "The child process terminated normally." << std::endl;
                    }
                    if (returnStatus == 1) {
                        std::cout << "The child process terminated with an error!." << std::endl;
                    }
                    ifstream infile("result.txt");
                    double result;
                    infile >> result;
                    cout << "Fitness value: " << result << endl;
                    return -result;
                }
            }
        }
    }
};

int main(int argc, char *argv[])
{
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
    //cmaparams._algo = BIPOP_CMAES;
    CMASolutions cmasols = cmaes<>(pacman_experiment, cmaparams);
    std::cout << "best solution: " << cmasols << std::endl;
    std::cout << "optimization took " << cmasols.elapsed_time() / 1000.0 << " seconds\n";
    return cmasols.run_status();
}
