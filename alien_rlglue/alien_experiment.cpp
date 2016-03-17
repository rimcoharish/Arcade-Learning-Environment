#include <rlglue/RL_glue.h>
#include <iostream>
#include <stdio.h>

using namespace std;

int main() {
    const char* task_spec_string = RL_init();
    cout << task_spec_string << endl;
    double total_reward = 0;
    int total_episodes = 10;
    for (int i = 0; i < total_episodes; i++) {
        int terminal=RL_episode(0);
        double reward_for_episode = RL_return();
        total_reward += reward_for_episode;
        printf("%d Steps \t%f total reward\t %d natural end \n", RL_num_steps(), reward_for_episode, terminal);
    }
    cout << "Average reward for " << total_episodes << " episodes is " << total_reward / total_episodes << endl;
    RL_cleanup();
    return 0;
}

