#include <rlglue/Agent_common.h>
#include <rlglue/utils/C/RLStruct_util.h>
#include <rlglue/utils/C/TaskSpec_Parser.h>
#include <iostream>
#include <time.h>
#include "pacman_image.h"
#include "constants.h"

using namespace std;

int num_actions = 0;
int max_action = 17;
double total_reward = 0;
action_t action;

pacman_image p_image;

int randInRange(int max){
    double r, x;
    r = ((double)rand() / ((double)(RAND_MAX)+(double)(1)));
    x = (r * (max+1));
    return (int)x;
}

void agent_init(const char* task_spec_string) {
    taskspec_t* task_spec = new taskspec_t;
    int decode_status = decode_taskspec(task_spec, task_spec_string);
    if (decode_status != 0) {
        cout << "Not able to parse task specification" << endl;
        cout << task_spec_string << endl;
        exit(1);
    }
    else {
        srand(time(0));
        allocateRLStruct(&action, 1, 0, 0);
    }
}

const action_t* agent_start(const observation_t* observation) {
    int screen_start_index = 128;
    int offset = 0;
    vector<vector<int> > screen;
    for (int row = 0; row < SCREEN_HEIGHT; ++row) {
        vector<int> pixel_row;
        for (int column = 0; column < SCREEN_WIDTH; ++column) {
            pixel_row.push_back(observation->intArray[screen_start_index + offset]);
            offset++;
        }
        screen.push_back(pixel_row);
    }
    vector<vector<int> > maze = detect_maze(screen);
    /* printing maze pattern
    for (int row = 0; row < maze.size(); ++row) {
        for (int column = 0; column < maze[row].size(); ++column) {
            cout << maze[row][column] << " ";
        }
        cout << endl;
    }
    */
    p_image.set_maze(maze);
    int action_val = randInRange(max_action);
    action.intArray[0] = action_val;
    return &action;
}

const action_t* agent_step(double reward, const observation_t* observation) {
    int screen_start_index = 128;
    int offset = 0;
    vector<vector<int> > screen;
    for (int row = 0; row < SCREEN_HEIGHT; ++row) {
        vector<int> pixel_row;
        for (int column = 0; column < SCREEN_WIDTH; ++column) {
            pixel_row.push_back(observation->intArray[screen_start_index + offset]);
            offset++;
        }
        screen.push_back(pixel_row);
    }
    pair<double, double> location = p_image.detect_pacman_loc(screen);
    cout << location.first << ", " << location.second << endl;
    total_reward += reward;
    int action_val = randInRange(max_action);
    action.intArray[0] = action_val;
    return &action;
}

void agent_end(double reward) {
    total_reward += reward;
    cout << "Total reward obtained " << total_reward << endl;
    clearRLStruct(&action);
}

void agent_cleanup() {
    clearRLStruct(&action);
}

const char* agent_message(const char* message) {
    cout << "Message received: " << message << endl;
    return "Message received";
}