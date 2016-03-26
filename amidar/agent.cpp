#include <rlglue/Agent_common.h>
#include <rlglue/utils/C/TaskSpec_Parser.h>
#include <rlglue/utils/C/RLStruct_util.h>
#include <bits/stdc++.h>
#include "../src/common/Constants.h"
#include "constants.h"
#include "astar.h"
#include "amidar_image.h"
#include "escape_agent.h"

using namespace std;

action_t action;
double episode_reward = 0;

amidar_image image;
escape_agent agent;

void print_image(vector<vector<int> > &screen) {
    ofstream my_file;
    my_file.open("image.txt", ofstream::out);
    for (size_t row = 0; row < screen.size(); ++row) {
        for (size_t column = 0; column < screen[row].size(); ++column) {
            int tmp = screen[row][column];
            if (tmp == MAZE_COLOR) tmp = 1;
            my_file << tmp << " ";
        }
        my_file << endl;
    }
    my_file.close();
}

void agent_init(const char* task_spec_string) {
    taskspec_t* task_spec = new taskspec_t;
    int decode_status = decode_taskspec(task_spec, task_spec_string);
    if (decode_status != 0) {
        cout << "Not able to parse task specification" << endl;
        cout << task_spec_string << endl;
        exit(1);
    }
    else allocateRLStruct(&action, 1, 0, 0);
}

const action_t* agent_start(const observation_t* observation) {
    action.intArray[0] = PLAYER_A_NOOP;
    return &action;
}

const action_t* agent_step(double reward, const observation_t* observation) {
    int screen_start_index = 128;
    int offset = 0;
    vector<vector<int> > screen;
    vector<vector<int> > full_screen;
    for (int row = 0; row < SCREEN_HEIGHT; ++row) {
        vector<int> pixel_row;
        bool maze_encountered = false;
        for (int column = 0; column < SCREEN_WIDTH; ++column) {
            if (observation->intArray[screen_start_index + offset] == MAZE_COLOR) maze_encountered = true;
            pixel_row.push_back(observation->intArray[screen_start_index + offset]);
            offset++;
        }
        if (maze_encountered) screen.push_back(pixel_row);
        full_screen.push_back(pixel_row);
    }
    print_image(screen);
    Action action_val = agent.get_action(image, screen);

    episode_reward += reward;
    action.intArray[0] = action_val;
    return &action;
}

void agent_end(double reward) {
    episode_reward += reward;
    cout << "Episode reward: " << episode_reward << endl;
    episode_reward = 0;
}
void agent_cleanup() {
    clearRLStruct(&action);
}

const char* agent_message(const char* message) {
    cout << "Message received: " << message << endl;
    return "Message received";
}