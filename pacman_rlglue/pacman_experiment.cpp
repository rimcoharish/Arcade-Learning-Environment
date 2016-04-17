#include <rlglue/RL_glue.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <string.h>
#include <fstream>
#include <iomanip>

using namespace std;

void parse_flags(int argc, char** argv) {
    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "--ghost_cost") == 0) {
            char msg[100];
            strcpy(msg, "setGhostCost ");
            strcat(msg, argv[i + 1]);
            const char* ghost_cost_msg = RL_agent_message(msg);
            if (strcmp(ghost_cost_msg, "Ghost Cost set") != 0) exit(0);
            i++;
        }
        else if (strcmp(argv[i], "--teleportation_cost") == 0) {
            char msg[100];
            strcpy(msg, "setTeleportationCost ");
            strcat(msg, argv[i + 1]);
            const char* ghost_cost_msg = RL_agent_message(msg);
            if (strcmp(ghost_cost_msg, "Teleportation Cost set") != 0) exit(0);
            i++;
        }
        else if (strcmp(argv[i], "--edible_ghost_cost") == 0) {
            char msg[100];
            strcpy(msg, "setEdibleGhostCost ");
            strcat(msg, argv[i + 1]);
            const char* ghost_cost_msg = RL_agent_message(msg);
            if (strcmp(ghost_cost_msg, "Edible Ghost Cost set") != 0) exit(0);
            i++;
        }
        else if (strcmp(argv[i], "--corner_cost") == 0) {
            char msg[100];
            strcpy(msg, "setCornerCost ");
            strcat(msg, argv[i + 1]);
            const char* ghost_cost_msg = RL_agent_message(msg);
            if (strcmp(ghost_cost_msg, "Corner Cost set") != 0) exit(0);
            i++;
        }
        else if (strcmp(argv[i], "--pellet_cost") == 0) {
            char msg[100];
            strcpy(msg, "setPelletCost ");
            strcat(msg, argv[i + 1]);
            const char* ghost_cost_msg = RL_agent_message(msg);
            if (strcmp(ghost_cost_msg, "Pellet Cost set") != 0) exit(0);
            i++;
        }
    }
}

int main(int argc, char * argv[]) {
    const char* task_spec_string = RL_init();
    cout << task_spec_string << endl;
    double total_reward = 0;
    int total_episodes = 10;
    vector<double> rewards;
    parse_flags(argc, argv);
    for (int i = 0; i < total_episodes; i++) {
        int terminal=RL_episode(0);
        double reward_for_episode = RL_return();
        total_reward += reward_for_episode;
        rewards.push_back(reward_for_episode);
        printf("%d Steps \t%f total reward\t %d natural end", RL_num_steps(), reward_for_episode, terminal);
        cout << endl;
    }
    double mean_reward = total_reward / total_episodes;
    cout << "Average reward for " << total_episodes << " episodes is " << mean_reward << endl;
    double sd = 0;
    for (int i = 0; i < rewards.size(); ++i) {
        sd += pow(rewards[i] - mean_reward, 2);
    }
    sd = sqrt(sd / (rewards.size() - 1));
    cout << "Standard deviation " << sd << endl;
    ofstream result_file;
    result_file.open("result.txt", ofstream::out);
    result_file << std::setprecision(10) << mean_reward - (sd / sqrt(total_episodes)) << endl; // True Mean
    result_file.close();
    RL_cleanup();
    return 0;
}

