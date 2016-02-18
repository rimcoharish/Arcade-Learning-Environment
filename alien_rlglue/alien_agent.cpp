#include <rlglue/Agent_common.h>
#include <rlglue/utils/C/RLStruct_util.h>
#include <rlglue/utils/C/TaskSpec_Parser.h>
#include <bits/stdc++.h>
#include "alien_image.h"
#include "constants.h"

using namespace std;

int num_actions = 0;
int max_action = 17;
double total_reward = 0;
action_t action;

alien_image a_image;

int randInRange(int max){
    double r, x;
    r = ((double)rand() / ((double)(RAND_MAX)+(double)(1)));
    x = (r * (max+1));
    return (int)x;
}

void print_image(vector<vector<int> > &screen) {
    ofstream my_file;
    my_file.open("image.txt", ofstream::out);
    for (size_t row = 0; row < screen.size(); ++row) {
        for (size_t column = 0; column < screen[row].size(); ++column) {
            int tmp = screen[row][column];
            if(tmp == MAZE_COLOR)
                tmp = 1;
            else if (tmp == BACKGROUND_COLOR) tmp = 0;
            my_file << tmp << " ";
        }
        my_file << endl;
    }
    my_file.close();
}


void location_check(vector<loc> &object_locations) {
    cout << "NAN index: ";
    for (size_t i = 0; i < object_locations.size(); ++i) {
        if(isnan(object_locations[i].first) || isnan(object_locations[i].second)) {
            cout << i << " ";
        }
    }
    cout << endl;
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
    print_image(screen);
    // vector<vector<int> > maze = detect_maze(screen);
    /* printing maze pattern
    for (int row = 0; row < maze.size(); ++row) {
        for (int column = 0; column < maze[row].size(); ++column) {
            cout << maze[row][column] << " ";
        }
        cout << endl;
    }
    */
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
    print_image(screen);
    total_reward += reward;
    int action_val = randInRange(max_action);
    action.intArray[0] = action_val;
    return &action;
}


void agent_end(double reward) {
    total_reward += reward;
    cout << "Total reward obtained " << total_reward << endl;
    total_reward = 0;
}

void agent_cleanup() {
    clearRLStruct(&action);
}

const char* agent_message(const char* message) {
    cout << "Message received: " << message << endl;
    return "Message received";
}