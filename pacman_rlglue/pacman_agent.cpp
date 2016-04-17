#include <rlglue/Agent_common.h>
#include <rlglue/utils/C/RLStruct_util.h>
#include <rlglue/utils/C/TaskSpec_Parser.h>
#include <bits/stdc++.h>
#include "pacman_image.h"
#include "escape_agent.h"
#include "constants.h"

using namespace std;

int num_actions = 0;
int max_action = 17;
double total_reward = 0;
action_t action;
int steps = 0;
int episode_num = 0;

pacman_image p_image;
escape_agent esc_agent;

vector<loc> prev_object_locations;

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
            if(tmp == 74)
                tmp = 1;
            else if (tmp == 144) tmp = 0;
            my_file << tmp << " ";
        }
        my_file << endl;
    }
    my_file.close();
}

void update_object_locations(vector<loc> &object_locations) {
    for (size_t i = 0; i < object_locations.size(); ++i) {
        if (std::isnan(object_locations[i].first) || std::isnan(object_locations[i].second)) {
            object_locations[i] = prev_object_locations[i];
        }
    }
}

void update_previous_locations(const vector<loc> &object_locations) {
    for (size_t i = 0; i < object_locations.size(); ++i) {
        prev_object_locations[i] = object_locations[i];
    }
}

void location_check(vector<loc> &object_locations) {
    cout << "NAN index: ";
    for (size_t i = 0; i < object_locations.size(); ++i) {
        if(std::isnan(object_locations[i].first) || std::isnan(object_locations[i].second)) {
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
    episode_num++;
    steps = 0;
    cout << "Episode " << episode_num << endl;
    cout << setprecision(10) << "Ghost Cost: " << GHOST_COST << endl;
    cout << "Corner Cost: " << CORNER_COST << endl;
    cout << "Teleportation Cost: " << TELE_COST << endl;
    cout << "Edible Ghost Cost: " << EDIBLE_GHOST_COST << endl;
    cout << "Pellet Cost: " << PELLET_COST << endl;
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
    prev_object_locations.clear();
    for (int i = 0; i < 5; ++i) {
        loc tmp_loc = make_pair(NAN, NAN);
        prev_object_locations.push_back(tmp_loc);
    }
    p_image.detect_maze_and_set(screen);
    int action_val = randInRange(max_action);
    action.intArray[0] = action_val;
    return &action;
}

const action_t* agent_step(double reward, const observation_t* observation) {
    total_reward += reward;
    steps++;
    if (steps % 4 != 0) return &action;
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
    vector<loc> object_locations = p_image.process_screen(screen);
    p_image.update_pellet_pos(screen);
    update_object_locations(object_locations);
    vector<loc> edible_ghosts = p_image.detect_edible_ghosts(screen);
    int action_val = esc_agent.get_action(p_image, object_locations, edible_ghosts);
    update_previous_locations(object_locations);
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
    string msg(message);
    if (msg.find(' ') != string::npos) {
        string token = msg.substr(0, msg.find(' '));
        double cost = stod(msg.substr(msg.find(' ')));
        if (token == "setGhostCost") {
            GHOST_COST = cost;
            return "Ghost Cost set";
        }
        else if (token == "setCornerCost") {
            CORNER_COST = cost;
            return "Corner Cost set";
        }
        else if (token == "setTeleportationCost") {
            TELE_COST = cost;
            return "Teleportation Cost set";
        }
        else if (token == "setEdibleGhostCost") {
            EDIBLE_GHOST_COST = cost;
            return "Edible Ghost Cost set";
        }
        else if (token == "setPelletCost") {
            PELLET_COST = cost;
            return "Pellet Cost set";
        }
        else {
            return "Invalid message";
        }
    }
    return "Invalid message";
}