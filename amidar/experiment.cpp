#include <rlglue/RL_glue.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <vector>

using namespace std;

int main() {
    const char* task_spec_string = RL_init();
    cout << task_spec_string << endl;
    double total_reward = 0;
    int total_episodes = 100;
    vector<double> rewards;
    for (int i = 0; i < total_episodes; i++) {
        int terminal=RL_episode(0);
        double reward_for_episode = RL_return();
        total_reward += reward_for_episode;
        rewards.push_back(reward_for_episode);
        printf("Episode: %d, %d Steps \t%f total reward\t %d natural end \n", i + 1, RL_num_steps(),
               reward_for_episode, terminal);
    }
    double mean_reward = total_reward / total_episodes;
    cout << "Average reward for " << total_episodes << " episodes is " << mean_reward << endl;
    double sd = 0;
    for (int i = 0; i < rewards.size(); ++i) {
        sd += pow(rewards[i] - mean_reward, 2);
    }
    sd = sqrt(sd / (rewards.size() - 1));
    cout << "Standard deviation " << sd << endl;
    RL_cleanup();
    return 0;
}
